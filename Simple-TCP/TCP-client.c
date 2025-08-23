// client TCP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

#define PORT 9090
#define okay(msg, ...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*]" msg "\n", ##__VA_ARGS__)

int main()
{
    // initializing WinSock and variables
    WSADATA wsaDATA;
    SOCKET socketClient = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char *msg = "Meow, Master ₍^. .^₎⟆ !";

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
    okay("The Winsock 2.2 dll was found");

    // step 1: Create a socket
    socketClient = socket(AF_INET, SOCK_STREAM, 0);
    if (socketClient == INVALID_SOCKET)
    {
        error("Socket creation failed (INVALID_SOCKET), error:%d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Created the client socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (serv_addr.sin_addr.s_addr == INADDR_NONE)
    {
        error("Invalid IP address");
        closesocket(socketClient);
        WSACleanup();
        return EXIT_FAILURE;
    }
    // step 2: Connect to the servers

    if (connect(socketClient, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
    {
        error("Connection failed, error:%d", WSAGetLastError());
        closesocket(socketClient);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Connected to the server");

    // step 3: send and receive
    send(socketClient, msg, strlen(msg), 0);
    recv(socketClient, buffer, 1024, 0);
    okay("Daddy's reply: %s", buffer);

    closesocket(socketClient);
    WSACleanup();
    return EXIT_SUCCESS;
}