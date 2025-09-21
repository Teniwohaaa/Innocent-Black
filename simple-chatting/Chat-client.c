// mingw32-make

#include "SocketUtil.h"

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 2048
#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*] " msg "\n", ##__VA_ARGS__)

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        error("WSAStartup failed");
        return EXIT_FAILURE;
    }

    info("creating the socket");
    SOCKET socketFD = CreateSocketTCPIPv4();
    if (socketFD == INVALID_SOCKET)
    {
        error("Socket creation failed, error:%d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Created the client socket");

    char *IP = "127.0.0.1";
    int PORT = 2000;
    struct sockaddr_in address = CreateIPv4Address(IP, PORT);

    info("trying to connect...");
    int iResult = connect(socketFD, (struct sockaddr *)&address, sizeof(address));
    if (iResult == SOCKET_ERROR)
    {
        error("Connection failed, ip: %s port: %d error: %d", IP, PORT, WSAGetLastError());
        closesocket(socketFD);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("connected to server with ip: %s and port: %d", IP, PORT);

    char *LineBuffer = NULL;
    int iLineSize = 0;
    info("Type text (q to QUIT): ");

    while (TRUE)
    {
        size_t CharCount = getline(&LineBuffer, &iLineSize, stdin);
        if (CharCount > 0)
        {
            if (strcmp(LineBuffer, "q") == 0)
                break;

            size_t iAmountSent = send(socketFD, LineBuffer, strlen(LineBuffer), 0);
        }
    }

    closesocket(socketFD);
    WSACleanup();

    return EXIT_SUCCESS;
}