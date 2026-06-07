# Bài tập Lập trình Hệ thống Linux - Thư mục & Quyền truy cập File

Bài tập này sử dụng kiến thức lập trình File System trong nhân Linux/POSIX để tạo ra một cấu trúc thư mục với các tệp tin có phân quyền khác nhau, sau đó quét đệ quy cấu trúc này để tìm và in ra danh sách các tệp tin ở trạng thái **Chỉ đọc (Read-only)**.

## 1. Cấu trúc thư mục tạo ra (`parent_dir/`)

Chương trình tạo ra một thư mục cha `parent_dir` với các thư mục con và tệp tin sau:
- `parent_dir/file_read_only.txt` (Quyền: `0444` - Chỉ đọc cho tất cả) -> **[READ-ONLY]**
- `parent_dir/file_read_write.txt` (Quyền: `0644` - Đọc/Ghi cho owner, đọc cho group/other)
- `parent_dir/sub_dir1/` (Quyền: `0755` - Thư mục con 1)
  - `parent_dir/sub_dir1/file_owner_read.txt` (Quyền: `0400` - Chỉ đọc cho owner) -> **[READ-ONLY]**
  - `parent_dir/sub_dir1/file_executable.sh` (Quyền: `0755` - Có thể thực thi)
- `parent_dir/sub_dir2/` (Quyền: `0755` - Thư mục con 2)
  - `parent_dir/sub_dir2/file_read_only_2.txt` (Quyền: `0444` - Chỉ đọc cho tất cả) -> **[READ-ONLY]**
  - `parent_dir/sub_dir2/file_write_only.txt` (Quyền: `0200` - Chỉ ghi cho owner)

## 2. Các hàm File System Linux/POSIX được sử dụng

- **`mkdir(const char *path, mode_t mode)`**: Tạo thư mục mới.
- **`chmod(const char *path, mode_t mode)`**: Thay đổi quyền (mode) của thư mục hoặc tệp tin để đảm bảo quyền chính xác, tránh việc bị ảnh hưởng bởi giá trị `umask` mặc định của hệ thống.
- **`open(const char *path, int oflag, mode_t mode)`**: Tạo/Mở tệp tin.
- **`opendir(const char *name)` / `readdir(DIR *dirp)` / `closedir(DIR *dirp)`**: Dùng để duyệt đệ quy qua các tệp tin và thư mục con trong hệ thống.
- **`stat(const char *path, struct stat *buf)`**: Lấy thuộc tính tệp tin (loại tệp tin, quyền truy cập).
  - Sử dụng macro `S_ISREG(mode)` để nhận diện file thường.
  - Sử dụng macro `S_ISDIR(mode)` để nhận diện thư mục con.
- **`unlink(const char *path)` / `rmdir(const char *path)`**: Dùng để xóa các tệp tin và thư mục để dọn dẹp sạch sẽ cấu trúc cũ trước khi tạo mới.

## 3. Cách nhận diện một file là "Read-only"

Một tệp tin được phân loại là **Read-only** nếu nó thỏa mãn điều kiện:
1. Chủ sở hữu (owner) có quyền đọc: `(statbuf.st_mode & S_IRUSR) != 0`
2. Chủ sở hữu **không** có quyền ghi: `(statbuf.st_mode & S_IWUSR) == 0`

Quy tắc này phản ánh chính xác trạng thái chỉ đọc đối với owner của tệp tin. Chương trình in ra các tệp tin thỏa mãn điều kiện này kèm theo chuỗi quyền tượng trưng (dạng `rwxrwxrwx`) và mã bát phân (octal).

## 4. Cách biên dịch và chạy chương trình

Chương trình yêu cầu môi trường hệ điều hành Linux (hoặc môi trường giả lập POSIX như WSL trên Windows).

### Bước 1: Biên dịch chương trình
Chạy lệnh sau tại thư mục chứa bài tập:
```bash
make
```

### Bước 2: Chạy chương trình
Chạy file thực thi để tạo thư mục và hiển thị kết quả lọc file:
```bash
make run
```
hoặc:
```bash
./main
```

### Bước 3: Dọn dẹp môi trường
Để xóa sạch các thư mục và file thực thi được tạo ra:
```bash
make clean
```
