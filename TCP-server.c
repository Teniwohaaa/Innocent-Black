// server TCP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib") 


#define okay(msg,...) printf("[+]" msg "\n", ##__VA_ARGS__)
#define error(msg,...) printf("[-]" msg "\n", ##__VA_ARGS__)
#define info(msg,...) printf("[*]" msg "\n", ##__VA_ARGS__)

int main(){
    // initializing WinSock
    WSADATA wsaDATA;

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
    

    // cleanup
     WSACleanup();

   return EXIT_SUCCESS; 
}