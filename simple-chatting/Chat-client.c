#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 2048
#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*] " msg "\n", ##__VA_ARGS__)

SOCKET CreateSocketTCPIPv4(void) {
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in CreateIPv4Address(const char *IP, int PORT) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(IP);
    return address;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error("WSAStartup failed");
        return EXIT_FAILURE;
    }

    info("creating the socket");
    SOCKET socketFD = CreateSocketTCPIPv4();
    if (socketFD == INVALID_SOCKET) {
        error("Socket creation failed, error:%d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Created the client socket");

    char *IP = "1.1.1.1";
    int PORT = 80;
    struct sockaddr_in address = CreateIPv4Address(IP, PORT);

    info("trying to connect...");
    int iResult = connect(socketFD, (struct sockaddr *)&address, sizeof(address));
    if (iResult == SOCKET_ERROR) {
        error("Connection failed, ip: %s port: %d error: %d", IP, PORT, WSAGetLastError());
        closesocket(socketFD);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("connected to server with ip: %s and port: %d", IP, PORT);

    char request[] = "GET / HTTP/1.1\r\n"
                     "Host: one.one.one.one\r\n"
                     "Connection: close\r\n"
                     "\r\n";

    send(socketFD, request, strlen(request), 0);

    char response[BUFFER_SIZE];
    int bytesReceived = recv(socketFD, response, BUFFER_SIZE - 1, 0);
    if (bytesReceived > 0) {
        response[bytesReceived] = '\0';
        info("received data was: %s", response);
    }

    closesocket(socketFD);
    WSACleanup();

    return EXIT_SUCCESS;
}