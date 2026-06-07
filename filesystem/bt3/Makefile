CC = gcc
CFLAGS = -Wall -Wextra -g

# Targets
all: myapp setup uninstall package

# Biên dịch thư viện liên kết động libhello.so
libhello.so: libhello.c libhello.h
	$(CC) $(CFLAGS) -fPIC -shared -o libhello.so libhello.c

# Biên dịch ứng dụng chính (có rpath để chạy cục bộ trước khi cài đặt)
myapp: main_app.c libhello.so
	$(CC) $(CFLAGS) -o myapp main_app.c -L. -lhello -Wl,-rpath,./

# Biên dịch chương trình cài đặt
setup: setup.c
	$(CC) $(CFLAGS) -o setup setup.c

# Biên dịch chương trình gỡ cài đặt
uninstall: uninstall.c
	$(CC) $(CFLAGS) -o uninstall uninstall.c

# Đóng gói bộ cài đặt (package)
package: myapp setup uninstall myapp.conf
	mkdir -p package/bin package/lib package/config
	cp myapp package/bin/
	cp libhello.so package/lib/
	cp myapp.conf package/config/
	cp setup package/
	cp uninstall package/

# Cài đặt bằng sudo
install: package
	sudo ./package/setup

# Gỡ cài đặt bằng sudo
remove:
	sudo ./package/uninstall

# Dọn dẹp các tệp build
clean:
	rm -f myapp libhello.so setup uninstall
	rm -rf package

.PHONY: all package install remove clean
