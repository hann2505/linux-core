# Bài tập 3 - Đóng gói, Cài đặt và Gỡ cài đặt Ứng dụng Linux

Bài tập này sử dụng các kiến thức lập trình hệ thống tệp tin (File System) trong Linux để viết chương trình cài đặt (`setup`) và gỡ cài đặt (`uninstall`) cho một ứng dụng viết bằng C.

## 1. Cấu trúc bộ cài đặt sinh ra (`package/`)

Sau khi chạy lệnh `make`, chương trình sẽ tự động đóng gói các thành phần sau vào thư mục `package/`:
- `package/setup` (Chương trình C thực hiện cài đặt hệ thống)
- `package/uninstall` (Chương trình C thực hiện gỡ cài đặt hệ thống)
- `package/bin/myapp` (File binary thực thi của ứng dụng chính)
- `package/lib/libhello.so` (Thư viện liên kết động mà ứng dụng sử dụng)
- `package/config/myapp.conf` (Tệp cấu hình của ứng dụng)

## 2. Vị trí cài đặt trên hệ thống đích

Chương trình `setup` sẽ sao chép các tệp tin vào đúng vị trí tiêu chuẩn trên Linux:
1. **File thực thi**: `/usr/local/bin/myapp` (Quyền truy cập: `0755` - Đọc/Ghi/Chạy cho owner, Đọc/Chạy cho group/other)
2. **Thư viện liên kết động**: `/usr/local/lib/libhello.so` (Quyền truy cập: `0755` - Đọc/Ghi/Chạy cho owner, Đọc/Chạy cho group/other)
3. **File cấu hình**: `/etc/myapp/myapp.conf` (Quyền truy cập: `0644` - Đọc/Ghi cho owner, chỉ đọc cho group/other)
   - Lưu ý: Thư mục `/etc/myapp` sẽ được tạo mới tự động nếu chưa tồn tại với quyền `0755`.

## 3. Hoạt động của các chương trình hệ thống C

### Chương trình Cài đặt (`setup.c`):
- **Kiểm tra quyền Root**: Kiểm tra xem tiến trình chạy dưới quyền Root hay không (`getuid() == 0`). Nếu không phải Root, chương trình sẽ từ chối chạy.
- **Tạo thư mục**: Sử dụng hàm `mkdir(DST_CFG_DIR, 0755)` để tạo thư mục cấu hình `/etc/myapp`.
- **Sao chép file**: Sử dụng hàm `open` kết hợp với `read`/`write` tuần tự để copy dữ liệu tệp tin một cách thủ công (chứng minh kiến thức file system cơ bản).
- **Thiết lập quyền**: Sử dụng hàm `chmod` để gán quyền truy cập rõ ràng.
- **Cập nhật Dynamic Linker**: Chạy lệnh hệ thống `system("ldconfig")` để hệ điều hành nhận diện thư viện mới `/usr/local/lib/libhello.so`.

### Chương trình Gỡ cài đặt (`uninstall.c`):
- **Kiểm tra quyền Root**.
- **Xóa file**: Sử dụng hàm hệ thống `unlink` để gỡ bỏ lần lượt tệp binary, thư viện `.so` và file cấu hình `.conf`.
- **Xóa thư mục**: Sử dụng hàm `rmdir` để xóa thư mục cấu hình `/etc/myapp` sau khi đã dọn sạch tệp tin bên trong.
- **Cập nhật Dynamic Linker**: Gọi `ldconfig` để xóa cache thư viện cũ.

---

## 4. Cách sử dụng

### Bước 1: Biên dịch và Đóng gói
Di chuyển vào thư mục bài tập và chạy lệnh build:
```bash
cd d:/code/dsa/signal/filesystem/bt3
make
```
Lệnh này sẽ biên dịch mã nguồn C và tạo ra thư mục `package/` chứa bộ cài đặt.

### Bước 2: Cài đặt ứng dụng (Cần quyền root)
Chạy lệnh cài đặt:
```bash
sudo ./package/setup
```
Hoặc dùng lối tắt:
```bash
make install
```

### Bước 3: Chạy ứng dụng đã cài đặt
Khi ứng dụng đã được cài đặt thành công vào `/usr/local/bin`, bạn có thể chạy nó từ bất kỳ thư mục nào bằng cách gõ:
```bash
myapp
```
Ứng dụng sẽ tự động tải thư viện liên kết động `/usr/local/lib/libhello.so` và đọc tệp cấu hình tại `/etc/myapp/myapp.conf` để in ra lời chào mừng.

### Bước 4: Gỡ cài đặt ứng dụng (Cần quyền root)
Chạy lệnh gỡ cài đặt:
```bash
sudo ./package/uninstall
```
Hoặc dùng lối tắt:
```bash
make remove
```
Sau bước này, toàn bộ các file được cài đặt và thư mục `/etc/myapp` sẽ biến mất hoàn toàn khỏi hệ thống của bạn.
