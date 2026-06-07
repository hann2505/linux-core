#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

#define BASE_DIR "Test folder"

/* Hàm chuyển đổi mode_t thành chuỗi ký tự phân quyền dạng ls -l */
void get_permissions_string(mode_t mode, char *str) {
    str[0] = S_ISDIR(mode) ? 'd' : '-';
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
}

/* 
 * Hàm xóa đệ quy thư mục cũ.
 * Vì các thư mục trước đó có thể đã bị hạ quyền xuống 0664 (thiếu quyền x),
 * ta phải khôi phục quyền thực thi trước khi duyệt và xóa nội dung bên trong.
 */
void clean_directory_robust(const char *dir_path) {
    // Khôi phục quyền tạm thời để cho phép đọc và truy cập thư mục
    chmod(dir_path, 0775);

    DIR *dir = opendir(dir_path);
    if (!dir) {
        return; // Thư mục không tồn tại
    }

    struct dirent *entry;
    char path[1024];
    struct stat statbuf;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        
        // Khôi phục quyền tạm thời cho file/thư mục con
        chmod(path, 0775);

        if (stat(path, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                clean_directory_robust(path);
            } else {
                unlink(path);
            }
        } else {
            // Trường hợp lỗi stat, cố gắng unlink trực tiếp
            unlink(path);
        }
    }
    closedir(dir);

    if (rmdir(dir_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Lỗi rmdir %s: %s\n", dir_path, strerror(errno));
    }
}

/* Hàm tạo thư mục con và thiết lập quyền tạm thời */
void make_directory(const char *path, mode_t mode) {
    if (mkdir(path, mode) < 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "Lỗi tạo thư mục %s: %s\n", path, strerror(errno));
            exit(1);
        }
    }
    if (chmod(path, mode) < 0) {
        fprintf(stderr, "Lỗi chmod %s: %s\n", path, strerror(errno));
    }
}

/* Hàm tạo file mẫu và đặt quyền */
void create_file(const char *path, mode_t mode) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        fprintf(stderr, "Lỗi tạo file %s: %s\n", path, strerror(errno));
        return;
    }
    const char *data = "Noi dung bai tap 2.\n";
    write(fd, data, strlen(data));
    close(fd);

    if (chmod(path, mode) < 0) {
        fprintf(stderr, "Lỗi chmod file %s: %s\n", path, strerror(errno));
    }
}

/* Thiết lập cấu trúc thư mục đúng chuẩn theo hình vẽ */
void setup_environment() {
    printf("[1/3] Dang khoi tao cau truc thu muc theo hinh ve...\n");
    
    // 1. Dọn dẹp cấu trúc cũ
    clean_directory_robust(BASE_DIR);

    // 2. Tạo các thư mục với quyền 0775 tạm thời để có thể ghi file bên trong
    make_directory(BASE_DIR, 0775);
    make_directory(BASE_DIR "/Test1", 0775);

    // 3. Tạo các file con
    create_file(BASE_DIR "/Test1/abc", 0444); // abc: r--r--r--
    create_file(BASE_DIR "/efg", 0664);       // efg: rw-rw-r--

    // 4. Thiết lập lại quyền của thư mục cha về đúng như hình vẽ: rw-rw-r-- (0664)
    printf("-> Khoa quyen thu muc ve dung hinh ve (rw-rw-r-- / 0664)...\n");
    if (chmod(BASE_DIR "/Test1", 0664) < 0) {
        perror("Loi chmod Test1");
    }
    if (chmod(BASE_DIR, 0664) < 0) {
        perror("Loi chmod Test folder");
    }

    printf("=> Khoi tao thanh cong cấu truc thư mục '%s/'!\n\n", BASE_DIR);
}

/* Quét đệ quy tìm kiếm các file Read-Only */
void scan_read_only_files(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("| %-35s | %-12s | [Bi chan / EACCES] |\n", dir_path, "opendir() Lỗi");
        fprintf(stderr, "[!] Canh bao: Khong the mo thu muc '%s': %s (Thư mục thieu quyen thuc thi 'x')\n", 
                dir_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    char path[1024];
    struct stat statbuf;
    char perm_str[11];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        if (stat(path, &statbuf) < 0) {
            printf("| %-35s | %-12s | [Bi chan / EACCES] |\n", path, "stat() Lỗi");
            fprintf(stderr, "[!] Canh bao: Khong the doc stat cua '%s': %s (Thư mục cha thieu quyen 'x')\n", 
                    path, strerror(errno));
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            scan_read_only_files(path);
        } else if (S_ISREG(statbuf.st_mode)) {
            // File read-only: owner co quyen doc va khong co quyen ghi
            if ((statbuf.st_mode & S_IRUSR) && !(statbuf.st_mode & S_IWUSR)) {
                get_permissions_string(statbuf.st_mode, perm_str);
                printf("| %-35s | %-12s | 0%03o          |\n", 
                       path, perm_str, statbuf.st_mode & 0777);
            }
        }
    }
    closedir(dir);
}

int main() {
    // 1. Thiết lập thư mục mẫu
    setup_environment();

    // 2. Thông báo kiểm tra
    uid_t uid = getuid();
    printf("[2/3] Thong tin tien trinh:\n");
    printf("- User ID (UID): %d (%s)\n", uid, uid == 0 ? "ROOT - Co quyen bypass" : "User thuong - Se bi chan quyen 'x'");
    if (uid != 0) {
        printf("- LƯU Ý: Vi chay duoi quyen User thuong, ban se thay loi Permission Denied\n");
        printf("  khi quet qua cac thu muc thieu quyen 'x'. Hay chay bang 'sudo ./main' de quet thanh cong.\n");
    }
    printf("\n");

    // 3. Quét tệp tin chỉ đọc
    printf("[3/3] Dang quet cac file chi doc (Read-only):\n");
    printf("+-------------------------------------+--------------+----------------+\n");
    printf("| Duong dan file                      | Chuoi quyen  | Quyen (Octal)  |\n");
    printf("+-------------------------------------+--------------+----------------+\n");
    
    scan_read_only_files(BASE_DIR);
    
    printf("+-------------------------------------+--------------+----------------+\n");

    return 0;
}
