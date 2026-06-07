/*
 * bt3/sender.c - Gửi file qua TCP (phía "nhà" hoặc "công ty")
 * Biên dịch: gcc -o sender sender.c
 * Chạy:      ./sender <receiver_ip> <port> <file_path>
 *
 * Giao thức:
 *   1. Gửi header: "FILENAME:<tên file>\nSIZE:<byte>\n\n"
 *   2. Gửi nội dung file
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <libgen.h>

#define BUF_SIZE 65536

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Dùng: %s <ip> <port> <file>\n", argv[0]);
        return 1;
    }
    const char *ip       = argv[1];
    int         port     = atoi(argv[2]);
    const char *filepath = argv[3];

    /* Mở file cần gửi */
    FILE *fp = fopen(filepath, "rb");
    if (!fp) { perror("fopen"); return 1; }

    // Lấy kích thước file cần gửi
    struct stat st;
    if (stat(filepath, &st) < 0) { perror("stat"); fclose(fp); return 1; }
    long filesize = st.st_size;

    /* Kết nối tới receiver */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); fclose(fp); return 1; }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(port)
    };
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        fprintf(stderr, "IP không hợp lệ\n"); return 1;
    }
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect"); fclose(fp); return 1;
    }

    /* Gửi header */
    char header[512];
    char *fname = basename((char *)filepath);
    int hlen = snprintf(header, sizeof(header),
                        "FILENAME:%s\nSIZE:%ld\n\n", fname, filesize);
    write(sock, header, hlen);

    /* Gửi nội dung file */
    char buf[BUF_SIZE];
    size_t n;
    long sent = 0;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        write(sock, buf, n);
        sent += n;
        printf("\rĐã gửi: %ld / %ld bytes (%.1f%%)",
               sent, filesize, sent * 100.0 / filesize);
        fflush(stdout);
    }
    printf("\nGửi xong: %s (%ld bytes)\n", fname, filesize);

    fclose(fp);
    close(sock);
    return 0;
}
