#include <winsock2.h>
#include <ws2tcpip.h>

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SOCKET CreateSocketTCPIPv4(void)
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in CreateIPv4Address(const char *IP, int PORT)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (IP == NULL || strlen(IP) == 0)

        address.sin_addr.s_addr = INADDR_ANY;

    else
        address.sin_addr.s_addr = inet_addr(IP);
    return address;
}