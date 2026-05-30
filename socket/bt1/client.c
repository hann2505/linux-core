/*
 * bt1/client.c - Chat client dùng Unix Domain Socket (cùng máy)
 * Biên dịch: gcc -o client client.c
 * Chạy:      ./client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/chat_socket"
#define BUF_SIZE  1024

void *recv_thread(void *arg) {
    int fd = *(int *)arg;
    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("Server: %s", buf);
        fflush(stdout);
    }
    printf("[Server đã ngắt kết nối]\n");
    exit(0);
}

int main() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect"); return 1;
    }
    printf("[Đã kết nối tới server] Bắt đầu chat (Ctrl+C để thoát)\n");

    pthread_t tid;
    pthread_create(&tid, NULL, recv_thread, &fd);
    pthread_detach(tid);

    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), stdin)) {
        if (write(fd, buf, strlen(buf)) < 0) {
            perror("write"); break;
        }
    }

    close(fd);
    return 0;
}
