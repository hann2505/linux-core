#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    sigset_t mask, pending;

    /* Block SIGINT */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    printf("PID = %d\n", getpid());
    printf("SIGINT blocked. Press Ctrl+C then wait...\n");
    fflush(stdout);

    /* Vòng lặp: mỗi giây kiểm tra pending signals */
    while (1) {
        sleep(1);

        /* Lấy danh sách signal đang pending (đã gửi nhưng bị block) */
        sigpending(&pending);

        if (sigismember(&pending, SIGINT)) {
            printf("SIGINT is pending! Exiting...\n");
            exit(0);
        }

        printf("No SIGINT pending, still running...\n");
        fflush(stdout);
    }

    return 0;
}
