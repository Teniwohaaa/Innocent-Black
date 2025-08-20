#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib") 

#define PORT 8080
#define BUFFER_SIZE 2048
#define okay(msg,...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg,...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg,...) printf("[*]" msg "\n", ##__VA_ARGS__)

int main(){
    // variables
    WSADATA wsaDATA; 
    SOCKET listen_socket = INVALID_SOCKET;
    // initiates use of the Winsock
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
    
    // creating a socket
    // create a TCP socket
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        error("Socket creation failed, error: %d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Listening socket created");
    // ...additional server setup and accept loop would go here...
    closesocket(listen_socket);
    WSACleanup();
    return EXIT_SUCCESS;
}