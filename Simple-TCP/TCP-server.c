// Simple TCP server (Windows)
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

#define PORT 9090
#define BUFFER_SIZE 1024

#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*] " msg "\n", ##__VA_ARGS__)

int main(void)
{
    WSADATA wsaDATA;
    SOCKET listen_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    int addrlen;
    // initializing WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0)
    {
        error("WSAStartup failed with error: %d", WSAGetLastError());
        return EXIT_FAILURE;
    }
    // Confirm that the WinSock DLL supports 2.2
    if (LOBYTE(wsaDATA.wVersion) != 2 || HIBYTE(wsaDATA.wVersion) != 2)
    {
        error("Unsupported Winsock version");
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Winsock 2.2 initialized");
    // step 1: create a socket
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET)
    {
        error("Socket creation failed: %d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Socket created");
    // step 2: setting socket options
    int opt = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    // step 3: bind
    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        error("Bind failed: %d", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Socket bound to port %d", PORT);
    // step 4: listen
    if (listen(listen_socket, 3) == SOCKET_ERROR)
    {
        error("Listen failed: %d", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }
    info("Listening on port %d...", PORT);
    // step 5: accept and reply
    addrlen = sizeof(client_addr);
    client_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &addrlen);
    if (client_socket == INVALID_SOCKET)
    {
        error("Accept failed: %d", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Client connected");

    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == SOCKET_ERROR)
    {
        error("recv failed: %d", WSAGetLastError());
    }
    else
    {
        buffer[bytes_received] = '\0';
        okay("Kitten's message: %s", buffer);
        send(client_socket, "Meow, Kitten ₍^. .^₎⟆ !", 27, 0);
    }

    closesocket(client_socket);
    closesocket(listen_socket);
    WSACleanup();

    return EXIT_SUCCESS;
}