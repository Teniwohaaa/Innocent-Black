cc = gcc
CFLAGS = -Wall -Wextra
TARGET = webserver

all: $(TARGET)
$(TARGET): webserver.c
    $(CC) $(CFLAGS) -o $@ $^
run:
    ./$(TARGET)
clean:
    rm -f $(TARGET)