CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lrt

TARGETS = server client

all: $(TARGETS)

server: server.c common.h
	$(CC) $(CFLAGS) server.c -o server $(LDFLAGS)

client: client.c common.h
	$(CC) $(CFLAGS) client.c -o client $(LDFLAGS)

clean:
	rm -f $(TARGETS) server_log.txt

.PHONY: all clean
