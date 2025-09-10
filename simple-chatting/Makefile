CC=	gcc
CFLAGS=	-Wall	-Wextra
LIBS=	-lws2_32

all: server client

server: Chat-Server.c
	$(CC) $(CFLAGS) Chat-Server.c -o daddy.exe $(LIBS)

client: Chat-client.c
	$(CC) $(CFLAGS) Chat-client.c -o kitten.exe $(LIBS)

run: all
	start cmd /k daddy.exe
	start cmd /k kitten.exe

clean:
	del /Q daddy.exe kitten.exe