CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread

all: mutex_counter mutex_file

mutex_counter: mutex_counter.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

mutex_file: mutex_file.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f mutex_counter mutex_file shared_output.txt
