CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = main
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Chạy thông thường
run: all
	./$(TARGET)

# Chạy với quyền root (để duyệt qua thư mục thiếu quyền x)
run-sudo: all
	sudo ./$(TARGET)

clean:
	# Gọi hàm clean_directory_robust qua chương trình tạm hoặc dọn bằng sudo nếu kẹt quyền
	sudo rm -rf "Test folder" 2>/dev/null || rm -rf "Test folder"
	rm -f $(TARGET)

.PHONY: all run run-sudo clean
