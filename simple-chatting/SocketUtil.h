#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>

#include <windows.h>
#ifdef _WIN32
typedef intptr_t ssize_t;
#endif

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

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL)
    {
        return -1;
    }
    if (stream == NULL)
    {
        return -1;
    }
    if (n == NULL)
    {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF)
    {
        return -1;
    }
    if (bufptr == NULL)
    {
        bufptr = malloc(128);
        if (bufptr == NULL)
        {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while (c != EOF)
    {
        if ((p - bufptr) > (size - 1))
        {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL)
            {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n')
        {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}