#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define FILENAME "shared.txt"
#define ITERATIONS 5

volatile sig_atomic_t my_turn = 0;

void sigusr1_handler(int sig) {
    my_turn = 1;
}

/* Đọc số counter ở dòng cuối cùng của file */
int read_last_counter() {
    FILE *f = fopen(FILENAME, "r");
    if (!f) return 0;

    int last = 0;
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        int val;
        /* Dòng đầu chỉ có số, các dòng sau có dạng "PID=xxx counter=yyy" */
        if (sscanf(line, "PID=%*d counter=%d", &val) == 1)
            last = val;
        else if (sscanf(line, "%d", &val) == 1)
            last = val;
    }
    fclose(f);
    return last;
}

/* Ghi 1 dòng mới vào cuối file */
void write_line(pid_t pid, int counter) {
    FILE *f = fopen(FILENAME, "a");
    if (!f) { perror("fopen"); exit(1); }
    fprintf(f, "PID=%d counter=%d\n", pid, counter);
    fclose(f);
}

/*
 * Vòng lặp chung cho cả A và B:
 *   - goes_first = 1 thì process này chạy trước (A)
 *   - goes_first = 0 thì chờ signal từ partner trước (B)
 */
void run(const char *name, pid_t partner, int goes_first) {
    if (goes_first)
        my_turn = 1;

    for (int i = 0; i < ITERATIONS; i++) {
        /* Chờ đến lượt mình — dùng sigsuspend để tránh race condition */
        sigset_t waitmask;
        sigemptyset(&waitmask);   /* unblock tất cả khi suspend */

        while (!my_turn)
            sigsuspend(&waitmask);
        my_turn = 0;

        /* Đọc counter cuối, tăng lên 1, ghi dòng mới */
        int counter = read_last_counter() + 1;
        write_line(getpid(), counter);
        printf("%s (PID=%d): wrote counter=%d\n", name, getpid(), counter);
        fflush(stdout);

        /* Nhường lượt cho partner */
        kill(partner, SIGUSR1);
    }
}

int main() {
    /* Khởi tạo file với counter = 0 */
    FILE *f = fopen(FILENAME, "w");
    if (!f) { perror("fopen"); return 1; }
    fprintf(f, "0\n");
    fclose(f);

    /* Đăng ký handler và block SIGUSR1 trước khi fork
       để cả A lẫn B đều có cùng mask ngay từ đầu    */
    struct sigaction sa = { .sa_handler = sigusr1_handler, .sa_flags = 0 };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    sigset_t block_usr1;
    sigemptyset(&block_usr1);
    sigaddset(&block_usr1, SIGUSR1);
    sigprocmask(SIG_BLOCK, &block_usr1, NULL);

    pid_t child_pid = fork();
    if (child_pid < 0) { perror("fork"); return 1; }

    if (child_pid == 0) {
        /* ---- Process B (child) ---- */
        run("B", getppid(), 0 /* A đi trước */);
        exit(0);
    } else {
        /* ---- Process A (parent) ---- */
        run("A", child_pid, 1 /* A đi trước */);

        /* Chờ B kết thúc */
        while (waitpid(child_pid, NULL, 0) == -1 && errno == EINTR);

        /* In nội dung file kết quả */
        printf("\n=== %s ===\n", FILENAME);
        f = fopen(FILENAME, "r");
        char line[128];
        while (fgets(line, sizeof(line), f)) printf("%s", line);
        fclose(f);
    }
    return 0;
}
