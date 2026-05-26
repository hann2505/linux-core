CC = gcc
CFLAGS = -Wall

all: block_sigint show_sigmask pending_check

block_sigint: block_sigint.c
	$(CC) $(CFLAGS) -o block_sigint block_sigint.c

show_sigmask: show_sigmask.c
	$(CC) $(CFLAGS) -o show_sigmask show_sigmask.c

pending_check: pending_check.c
	$(CC) $(CFLAGS) -o pending_check pending_check.c

clean:
	rm -f block_sigint show_sigmask pending_check
