// client side
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
#define IP "1.1.1.1"
#define PORT 80
#define BUFFER_SIZE 2048
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
    info("trying to connect");
    struct sockaddr_in address;
    ;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(IP);

    int iResult = connect(socketFD, (struct sockaddr *)&address, sizeof(address));
    if (iResult == SOCKET_ERROR)
    {
        error("Connection failed, with ip: %s and PORT: %d error: %d", IP, PORT, WSAGetLastError());
        closesocket(socketFD);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("connected to server with ip: %s and PORT: %d", IP, PORT);

    info("sending and receiving");
    char *buffer = "GET / HTTP/1.1\r\n"
                   "Host: one.one.one.one\r\n"
                   "Connection: close\r\n"
                   "\r\n";
    send(socketFD, buffer, strlen(buffer), 0);
    recv(socketFD, buffer, strlen(buffer), 0);

    info("received data was: %s", buffer);
    // cleanup
    closesocket(socketFD);
    WSACleanup();

    return EXIT_SUCCESS;
}