# Bài tập 1 – Tìm hiểu lệnh kill và các Signal

## 1. Xem danh sách signal
```bash
kill -l
kill -l SIGTERM      # in số của SIGTERM (= 15)
```

## 2. Gửi SIGTERM để kết thúc tiến trình
```bash
nano                              # mở nano, nó chạy ở foreground
# terminal khác:
ps -a                             # tìm PID của nano
kill -SIGTERM <PID>               # cách tương đương (*)
kill -15 <PID>                    # dùng số thay symbolic name
kill <PID>                        # mặc định gửi SIGTERM
```

## 3. Gửi SIGKILL
```bash
kill -SIGKILL <PID>
kill -9 <PID>
```

### Phân biệt SIGTERM vs SIGKILL
| | SIGTERM (15) | SIGKILL (9) |
|---|---|---|
| Có thể catch/ignore | Có | **Không** |
| Tiến trình dọn dẹp trước khi thoát | Có thể | Không |
| Kernel kill ngay lập tức | Không | **Có** |
| Dùng khi | Muốn tiến trình tự thoát sạch | Tiến trình không phản hồi |

## 4. SIGSTOP và SIGCONT
```bash
sleep 100 &          # chạy nền, ghi lại PID
kill -SIGSTOP <PID>  # tạm dừng (freeze) tiến trình
jobs                 # thấy trạng thái Stopped
kill -SIGCONT <PID>  # tiếp tục chạy lại
```

## 5. Lệnh Windows tương đương kill
```cmd
tasklist                          # xem danh sách process (tương đương ps -a)
taskkill /PID <PID>               # gửi terminate (tương đương SIGTERM)
taskkill /F /PID <PID>            # force kill (tương đương SIGKILL)
taskkill /IM notepad.exe /F       # kill theo tên process
```
PowerShell:
```powershell
Stop-Process -Id <PID>
Stop-Process -Name notepad
```
