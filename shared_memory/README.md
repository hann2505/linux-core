# Tài liệu hướng dẫn Lập trình hệ thống Linux - Shared Memory & Signals

Thư mục này chứa lời giải cho 3 bài tập về bộ nhớ chia sẻ (Shared Memory) và Tín hiệu (Signals) trong lập trình hệ thống Linux.

## Cấu trúc thư mục

```text
shared_memory/
├── ex1_mmap_chat/           # Bài 1: Ứng dụng Chat bằng mmap (lưu lịch sử vào file)
│   ├── common.h
│   ├── server.c
│   ├── client.c
│   └── Makefile
├── ex2_shm_chat/            # Bài 2: Ứng dụng Chat bằng POSIX Shared Memory
│   ├── common.h
│   ├── server.c
│   ├── client.c
│   └── Makefile
├── ex3_signal_shm/          # Bài 3: Client-Server đồng bộ bằng Shared Memory và Tín hiệu
│   ├── common.h
│   ├── server.c
│   ├── client.c
│   └── Makefile
└── README.md                # Tài liệu hướng dẫn này
```

---

## Chi tiết các bài tập

### Bài 1: Ứng dụng chat bằng `mmap` (Lưu lịch sử chat vào file)
- **Cơ chế hoạt động**: Sử dụng một file thông thường `chat_history.dat` được ánh xạ vào bộ nhớ thông qua lệnh gọi hệ thống `mmap`. 
- **Đồng bộ hóa**: Sử dụng mutex và biến điều kiện (Condition Variable) loại chia sẻ tiến trình (`PTHREAD_PROCESS_SHARED`) được đặt trực tiếp bên trong cấu trúc vùng nhớ map để đồng bộ việc đọc/ghi giữa Server và Client mà không bị lặp vô tận (busy loop).
- **Lưu lịch sử**: Dữ liệu chat được lưu trực tiếp vào file. Khi khởi chạy lại ứng dụng chat, lịch sử chat cũ sẽ tự động được hiển thị.
- **Thoát**: Nhập `/exit` để thoát ứng dụng an toàn.

#### Hướng dẫn biên dịch và chạy:
```bash
cd ex1_mmap_chat
make

# Terminal 1: Chạy Server trước
./server

# Terminal 2: Chạy Client
./client
```

---

### Bài 2: Ứng dụng chat bằng POSIX Shared Memory
- **Cơ chế hoạt động**: Sử dụng bộ nhớ chia sẻ POSIX thông qua `shm_open` tạo một đối tượng shared memory dưới tên `/ex2_chat_shm` nằm trong RAM (trên Linux thường nằm ở `/dev/shm`).
- **Đồng bộ hóa**: Sử dụng mutex và condition variable process-shared tương tự như Bài 1.
- **Dọn dẹp**: Khi Server thoát, nó sẽ gọi `shm_unlink` để giải phóng đối tượng bộ nhớ chia sẻ khỏi hệ thống.

#### Hướng dẫn biên dịch và chạy:
```bash
cd ex2_shm_chat
make

# Terminal 1: Chạy Server trước
./server

# Terminal 2: Chạy Client
./client
```

---

### Bài 3: Client - Server đồng bộ bằng Shared Memory và Signals
- **Cơ chế hoạt động**:
  - **Server** tạo ra vùng nhớ chia sẻ `/ex3_sig_shm` thông qua `shm_open`, ghi PID của chính nó vào trường `server_pid`, sau đó ngủ sâu bằng lệnh `pause()`.
  - **Client** kết nối vào vùng nhớ chia sẻ, yêu cầu người dùng nhập thông tin Tên (`name`) và Tuổi (`age`).
  - Sau khi ghi thông tin vào struct trong shared memory, Client đọc PID của Server và gửi tín hiệu `SIGUSR1` (`kill(server_pid, SIGUSR1)`) để đánh thức Server dậy.
  - **Server** khi nhận tín hiệu sẽ thức giấc, đọc dữ liệu từ shared memory, ghi thông tin kèm mốc thời gian vào file log `server_log.txt`, sau đó tiếp tục ngủ lại.
- **Yêu cầu phụ**: Hai tiến trình được viết trên 2 file C riêng biệt, không sử dụng `fork()`. Server dọn dẹp bộ nhớ chia sẻ khi nhận tín hiệu Ctrl+C (`SIGINT`).

#### Hướng dẫn biên dịch và chạy:
```bash
cd ex3_signal_shm
make

# Terminal 1: Chạy Server trước
./server

# Terminal 2: Chạy Client
./client

# Nhập dữ liệu tại Client:
# Enter Name: Nguyen Van A
# Enter Age: 20
# Sending SIGUSR1 to Server...

# Xem file log ghi lại ở Server:
cat server_log.txt
```

---

## Các hàm hệ thống Linux được sử dụng chính
1. **`mmap` / `munmap`**: Ánh xạ file hoặc đối tượng shared memory vào không gian địa chỉ ảo của tiến trình.
2. **`shm_open` / `shm_unlink`**: Tạo/mở và xóa đối tượng bộ nhớ chia sẻ POSIX.
3. **`ftruncate`**: Thiết lập kích thước cho file hoặc phân đoạn shared memory.
4. **`pthread_mutex_init` / `pthread_cond_init`**: Thiết lập mutex và condition variable với thuộc tính `PTHREAD_PROCESS_SHARED` để đồng bộ giữa hai tiến trình khác nhau.
5. **`sigaction` / `pause`**: Đăng ký hàm xử lý tín hiệu và đưa tiến trình vào trạng thái tạm dừng chờ tín hiệu (sleep).
6. **`kill`**: Gửi tín hiệu đánh thức tiến trình khác qua PID của nó.
