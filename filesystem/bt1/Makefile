CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = main
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf parent_dir

.PHONY: all run clean
