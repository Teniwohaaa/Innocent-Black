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

#define okay(msg, ...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*]" msg "\n", ##__VA_ARGS__)

// struct for every client
typedef struct
{
    SOCKET socket;
    int id;
} ClientData;

// Thread routine for client connection
DWORD WINAPI handle_client(LPVOID lpParam)
{
    // buffer for receiving messages
    char buffer[BUFFER_SIZE] = {0};
    // Created client socket
    ClientData client = *(ClientData *)lpParam;
    int bytes = recv(client.socket, buffer, sizeof(buffer), 0);
    // Receive message from client
    if (bytes == SOCKET_ERROR)
    {
        error("recv function failed with error: %d", WSAGetLastError());
    }
    // if client exists:
    info("Kitten number %d said: %s", client.id, buffer);
    // threaded server response
    send(client.socket, "Meow, Kitten ₍^. .^₎⟆ !", strlen("Meow, Kitten ₍^. .^₎⟆ !"), 0);
    closesocket(client.socket);
    free(lpParam);
    return 0;
}

// DWORD WINAPI serverSend(LPVOID lpParam)
// {

//     int sock_client = *(int *)lpParam;
//     send(sock_client, "Meow, Kitten ₍^. .^₎⟆ !", strlen("Meow, Kitten ₍^. .^₎⟆ !"), 0);
//     return 0;
// }

int main(void)
{
    WSADATA wsaDATA;
    SOCKET server_fd = INVALID_SOCKET;

    // initiates use of the Winsock DLL by a process
    if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0)
    {
        error("WSAStartup failed with error: %d", WSAGetLastError());
        return EXIT_FAILURE;
    }

    // Confirm that the WinSock DLL supports 2.2.
    if (LOBYTE(wsaDATA.wVersion) != 2 || HIBYTE(wsaDATA.wVersion) != 2)
    {
        error("Could not find a usable version of Winsock.dll");
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Winsock 2.2 initilized");

    // making the server, creating the socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET)
    {
        error("socket creation failed, error:%d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Server socket created");

    // setting socket options:
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
    // binding
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // If socket error occured:
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        error("Bind failed with error: %d", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Socket bound to port %d", PORT);

    // listening :
    if (listen(server_fd, 10) == SOCKET_ERROR)
    {
        error("Listen function failed with error: %d", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return EXIT_FAILURE;
    }
    info("Server listening on port %d...", PORT);
    int client_id = 1;
    while (1)
    {
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);

        SOCKET client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (client_socket == INVALID_SOCKET)
        {
            error("Accept failed: %d", WSAGetLastError());
            continue;
        }

        ClientData *client = malloc(sizeof(ClientData));
        client->socket = client_socket;
        client->id = client_id++;

        HANDLE thread = CreateThread(NULL, 0, handle_client, client, 0, NULL);
        if (thread == NULL)
        {
            error("Thread creation failed: %lu", GetLastError());
            closesocket(client_socket);
            free(client);
        }
        else
        {
            CloseHandle(thread);
        }
    }
    closesocket(server_fd);
    WSACleanup();
    return EXIT_FAILURE;
}