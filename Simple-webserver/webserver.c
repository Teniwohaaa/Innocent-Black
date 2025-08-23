#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 2048
#define okay(msg, ...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*]" msg "\n", ##__VA_ARGS__)

void send_HTML(SOCKET socketClient, const char *Path)
{
    FILE *file = fopen(Path, "r");
    if (!file)
    {
        perror("Failed to open HTML file");
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    // send HTTP header
    char *headers = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n\r\n";
    send(socketClient, headers, strlen(headers), 0);
    // Send HTML content
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        send(socketClient, buffer, bytes_read, 0);
    }
    fclose(file);
}

int main()
{
    // variables
    WSADATA wsaDATA;
    SOCKET socketFD = INVALID_SOCKET, socketClient = INVALID_SOCKET;
    struct sockaddr_in service, client_addr; // The socket address to be passed to bind
    int iResult = SOCKET_ERROR;
    socklen_t client_len = sizeof(client_addr);

    // initiates use of the Winsock
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

    // creating a socket

    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketFD == INVALID_SOCKET)
    {
        error("Socket creation failed, error: %ld", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Listening socket created");
    // binding the socket
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(PORT);

    iResult = bind(socketFD, (SOCKADDR *)&service, sizeof(service));
    if (iResult == SOCKET_ERROR)
    {
        error("Bind failed with error: %ld", WSAGetLastError());

        closesocket(socketFD);
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("bind returned sucess");

    // listen for connections

    if (listen(socketFD, 5) == SOCKET_ERROR)
    {
        error("Listen function failed with error: %ld", WSAGetLastError());
        closesocket(socketFD);
        WSACleanup();
        return EXIT_FAILURE;
    }
    info("listening to port %d...", PORT);

    // accepting connections

    socketClient = accept(socketFD, (struct sockaddr *)&client_addr, &client_len);
    if (socketClient == INVALID_SOCKET)
    {
        error("Accept failed with error: %ld", WSAGetLastError());
        closesocket(socketFD);
        WSACleanup();
        return EXIT_FAILURE;
    }
    info("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

    // send http response

    closesocket(socketFD);
    WSACleanup();
    info("Client disconnected\n");
    return EXIT_SUCCESS;
}