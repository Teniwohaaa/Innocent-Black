CC=	gcc
CFLAGS=	-Wall	-Wextra
LIBS=	-lws2_32

all: server client

server: TCP-Multi.c
	$(CC) $(CFLAGS) TCP-Multi.c -o daddy.exe $(LIBS)

client: TCP-client.c
	$(CC) $(CFLAGS) TCP-client.c -o kitten.exe $(LIBS)

run: all
	start cmd /k daddy.exe
	start cmd /k kitten.exe

clean:
	del /Q daddy.exe kitten.exe
