// Multi server TCP
#include <winsock2.h>
#include <ws2tcpip.h>

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib") 

#define PORT 9090
#define BUFFER_SIZE 1024

#define okay(msg,...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg,...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg,...) printf("[*]" msg "\n", ##__VA_ARGS__)

// struct for every client 
typedef struct 
{
    int socket;
    int id;
}ClientData;

// Thread routine for client connection
DWORD WINAPI handle_client(LPVOID lpParam){
    // buffer for receiving messages
    char buffer[BUFFER_SIZE] = {0}; 
    // Created client socket
    ClientData client = *(ClientData*)lpParam;
    int sock_client = client.socket;
    int Client_Id = client.id;
    // Receive message from client
    if(recv(sock_client,buffer,sizeof(buffer),0) == SOCKET_ERROR){
        error("recv function failed with error: %d",WSAGetLastError());
        return EXIT_FAILURE;
    }
    //if client exists:
    info("Kitten number %d said: %s",Client_Id,buffer);
    // threaded server response 
    send(sock_client, "Meow, Kitten ₍^. .^₎⟆ !", strlen("Meow, Kitten ₍^. .^₎⟆ !"), 0);
    closesocket(sock_client);
    free(lpParam);
    return 0;
}

DWORD WINAPI serverSend(LPVOID lpParam){
    // Created client socket
    int sock_client = *(int*)lpParam;
    send(sock_client, "Meow, Kitten ₍^. .^₎⟆ !", strlen("Meow, Kitten ₍^. .^₎⟆ !"), 0);
    return 0;
}

int main(){
    // variables 
    WSADATA wsaDATA;
    int server_fd = 0, new_socket = 0;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // initiates use of the Winsock DLL by a process
    if (WSAStartup(MAKEWORD(2,2), &wsaDATA) != 0)
    {
        error("WSAStartup failed with error: %d", WSAGetLastError());
        return EXIT_FAILURE;
    }

    //Confirm that the WinSock DLL supports 2.2.
    if (LOBYTE(wsaDATA.wVersion)!=2 || HIBYTE(wsaDATA.wVersion)!=2)
    {
        error("Could not find a usable version of Winsock.dll");
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("The Winsock 2.2 dll was found");

    // making the server, creating the socket
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd == INVALID_SOCKET )
    {
        error("socket creation failed, error:%d",WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("created the socket");

    //setting socket options:
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    //binding
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //If socket error occured:
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        error("Bind failed with error: %d", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Socket bound to port %d", PORT);

    // listening :
    if (listen(server_fd,10))
    {
        error("Listen function failed with error: %d",WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return EXIT_FAILURE;
    }
    info("Threaded server listening on port %d...", PORT);
    while (1)
    {
        int* new_sock = malloc(sizeof(int));
        *new_sock = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        static int i = 1;
        ClientData* client = malloc(sizeof(ClientData));
        client->id = i;
        client->socket = *new_sock;

        HANDLE thread = CreateThread(
            NULL,              // default security
            0,                 // default stack size
            handle_client,     // function to run
            (LPVOID)client,    // parameter to pass
            0,                 // run immediately
            NULL               // don't need the thread ID
        );

        if (thread == NULL) {
            error("Failed to create thread: %d", GetLastError());
            free(client);
            closesocket(*new_sock);
        }
        free(new_sock);
        i++;
    }
    closesocket(server_fd);
    WSACleanup();
    return EXIT_FAILURE;
}