#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define DST_BIN "/usr/local/bin/myapp"
#define DST_LIB "/usr/local/lib/libhello.so"
#define DST_CFG_DIR "/etc/myapp"
#define DST_CFG "/etc/myapp/myapp.conf"

int main() {
    // 1. Kiểm tra quyền Root
    if (getuid() != 0) {
        fprintf(stderr, "[Loi] Yeu cau quyen Root (sudo) de go cai dat ung dung!\n");
        return 1;
    }

    printf("=== DANG GO CAI DAT UNG DUNG (myapp) ===\n\n");

    // 2. Xóa binary
    printf("[1/4] Dang xoa file thuc thi: %s...\n", DST_BIN);
    if (unlink(DST_BIN) < 0) {
        if (errno != ENOENT) {
            fprintf(stderr, "[Canh bao] Khong the xoa %s: %s\n", DST_BIN, strerror(errno));
        } else {
            printf("- File khong ton tai, bo qua.\n");
        }
    }

    // 3. Xóa thư viện
    printf("[2/4] Dang xoa thu vien: %s...\n", DST_LIB);
    if (unlink(DST_LIB) < 0) {
        if (errno != ENOENT) {
            fprintf(stderr, "[Canh bao] Khong the xoa %s: %s\n", DST_LIB, strerror(errno));
        } else {
            printf("- File khong ton tai, bo qua.\n");
        }
    }

    // 4. Xóa file cấu hình
    printf("[3/4] Dang xoa file cau hinh: %s...\n", DST_CFG);
    if (unlink(DST_CFG) < 0) {
        if (errno != ENOENT) {
            fprintf(stderr, "[Canh bao] Khong the xoa %s: %s\n", DST_CFG, strerror(errno));
        } else {
            printf("- File khong ton tai, bo qua.\n");
        }
    }

    // 5. Xóa thư mục cấu hình
    printf("[4/4] Dang xoa thu muc cau hinh: %s...\n", DST_CFG_DIR);
    if (rmdir(DST_CFG_DIR) < 0) {
        if (errno != ENOENT) {
            fprintf(stderr, "[Canh bao] Khong the xoa thu muc %s: %s (co the thu muc khong trong)\n", DST_CFG_DIR, strerror(errno));
        } else {
            printf("- Thu muc khong ton tai, bo qua.\n");
        }
    }

    // 6. Cập nhật cache của dynamic linker
    printf("\nUpdating dynamic linker cache (ldconfig)...\n");
    system("ldconfig");

    printf("\n=== GO CAI DAT HOAN TAT! ===\n");
    printf("============================\n");

    return 0;
}
