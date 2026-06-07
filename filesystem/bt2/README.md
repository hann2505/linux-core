# Bài tập 2 - Linux File System & Cơ chế bảo mật quyền thư mục

Bài tập này giải quyết bài toán tạo lập cấu trúc thư mục đúng chuẩn theo sơ đồ và tìm kiếm các tệp tin chỉ đọc (**Read-only**), đồng thời minh họa trực quan cơ chế hoạt động của quyền thực thi (`x` - execute) đối với thư mục trong hệ thống Linux.

## 1. Cấu trúc thư mục yêu cầu

Chương trình tạo ra cấu trúc thư mục sau:
```
Test folder  (Quyền: rw-rw-r-- / 0664)
├── Test1    (Quyền: rw-rw-r-- / 0664)
│   └── abc  (Quyền: r--r--r-- / 0444) -> **[Tệp tin Read-Only]**
└── efg      (Quyền: rw-rw-r-- / 0664)
```

## 2. Lý thuyết File System: Quyền thực thi (`x`) trên thư mục

Trong Linux/Unix, ý nghĩa phân quyền cho **Thư mục** khác với **Tệp tin thông thường**:
* **Quyền Đọc (`r`)**: Cho phép đọc danh sách tệp tin nằm trong thư mục (sử dụng hàm `readdir` hoặc lệnh `ls`).
* **Quyền Ghi (`w`)**: Cho phép thêm, xóa hoặc đổi tên tệp tin trong thư mục đó.
* **Quyền Thực thi (`x`)**: Cho phép "truy cập" hay "đi xuyên qua" thư mục đó để vào các thư mục con hoặc truy cập siêu dữ liệu (metadata/inode) của tệp tin bên trong (sử dụng hàm `stat` hoặc `opendir`).

### Vấn đề gặp phải trong Bài tập 2
Theo yêu cầu, thư mục `Test folder` và `Test1` có quyền là `rw-rw-r--` (hoàn toàn thiếu quyền thực thi `x`). Do đó:
1. **Nếu chạy bằng User thường**: 
   - Tiến trình không có quyền `x` để truy cập vào `Test folder` hay đi sâu vào `Test1`.
   - Hàm `opendir("Test folder")` sẽ báo lỗi `EACCES (Permission denied)`.
   - Kết quả: Không thể tìm thấy file `abc` ở sâu bên trong.
2. **Nếu chạy bằng quyền Root (Superuser/Sudo)**:
   - Root có đặc quyền bypass tất cả các kiểm tra phân quyền truy cập thông thường.
   - Hàm `opendir` và `stat` sẽ thực thi thành công, chương trình sẽ quét được vào trong thư mục `Test1` và hiển thị tệp tin chỉ đọc `abc` (`r--r--r--`).

## 3. Cách chạy chương trình

### Bước 1: Biên dịch chương trình
```bash
make
```

### Bước 2: Chạy kiểm thử dưới quyền User thường (Sẽ gặp lỗi bảo mật)
```bash
make run
```
**Kết quả mong đợi:** Chương trình sẽ thông báo không thể mở thư mục `Test folder` do thiếu quyền truy cập (Permission denied).

### Bước 3: Chạy dưới quyền Superuser / Sudo (Duyệt thành công)
```bash
make run-sudo
```
**Kết quả mong đợi:** Chương trình quét thành công và hiển thị tệp tin chỉ đọc duy nhất là `Test folder/Test1/abc` với quyền `0444`.

### Bước 4: Dọn dẹp
```bash
make clean
```
