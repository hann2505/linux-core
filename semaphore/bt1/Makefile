CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread

all: count_odd gen_files

count_odd: count_odd.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

gen_files: gen_files.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f block_check_sigint count_odd gen_files output_*.txt
