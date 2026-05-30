/*
 * bt2/client.c - Chat client TCP cho mạng LAN
 * Biên dịch: gcc -o client client.c -lpthread
 * Chạy:      ./client <server_ip> [port]
 *
 * Ví dụ cùng máy:  ./client 127.0.0.1
 * Ví dụ qua LAN:   ./client 192.168.1.10
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE     1024
#define DEFAULT_PORT 9000

static int sock_fd;

void *recv_thread(void *arg) {
    (void)arg;
    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(sock_fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
        fflush(stdout);
    }
    printf("[Mất kết nối với server]\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Dùng: %s <server_ip> [port]\n", argv[0]);
        return 1;
    }
    const char *ip   = argv[1];
    int         port = (argc > 2) ? atoi(argv[2]) : DEFAULT_PORT;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { perror("socket"); return 1; }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(port)
    };
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        fprintf(stderr, "IP không hợp lệ: %s\n", ip); return 1;
    }
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect"); return 1;
    }
    printf("[Đã kết nối tới %s:%d] Bắt đầu chat (Ctrl+C để thoát)\n", ip, port);

    pthread_t tid;
    pthread_create(&tid, NULL, recv_thread, NULL);
    pthread_detach(tid);

    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), stdin)) {
        if (write(sock_fd, buf, strlen(buf)) < 0) {
            perror("write"); break;
        }
    }

    close(sock_fd);
    return 0;
}
