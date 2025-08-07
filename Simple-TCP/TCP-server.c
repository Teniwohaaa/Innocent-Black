// server TCP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib") 

#define PORT 9090
#define BUFFER_SIZE 1024

#define okay(msg,...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg,...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg,...) printf("[*]" msg "\n", ##__VA_ARGS__)

int main(){
    // initializing WinSock and variables 
    WSADATA wsaDATA;
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};
    int opt = 1;
    socklen_t addrlen = sizeof(address);

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
    
    //step 1: create a socket 
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd == INVALID_SOCKET)
    {
        error("socket creation failed, error:%d",WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }
    okay("Created the Socket");
    // step 2: setting socket options
    setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    // step 3: bind
    // step 4: listen
    // step 5: accept
    // step 6: reply
    // step 7: close
    close(new_socket);
    close(server_fd);
    // cleanup
     WSACleanup();

   return EXIT_SUCCESS; 
}