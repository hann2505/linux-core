/*
 * bt3/receiver.c - Nhận file qua TCP (phía "nhà" hoặc "công ty")
 * Biên dịch: gcc -o receiver receiver.c
 * Chạy:      ./receiver [port]          (mặc định 9001)
 *
 * File nhận được lưu vào thư mục hiện tại với tên gốc.
 * Receiver tiếp tục lắng nghe để nhận nhiều file liên tiếp.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE     65536
#define DEFAULT_PORT 9001

/* Phân tích header, trả về số byte đã đọc của header */
int parse_header(const char *buf, int total,
                 char *filename, long *filesize) {
    /* Tìm "\n\n" kết thúc header */
    const char *end = NULL;
    for (int i = 0; i < total - 1; i++) {
        if (buf[i] == '\n' && buf[i+1] == '\n') {
            end = buf + i + 2;
            break;
        }
    }
    if (!end) return -1;

    /* Parse từng dòng */
    char tmp[512];
    strncpy(tmp, buf, end - buf);
    tmp[end - buf] = '\0';

    char *line = strtok(tmp, "\n");
    while (line) {
        if (strncmp(line, "FILENAME:", 9) == 0)
            strncpy(filename, line + 9, 255);
        else if (strncmp(line, "SIZE:", 5) == 0)
            *filesize = atol(line + 5);
        line = strtok(NULL, "\n");
    }
    return (int)(end - buf);
}

void handle_connection(int cli_fd, const char *cli_ip) {
    char buf[BUF_SIZE];
    char filename[256] = {0};
    long filesize = 0;

    /* Đọc đủ header trước */
    int header_read = 0;
    int header_end  = -1;
    while (header_end < 0 && header_read < (int)sizeof(buf) - 1) {
        ssize_t n = read(cli_fd, buf + header_read,
                         sizeof(buf) - 1 - header_read);
        if (n <= 0) break;
        header_read += n;
        buf[header_read] = '\0';
        /* Tìm \n\n */
        for (int i = 0; i < header_read - 1; i++) {
            if (buf[i] == '\n' && buf[i+1] == '\n') {
                header_end = i + 2;
                break;
            }
        }
    }

    if (parse_header(buf, header_read, filename, &filesize) < 0) {
        fprintf(stderr, "Header không hợp lệ từ %s\n", cli_ip);
        return;
    }
    printf("Nhận file '%s' (%ld bytes) từ %s\n", filename, filesize, cli_ip);

    FILE *fp = fopen(filename, "wb");
    if (!fp) { perror("fopen"); return; }

    /* Ghi phần dữ liệu đã đọc cùng header */
    long received = header_read - header_end;
    if (received > 0)
        fwrite(buf + header_end, 1, received, fp);

    /* Đọc phần còn lại */
    ssize_t n;
    while (received < filesize &&
           (n = read(cli_fd, buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, n, fp);
        received += n;
        printf("\rĐã nhận: %ld / %ld bytes (%.1f%%)",
               received, filesize, received * 100.0 / filesize);
        fflush(stdout);
    }
    fclose(fp);
    printf("\nLưu xong: %s\n", filename);
}

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;

    int srv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port        = htons(port)
    };
    if (bind(srv_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(srv_fd, 4) < 0) { perror("listen"); return 1; }

    printf("Receiver đang lắng nghe trên port %d ...\n", port);

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        int cli_fd = accept(srv_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (cli_fd < 0) { perror("accept"); continue; }

        char cli_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, sizeof(cli_ip));

        handle_connection(cli_fd, cli_ip);
        close(cli_fd);
    }

    close(srv_fd);
    return 0;
}
