#include "SocketUtil.h"

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 2048
#define PORT 2000
#define MAX_KITTENS 20
#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*] " msg "\n", ##__VA_ARGS__)

SOCKET connectedClients[MAX_KITTENS];
int clientCount = 0;
CRITICAL_SECTION csClientList;

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
void AddClient(SOCKET socketClient)
{
  EnterCriticalSection(&csClientList);
  if (clientCount < MAX_KITTENS)
  {
    connectedClients[clientCount++] = socketClient;
    info("Kittent connected! you have now %d kittens >-<", clientCount);
  }
  else
  {
    error("Maximum Kittens reached. Cannot add new cats !! >-<.");
  }

  LeaveCriticalSection(&csClientList);
}

void RemoveClient(SOCKET socketClient)
{
  EnterCriticalSection(&csClientList);
  for (int i = 0; i < clientCount; i++)
  {
    if (connectedClients[i] == socketClient)
    {
      for (int j = i; j < clientCount - 1; j++)
      {
        connectedClients[j] = connectedClients[j + 1];
      }
      clientCount--;
      info("kitten gone ;^; there is only %d left", clientCount);
      break;
    }
  }
  LeaveCriticalSection(&csClientList);
}

void BroadcastMessage(SOCKET senderSocket, const char *message)
{
  EnterCriticalSection(&csClientList);

  for (int i = 0; i < clientCount; i++)
  {
    if (connectedClients[i] != senderSocket && connectedClients[i] != INVALID_SOCKET)
    {
      send(connectedClients[i], message, strlen(message), 0);
    }
  }

  LeaveCriticalSection(&csClientList);
}

DWORD WINAPI ClientHandler(LPVOID lpkitten)
{
  SOCKET socketClientFD = (SOCKET)lpkitten;
  char buffer[BUFFER_SIZE];

  AddClient(socketClientFD);
  char *hello = "Say hi to daddy kitten !\n";
  send(socketClientFD, hello, strlen(hello), 0);

  while (TRUE)
  {
    ssize_t AmountR = recv(socketClientFD, buffer, BUFFER_SIZE, 0);
    if (AmountR > 0)
    {
      buffer[AmountR] = 0;
      info("Received: %s", buffer);
      BroadcastMessage(socketClientFD, buffer);
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
  RemoveClient(socketClientFD);
  closesocket(socketClientFD);
  return EXIT_SUCCESS;
}
void StartAcceptingIncomingConnection(SOCKET socketServerFD)
{
  while (TRUE)
  {
    struct AcceptedSocket *socketClient = AcceptIncomingConnection(socketServerFD);
    if (socketClient == NULL)
    {
      error("Accept failed");
      info("listening....");
      continue;
    }
    okay("Client connected: %s", inet_ntoa(socketClient->ClientAddress.sin_addr));

    // creating the thread for the client !!
    DWORD ThreadID = NULL;
    HANDLE hThread = CreateThread(NULL, 0, ClientHandler, (LPVOID)socketClient->sockAcceptedFD, 0, &ThreadID);
    if (hThread == NULL)
    {
      error("Failed to create thread for client, error: %d", GetLastError());
      closesocket(socketClient->sockAcceptedFD);
      RemoveClient(socketClient->sockAcceptedFD);
    }
    else
    {
      info("Created thread");
      CloseHandle(hThread);
    }
    free(socketClient);
  }
};

int main()
{
  // Starting the windows socket api
  InitializeCriticalSection(&csClientList);
  for (int i = 0; i < MAX_KITTENS; i++)
  {
    connectedClients[i] = INVALID_SOCKET;
  }

  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    error("WSAStartup failed");
    DeleteCriticalSection(&csClientList);
    return EXIT_FAILURE;
  }
  // Confirm that the WinSock DLL supports 2.2
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
  {
    error("Could not find a usable version of Winsock.dll");
    WSACleanup();
    DeleteCriticalSection(&csClientList);
    return EXIT_FAILURE;
  }

  info("creating the socket");

  SOCKET socketServerFD = CreateSocketTCPIPv4();
  if (socketServerFD == INVALID_SOCKET)
  {
    error("Socket creation failed, error:%d", WSAGetLastError());
    WSACleanup();
    DeleteCriticalSection(&csClientList);
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
    DeleteCriticalSection(&csClientList);
    return EXIT_FAILURE;
  }
  okay("Socket bound to port %d", PORT);

  // Lisntening
  if (listen(socketServerFD, 3) == SOCKET_ERROR)
  {
    error("Listen function failed with error: %d", WSAGetLastError());
    closesocket(socketServerFD);
    WSACleanup();
    DeleteCriticalSection(&csClientList);
    return EXIT_FAILURE;
  }
  info("server listening on port %d...", PORT);

  StartAcceptingIncomingConnection(socketServerFD);

  // Cleanup
  closesocket(socketServerFD);
  DeleteCriticalSection(&csClientList);
  WSACleanup();

  return EXIT_SUCCESS;
}
