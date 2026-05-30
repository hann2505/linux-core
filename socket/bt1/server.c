/*
 * bt1/server.c - Chat server dùng Unix Domain Socket (cùng máy)
 * Biên dịch: gcc -o server server.c
 * Chạy:      ./server
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

static int client_fd = -1;

/* Thread đọc tin nhắn từ client và in ra */
void *recv_thread(void *arg) {
    int fd = *(int *)arg;
    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("Client: %s", buf);
        fflush(stdout);
    }
    printf("[Client đã ngắt kết nối]\n");
    exit(0);
}

int main() {
    /* Xóa socket cũ nếu còn */
    unlink(SOCK_PATH);

    int srv_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (srv_fd < 0) { perror("socket"); return 1; }

    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(srv_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(srv_fd, 1) < 0) { perror("listen"); return 1; }

    printf("Server đang chờ kết nối tại %s ...\n", SOCK_PATH);
    client_fd = accept(srv_fd, NULL, NULL);
    if (client_fd < 0) { perror("accept"); return 1; }
    printf("[Client đã kết nối] Bắt đầu chat (Ctrl+C để thoát)\n");

    pthread_t tid;
    pthread_create(&tid, NULL, recv_thread, &client_fd);
    pthread_detach(tid);

    /* Main thread: đọc stdin và gửi cho client */
    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), stdin)) {
        if (write(client_fd, buf, strlen(buf)) < 0) {
            perror("write"); break;
        }
    }

    close(client_fd);
    close(srv_fd);
    unlink(SOCK_PATH);
    return 0;
}
