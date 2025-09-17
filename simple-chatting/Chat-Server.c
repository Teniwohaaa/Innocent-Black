#include "SocketUtil.h"

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 2048
#define PORT 2000
#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define error(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[*] " msg "\n", ##__VA_ARGS__)

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
  okay("Created the client socket");

  // creating the adress to bind for listening
  struct sockaddr_in address_server = CreateIPv4Address("", PORT);
  okay("Created the @");

  // binding the socket and @
  if (bind(socketServerFD, (struct sockaddr *)&address_server, sizeof(address_server)) == SOCKET_ERROR)
  {
    error("Bind failed with error: %d", WSAGetLastError());
    closesocket(socketServerFD);
    WSACleanup();
    return EXIT_FAILURE;
  }
  okay("Socket bound to port %d", PORT);

  // Lisntening
  if (listen(socketServerFD, 3))
  {
    error("Listen function failed with error: %d", WSAGetLastError());
    closesocket(socketServerFD);
    WSACleanup();
    return EXIT_FAILURE;
  }
  info(" server listening on port %d...", PORT);

  struct sockaddr_in ClientAddress;
  int addrlen = sizeof(ClientAddress);
  info("Creating the socket for the client...");
  SOCKET socketClientFD = accept(socketServerFD, (struct sockaddr *)&ClientAddress, &addrlen);
  if (socketClientFD == INVALID_SOCKET)
  {
    error("Accept failed with error: %d", WSAGetLastError());
    closesocket(socketServerFD);
    WSACleanup();
    return EXIT_FAILURE;
  }
  okay("Client connected: %s\n", inet_ntoa(ClientAddress.sin_addr));

  char buffer[BUFFER_SIZE];
  recv(socketClientFD, buffer, BUFFER_SIZE, 0);
  info("Response was: %s", buffer);

  closesocket(socketServerFD);
  WSACleanup();

  return EXIT_SUCCESS;
}