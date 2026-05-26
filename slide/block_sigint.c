#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t mask;

    /* Tạo tập chứa SIGINT */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    /* Block SIGINT — Ctrl+C sẽ bị giữ lại, không deliver cho process */
    sigprocmask(SIG_BLOCK, &mask, NULL);

    printf("PID = %d\n", getpid());
    printf("SIGINT is now BLOCKED. Ctrl+C will have no effect.\n");
    printf("Sleeping 10 seconds...\n");
    fflush(stdout);

    sleep(10);

    /* Unblock SIGINT — nếu user đã nhấn Ctrl+C trong lúc blocked,
       signal pending sẽ được deliver ngay tại đây và kill process */
    printf("Unblocking SIGINT now...\n");
    fflush(stdout);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    printf("Done (you did not press Ctrl+C while unblocked)\n");
    return 0;
}
