/*
 * bt2/server.c - Chat server TCP cho mạng LAN
 * Biên dịch: gcc -o server server.c -lpthread
 * Chạy:      ./server [port]          (mặc định port 9000)
 *
 * Hỗ trợ nhiều client cùng lúc; mỗi tin nhắn được broadcast tới tất cả.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_CLIENTS 16
#define BUF_SIZE    1024
#define DEFAULT_PORT 9000

typedef struct {
    int  fd;
    char addr[INET_ADDRSTRLEN];
    int  port;
} Client;

static Client  clients[MAX_CLIENTS];
static int     num_clients = 0;
static pthread_mutex_t clients_lock = PTHREAD_MUTEX_INITIALIZER;

/* Gửi msg tới tất cả client trừ sender */
void broadcast(const char *msg, int sender_fd) {
    pthread_mutex_lock(&clients_lock);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].fd != sender_fd)
            write(clients[i].fd, msg, strlen(msg));
    }
    pthread_mutex_unlock(&clients_lock);
}

/* Xóa client khỏi danh sách */
void remove_client(int fd) {
    pthread_mutex_lock(&clients_lock);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].fd == fd) {
            clients[i] = clients[--num_clients];
            break;
        }
    }
    pthread_mutex_unlock(&clients_lock);
}

void *handle_client(void *arg) {
    Client c = *(Client *)arg;
    free(arg);

    char buf[BUF_SIZE];
    char msg[BUF_SIZE + 64];
    ssize_t n;

    snprintf(msg, sizeof(msg), "[%s:%d đã tham gia]\n", c.addr, c.port);
    printf("%s", msg);
    broadcast(msg, c.fd);

    while ((n = read(c.fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        snprintf(msg, sizeof(msg), "[%s:%d] %s", c.addr, c.port, buf);
        printf("%s", msg);
        fflush(stdout);
        broadcast(msg, c.fd);
    }

    snprintf(msg, sizeof(msg), "[%s:%d đã rời]\n", c.addr, c.port);
    printf("%s", msg);
    broadcast(msg, c.fd);

    remove_client(c.fd);
    close(c.fd);
    return NULL;
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
    if (listen(srv_fd, MAX_CLIENTS) < 0) { perror("listen"); return 1; }

    printf("Chat server đang lắng nghe trên port %d ...\n", port);

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        int cli_fd = accept(srv_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (cli_fd < 0) { perror("accept"); continue; }

        pthread_mutex_lock(&clients_lock);
        if (num_clients >= MAX_CLIENTS) {
            pthread_mutex_unlock(&clients_lock);
            const char *full = "Server đầy, thử lại sau.\n";
            write(cli_fd, full, strlen(full));
            close(cli_fd);
            continue;
        }
        clients[num_clients].fd   = cli_fd;
        clients[num_clients].port = ntohs(cli_addr.sin_port);
        inet_ntop(AF_INET, &cli_addr.sin_addr,
                  clients[num_clients].addr, INET_ADDRSTRLEN);
        num_clients++;
        pthread_mutex_unlock(&clients_lock);

        Client *cp = malloc(sizeof(Client));
        *cp = clients[num_clients - 1];

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, cp);
        pthread_detach(tid);
    }

    close(srv_fd);
    return 0;
}
