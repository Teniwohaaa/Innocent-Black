CC = gcc
CFLAGS = -Wall -Wextra
TARGET = webserver

all: $(TARGET)

$(TARGET): webserver.c
	$(CC) $(CFLAGS) -o $@ $^ -lws2_32

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean