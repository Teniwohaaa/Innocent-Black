#include "SocketUtil.h"

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 2048
#define PORT 2000
#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*] " msg "\n", ##__VA_ARGS__)

struct AcceptedSocket
{
  SOCKET sockAcceptedFD;
  struct sockaddr_in ClientAddress;
  boolean bSuccess;
  int iErrorCode;
};

struct AcceptedSocket *AcceptIncomingConnection(SOCKET socketServerFD)
{
  struct AcceptedSocket *socketClient = malloc(sizeof(*socketClient));
  if (!socketClient)
  {
    error("Memory allocation failed for AcceptedSocket");
    return NULL;
  }
  // initializing the struct
  socketClient->bSuccess = FALSE;
  socketClient->sockAcceptedFD = INVALID_SOCKET;
  socketClient->iErrorCode = 0;

  int addrlen = sizeof(socketClient->ClientAddress);
  info("waiting for a connection...");

  socketClient->sockAcceptedFD = accept(socketServerFD, (struct sockaddr *)&socketClient->ClientAddress, &addrlen);

  if (socketClient->sockAcceptedFD == INVALID_SOCKET)
  {
    socketClient->iErrorCode = WSAGetLastError();
    error("accept() failed with error: %d", socketClient->iErrorCode);
    free(socketClient);
    return NULL;
  }
  socketClient->bSuccess = TRUE;
  return socketClient;
}

int main()
{
  // Starting the windows socket api
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    error("WSAStartup failed");
    return EXIT_FAILURE;
  }
  // Confirm that the WinSock DLL supports 2.2
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
  {
    error("Could not find a usable version of Winsock.dll");
    WSACleanup();
    return EXIT_FAILURE;
  }

  info("creating the socket");

  SOCKET socketServerFD = CreateSocketTCPIPv4();
  if (socketServerFD == INVALID_SOCKET)
  {
    error("Socket creation failed, error:%d", WSAGetLastError());
    WSACleanup();
    return EXIT_FAILURE;
  }
  okay("Created the server socket");

  // creating the adress to bind for listening
  struct sockaddr_in address_server = CreateIPv4Address("", PORT);
  okay("Created the server address");

  // binding the socket and address
  if (bind(socketServerFD, (struct sockaddr *)&address_server, sizeof(address_server)) == SOCKET_ERROR)
  {
    error("Bind failed with error: %d", WSAGetLastError());
    closesocket(socketServerFD);
    WSACleanup();
    return EXIT_FAILURE;
  }
  okay("Socket bound to port %d", PORT);

  // Lisntening
  if (listen(socketServerFD, 3) == SOCKET_ERROR)
  {
    error("Listen function failed with error: %d", WSAGetLastError());
    closesocket(socketServerFD);
    WSACleanup();
    return EXIT_FAILURE;
  }
  info("server listening on port %d...", PORT);

  struct AcceptedSocket *socketClient = AcceptIncomingConnection(socketServerFD);
  if (socketClient == NULL)
  {
    error("Accept failed");
    closesocket(socketServerFD);
    WSACleanup();
    return EXIT_FAILURE;
  }
  okay("Client connected: %s", inet_ntoa(socketClient->ClientAddress.sin_addr));

  char buffer[BUFFER_SIZE];

  while (TRUE)
  {
    ssize_t AmountR = recv(socketClient->sockAcceptedFD, buffer, BUFFER_SIZE, 0);
    if (AmountR > 0)
    {
      buffer[AmountR] = 0;
      info("Received: %s", buffer);
    }
    else if (AmountR == 0)
    {
      info("Client disconnected");
      break;
    }
    else
    {
      error("recv failed with error: %d", WSAGetLastError());
      break;
    }
  }

  closesocket(socketClient->sockAcceptedFD);
  closesocket(socketServerFD);
  free(socketClient);
  WSACleanup();

  return EXIT_SUCCESS;
}