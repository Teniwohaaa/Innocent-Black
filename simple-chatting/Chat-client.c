// client side
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
#define PORT 80
#define okay(msg, ...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*]" msg "\n", ##__VA_ARGS__)

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        error("WSAStartup failed");
        return EXIT_FAILURE;
    }

    info("creating the socket");
    SOCKET socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == INVALID_SOCKET)
    {
        error("Socket creation failed (INVALID_SOCKET), error:%d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Created the client socket");

    char* ip ="142.250.31.27";
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int iResult = connect(socketFD, (struct sockaddr *)&address, sizeof(address));
    if (iResult == SOCKET_ERROR)
    {
        error("Connection failed, error:%d", WSAGetLastError());
        closesocket(socketFD);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("connected to server");

    return EXIT_SUCCESS;
}