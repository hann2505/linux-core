#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DST_BIN "/usr/local/bin/myapp"
#define DST_LIB "/usr/local/lib/libhello.so"
#define DST_CFG_DIR "/etc/myapp"
#define DST_CFG "/etc/myapp/myapp.conf"

/* Hàm sao chép file an toàn */
int copy_file(const char *src, const char *dst, mode_t mode) {
    int src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        fprintf(stderr, "[Loi] Khong the mo file nguon '%s': %s\n", src, strerror(errno));
        return -1;
    }

    int dst_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (dst_fd < 0) {
        fprintf(stderr, "[Loi] Khong the tao file dich '%s': %s\n", dst, strerror(errno));
        close(src_fd);
        return -1;
    }

    char buf[4096];
    ssize_t bytes_read, bytes_written;
    while ((bytes_read = read(src_fd, buf, sizeof(buf))) > 0) {
        bytes_written = write(dst_fd, buf, bytes_read);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "[Loi] Loi ghi file dich '%s': %s\n", dst, strerror(errno));
            close(src_fd);
            close(dst_fd);
            return -1;
        }
    }

    close(src_fd);
    close(dst_fd);

    // Chmod trực tiếp để tránh umask can thiệp
    if (chmod(dst, mode) < 0) {
        fprintf(stderr, "[Canh bao] Khong the chmod '%s': %s\n", dst, strerror(errno));
    }
    return 0;
}

int main() {
    // 1. Kiểm tra quyền Root
    if (getuid() != 0) {
        fprintf(stderr, "[Loi] Yeu cau quyen Root (sudo) de cai dat ung dung!\n");
        return 1;
    }

    printf("=== DANG BAT DAU CAI DAT UNG DUNG (myapp) ===\n\n");

    // 2. Xác định đường dẫn nguồn
    const char *src_bin = "bin/myapp";
    const char *src_lib = "lib/libhello.so";
    const char *src_cfg = "config/myapp.conf";

    if (access(src_bin, F_OK) != 0) {
        // Nếu không chạy trực tiếp trong package/, thử chạy từ bt3/
        src_bin = "package/bin/myapp";
        src_lib = "package/lib/libhello.so";
        src_cfg = "package/config/myapp.conf";
    }

    if (access(src_bin, F_OK) != 0) {
        fprintf(stderr, "[Loi] Khong tim thay goi cai dat. Vui long build truoc bang 'make'\n");
        return 1;
    }

    // 3. Tạo thư mục cấu hình /etc/myapp
    printf("[1/4] Dang tao thu muc cau hinh: %s...\n", DST_CFG_DIR);
    if (mkdir(DST_CFG_DIR, 0755) < 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "[Loi] Khong the tao thu muc %s: %s\n", DST_CFG_DIR, strerror(errno));
            return 1;
        }
    }
    chmod(DST_CFG_DIR, 0755);

    // 4. Sao chép file cấu hình
    printf("[2/4] Dang cai dat file cau hinh: %s...\n", DST_CFG);
    if (copy_file(src_cfg, DST_CFG, 0644) < 0) {
        fprintf(stderr, "[Loi] Cai dat file cau hinh that bai!\n");
        return 1;
    }

    // 5. Sao chép thư viện liên kết động
    printf("[3/4] Dang cai dat thu vien: %s...\n", DST_LIB);
    if (copy_file(src_lib, DST_LIB, 0755) < 0) {
        fprintf(stderr, "[Loi] Cai dat thu vien that bai!\n");
        return 1;
    }

    // 6. Sao chép file thực thi binary
    printf("[4/4] Dang cai dat file thuc thi: %s...\n", DST_BIN);
    if (copy_file(src_bin, DST_BIN, 0755) < 0) {
        fprintf(stderr, "[Loi] Cai dat binary that bai!\n");
        return 1;
    }

    // 7. Cập nhật cache của dynamic linker
    printf("\nUpdating dynamic linker cache (ldconfig)...\n");
    system("ldconfig");

    printf("\n=== CAI DAT THANH CONG! ===\n");
    printf("- Chay ung dung bang cach go: `myapp` tu bat ky dau.\n");
    printf("- File cau hinh nam tai: %s\n", DST_CFG);
    printf("===========================\n");

    return 0;
}
