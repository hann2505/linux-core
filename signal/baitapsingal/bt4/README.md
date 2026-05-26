# Bài tập 4 – File Descriptor và Pipe cơ bản

## 1. Xem file descriptor của một tiến trình

### Terminal 1:
```bash
more /var/log/syslog
```

### Terminal 2:
```bash
ps -a                          # tìm PID của tiến trình more
ls -l /proc/<PID>/fd           # xem các fd đang mở
```

Kết quả thường thấy:
```
lrwxrwxrwx 1 ... 0 -> /dev/pts/0    # stdin  (fd=0) → terminal
lrwxrwxrwx 1 ... 1 -> /dev/pts/0    # stdout (fd=1) → terminal
lrwxrwxrwx 1 ... 2 -> /dev/pts/0    # stderr (fd=2) → terminal
lrwxrwxrwx 1 ... 3 -> /var/log/syslog  # file đang đọc
```

Ba fd mặc định của mọi process:
| fd | Tên | Hướng |
|---|---|---|
| 0 | stdin  | đọc từ terminal / keyboard |
| 1 | stdout | ghi ra terminal |
| 2 | stderr | ghi lỗi ra terminal |

## 2. Pipe trong shell
```bash
ls / | sort              # stdout của ls → stdin của sort
cat /etc/passwd | more   # stdout của cat → stdin của more
```

Pipe `|` kết nối stdout của process trái với stdin của process phải,
kernel dùng anonymous pipe (FIFO trong bộ nhớ) để truyền dữ liệu.
