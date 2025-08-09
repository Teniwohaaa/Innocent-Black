// server TCP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib") 

#define PORT 9090
#define BUFFER_SIZE 1024

#define okay(msg,...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg,...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg,...) printf("[*]" msg "\n", ##__VA_ARGS__)

int main(){
    // initializing WinSock and variables 
    WSADATA wsaDATA;
    int server_fd = NULL, new_socket = NULL;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};
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
    
    //step 1: create a socket 
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd == INVALID_SOCKET)
    {
        error("socket creation failed, error:%d",WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Created the Socket");
    // step 2: setting socket options
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    // step 3: bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Bind to any address
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        error("Bind failed with error: %d", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Socket bound to port %d", PORT);
    // step 4: listen
    listen(server_fd, 3); // Listen for incoming connections, max 3 queued connections
    // step 5: accept and reply
    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket == INVALID_SOCKET)
    {
        error("Accept failed with error: %d", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return EXIT_FAILURE;
    }
    int bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == SOCKET_ERROR) {
        error("recv failed with error: %d", WSAGetLastError());
    } else {
        buffer[bytes_received] = '\0';
        okay("Kitten's message: %s", buffer);
        send(new_socket, "Meow, Kitten ₍^. .^₎⟆ !", strlen("Meow, Kitten ₍^. .^₎⟆ !"), 0);
    }
    // step 6: close
    closesocket(new_socket);
    closesocket(server_fd);
    // cleanup
     WSACleanup();

   return EXIT_SUCCESS; 
}