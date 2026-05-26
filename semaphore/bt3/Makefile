CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lpthread

all: sem_write matmul

sem_write: sem_write.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

matmul: matmul.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f sem_write matmul out_even.txt out_odd.txt
