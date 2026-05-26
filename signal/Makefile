CC = gcc
CFLAGS = -Wall

all: ctrlc_hello sleeper wakeup

ctrlc_hello: ctrlc_hello.c
	$(CC) $(CFLAGS) -o ctrlc_hello ctrlc_hello.c

sleeper: sleeper.c
	$(CC) $(CFLAGS) -o sleeper sleeper.c

wakeup: wakeup.c
	$(CC) $(CFLAGS) -o wakeup wakeup.c

clean:
	rm -f ctrlc_hello sleeper wakeup
