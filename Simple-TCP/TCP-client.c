// client TCP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib") 

#define PORT 9090

int main(){
    // initializing WinSock and variables 
    WSADATA wsaDATA;
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char *msg = "Meow, Server ₍^. .^₎⟆ !";

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

    // step 1: Create a socket
    sock = socket(AF_INET,SOCK_STREAM,0);
     if (sock == INVALID_SOCKET)
    {
        error("socket creation failed (INVALID_SOCKET), error:%d",WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Created the Socket");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton();
    // step 2: Connect to the server

    return EXIT_FAILURE;
}