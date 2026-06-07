# Bài tập Lập trình File System Linux

Dự án này chứa ba bài tập C về lập trình File System trong Linux sử dụng các API hệ thống chuẩn POSIX như `opendir`, `readdir`, `stat`, `mkdir` và `chmod`.

## Cấu trúc thư mục dự án

```
filesystem/
├── Makefile          - Makefile gốc để điều phối biên dịch cả ba bài tập
├── README.md         - Hướng dẫn tổng quan
├── bt1/              - Bài tập 1: Duyệt đệ quy và lọc file chỉ đọc trên cây thư mục tự chọn
│   ├── main.c
│   ├── Makefile
│   └── README.md
├── bt2/              - Bài tập 2: Tạo cấu trúc cây thư mục "Test folder" khóa quyền và lọc file
│   ├── main.c
│   ├── Makefile
│   └── README.md
└── bt3/              - Bài tập 3: Đóng gói ứng dụng và viết bộ cài đặt/gỡ cài đặt bằng C
    ├── main_app.c
    ├── libhello.h / libhello.c
    ├── setup.c / uninstall.c
    ├── myapp.conf
    ├── Makefile
    └── README.md
```

## Các bài tập

### [Bài tập 1 (bt1)](file:///d:/code/dsa/signal/filesystem/bt1/README.md)
* **Mô tả**: Tạo một cấu trúc thư mục mẫu đa dạng gồm nhiều cấp thư mục và file khác nhau với các quyền truy cập đa dạng. Chương trình sẽ quét đệ quy và lọc ra tất cả các tệp tin chỉ đọc (Read-only) của tiến trình.
* **Cách chạy**:
  ```bash
  make run-bt1
  ```

### [Bài tập 2 (bt2)](file:///d:/code/dsa/signal/filesystem/bt2/README.md)
* **Mô tả**: Tạo lập đúng cấu trúc thư mục theo sơ đồ (trong đó thư mục cha và thư mục con `Test1` có quyền `rw-rw-r--` - thiếu quyền thực thi `x`). Chương trình minh họa lỗi phân quyền khi chạy bằng User thường và sự thành công (bypass) của tài khoản Root khi quét sâu tìm file `abc` (`r--r--r--`).
* **Cách chạy thông thường**:
  ```bash
  make run-bt2
  ```
* **Cách chạy bằng Root/Sudo**:
  ```bash
  make run-bt2-sudo
  ```

### [Bài tập 3 (bt3)](file:///d:/code/dsa/signal/filesystem/bt3/README.md)
* **Mô tả**: Đóng gói ứng dụng `myapp` cùng thư viện liên kết động `libhello.so` và tệp cấu hình `myapp.conf` thành bộ phân phối. Viết chương trình cài đặt `setup` và gỡ cài đặt `uninstall` bằng ngôn ngữ C sử dụng các hàm API hệ thống tương tác tệp tin.
* **Cách cài đặt (cần sudo)**:
  ```bash
  make run-bt3-install
  ```
* **Cách gỡ cài đặt (cần sudo)**:
  ```bash
  make run-bt3-remove
  ```

---

## Biên dịch và Dọn dẹp tổng thể

* **Biên dịch cả 3 bài tập**:
  ```bash
  make
  ```
* **Dọn dẹp môi trường ở cả 3 thư mục con**:
  ```bash
  make clean
  ```

