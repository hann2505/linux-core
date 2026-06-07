#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

#define BASE_DIR "parent_dir"

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

/* Hàm xóa đệ quy thư mục cũ để mỗi lần chạy đều bắt đầu mới */
void clean_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        // Thư mục không tồn tại, không cần xóa
        return;
    }

    struct dirent *entry;
    char path[1024];
    struct stat statbuf;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        if (stat(path, &statbuf) < 0) {
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            clean_directory(path);
        } else {
            if (unlink(path) < 0) {
                fprintf(stderr, "Không thể xóa file %s: %s\n", path, strerror(errno));
            }
        }
    }
    closedir(dir);

    if (rmdir(dir_path) < 0) {
        fprintf(stderr, "Không thể xóa thư mục %s: %s\n", dir_path, strerror(errno));
    }
}

/* Hàm tạo thư mục con với chế độ phân quyền cụ thể */
void make_directory(const char *path, mode_t mode) {
    if (mkdir(path, mode) < 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "Lỗi tạo thư mục %s: %s\n", path, strerror(errno));
            exit(1);
        }
    }
    // Sử dụng chmod để bỏ qua ảnh hưởng của umask hệ thống
    if (chmod(path, mode) < 0) {
        fprintf(stderr, "Lỗi thiết lập quyền cho thư mục %s: %s\n", path, strerror(errno));
    }
}

/* Hàm tạo file mẫu và đặt chế độ phân quyền cụ thể */
void create_file(const char *path, mode_t mode) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        fprintf(stderr, "Lỗi tạo file %s: %s\n", path, strerror(errno));
        return;
    }
    // Ghi một vài dữ liệu mẫu
    const char *data = "Du lieu mau cho file system test.\n";
    write(fd, data, strlen(data));
    close(fd);

    // Dùng chmod để gán quyền chính xác
    if (chmod(path, mode) < 0) {
        fprintf(stderr, "Lỗi thiết lập quyền cho file %s: %s\n", path, strerror(errno));
    }
}

/* Tạo cấu trúc thư mục mẫu */
void setup_environment() {
    printf("[1/3] Dang khoi tao cau truc thu muc mau...\n");
    
    // Xóa thư mục cũ nếu có
    clean_directory(BASE_DIR);

    // Tạo thư mục cha: parent_dir
    make_directory(BASE_DIR, 0755);

    // Tạo các file trong thư mục cha
    create_file(BASE_DIR "/file_read_only.txt", 0444);  // r--r--r--
    create_file(BASE_DIR "/file_read_write.txt", 0644); // rw-r--r--

    // Tạo thư mục con 1: sub_dir1
    make_directory(BASE_DIR "/sub_dir1", 0755);
    create_file(BASE_DIR "/sub_dir1/file_owner_read.txt", 0400); // r--------
    create_file(BASE_DIR "/sub_dir1/file_executable.sh", 0755);  // rwxr-xr-x

    // Tạo thư mục con 2: sub_dir2
    make_directory(BASE_DIR "/sub_dir2", 0755);
    create_file(BASE_DIR "/sub_dir2/file_read_only_2.txt", 0444); // r--r--r--
    create_file(BASE_DIR "/sub_dir2/file_write_only.txt", 0200);  // -w-------

    printf("=> Khoi tao thanh cong cau truc thu muc '%s/'!\n\n", BASE_DIR);
}

/* Quét đệ quy tìm kiếm các file Read-Only */
void scan_read_only_files(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "Loi khong the mo thu muc %s: %s\n", dir_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    char path[1024];
    struct stat statbuf;
    char perm_str[11];

    while ((entry = readdir(dir)) != NULL) {
        // Bỏ qua thư mục hiện tại '.' và cha '..'
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Tạo đường dẫn tuyệt đối/tương đối đầy đủ
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        // Lấy thông tin inode/stat của tệp tin
        if (stat(path, &statbuf) < 0) {
            fprintf(stderr, "Loi stat file %s: %s\n", path, strerror(errno));
            continue;
        }

        // Nếu là thư mục con, tiến hành đệ quy
        if (S_ISDIR(statbuf.st_mode)) {
            scan_read_only_files(path);
        }
        // Nếu là file thông thường, kiểm tra xem có phải read-only không
        else if (S_ISREG(statbuf.st_mode)) {
            /* 
             * Một file được xem là "Read-only" nếu:
             * 1. Chủ sở hữu (owner) có quyền Đọc (S_IRUSR)
             * 2. Chủ sở hữu KHONG có quyền Ghi (S_IWUSR)
             *
             * Chúng ta có thể kiểm tra thêm: không ai có quyền ghi (mode & 0222 == 0)
             * Nhưng kiểm tra trên owner là chuẩn nhất trong hệ thống Linux cho "read-only đối với tiến trình chạy".
             */
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
    // 1. Thiết lập cấu trúc thư mục mẫu
    setup_environment();

    // 2. Liệt kê toàn bộ file để đối chiếu (sử dụng lệnh shell hoặc hiển thị)
    printf("[2/3] Danh sach cac file vua tao va quyen tuong ung:\n");
    printf("-----------------------------------------------------------------\n");
    printf("Chay lenh `find parent_dir -type f -exec ls -l {} +` de doi chieu:\n\n");
    fflush(stdout);
    
    // Gọi lệnh hệ thống để hiển thị trực quan cấu trúc vừa tạo
    system("find parent_dir -type f | xargs ls -l 2>/dev/null || ls -lR parent_dir");
    printf("\n");

    // 3. Quét tìm kiếm và hiển thị các file read-only
    printf("[3/3] Dang tim kiem cac file co che do chi doc (Read-only):\n");
    printf("+-------------------------------------+--------------+----------------+\n");
    printf("| Duong dan file                      | Chuoi quyen  | Quyen (Octal)  |\n");
    printf("+-------------------------------------+--------------+----------------+\n");
    
    scan_read_only_files(BASE_DIR);
    
    printf("+-------------------------------------+--------------+----------------+\n");

    return 0;
}
