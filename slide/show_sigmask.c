#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/* Tên của 31 signal chuẩn trong Linux */
static const char *sig_names[] = {
    NULL,        /* 0 - không dùng */
    "SIGHUP",    /* 1  */
    "SIGINT",    /* 2  */
    "SIGQUIT",   /* 3  */
    "SIGILL",    /* 4  */
    "SIGTRAP",   /* 5  */
    "SIGABRT",   /* 6  */
    "SIGBUS",    /* 7  */
    "SIGFPE",    /* 8  */
    "SIGKILL",   /* 9  */
    "SIGUSR1",   /* 10 */
    "SIGSEGV",   /* 11 */
    "SIGUSR2",   /* 12 */
    "SIGPIPE",   /* 13 */
    "SIGALRM",   /* 14 */
    "SIGTERM",   /* 15 */
    "SIGSTKFLT", /* 16 */
    "SIGCHLD",   /* 17 */
    "SIGCONT",   /* 18 */
    "SIGSTOP",   /* 19 */
    "SIGTSTP",   /* 20 */
    "SIGTTIN",   /* 21 */
    "SIGTTOU",   /* 22 */
    "SIGURG",    /* 23 */
    "SIGXCPU",   /* 24 */
    "SIGXFSZ",   /* 25 */
    "SIGVTALRM", /* 26 */
    "SIGPROF",   /* 27 */
    "SIGWINCH",  /* 28 */
    "SIGIO",     /* 29 */
    "SIGPWR",    /* 30 */
    "SIGSYS",    /* 31 */
};

void print_sigmask(const char *label, sigset_t *mask) {
    printf("\n--- %s ---\n", label);
    printf("%-12s %-4s %s\n", "Signal", "No.", "Blocked?");
    printf("%-12s %-4s %s\n", "──────────", "────", "────────");

    int found = 0;
    for (int sig = 1; sig <= 31; sig++) {
        if (sigismember(mask, sig)) {
            printf("%-12s %-4d YES (blocked)\n", sig_names[sig], sig);
            found++;
        }
    }
    if (found == 0)
        printf("(no signals blocked)\n");
}

int main() {
    sigset_t current_mask;

    printf("PID = %d\n", getpid());

    /* 1. In signal mask ban đầu (thường rỗng) */
    sigprocmask(SIG_BLOCK, NULL, &current_mask);
    print_sigmask("Initial signal mask", &current_mask);

    /* 2. Block SIGINT và SIGTERM, rồi in lại */
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGINT);
    sigaddset(&block, SIGTERM);
    sigprocmask(SIG_BLOCK, &block, NULL);

    sigprocmask(SIG_BLOCK, NULL, &current_mask);
    print_sigmask("After blocking SIGINT + SIGTERM", &current_mask);

    /* 3. Unblock SIGINT, in lại */
    sigset_t unblock;
    sigemptyset(&unblock);
    sigaddset(&unblock, SIGINT);
    sigprocmask(SIG_UNBLOCK, &unblock, NULL);

    sigprocmask(SIG_BLOCK, NULL, &current_mask);
    print_sigmask("After unblocking SIGINT", &current_mask);

    /* 4. Xóa toàn bộ mask */
    sigset_t empty;
    sigemptyset(&empty);
    sigprocmask(SIG_SETMASK, &empty, NULL);

    sigprocmask(SIG_BLOCK, NULL, &current_mask);
    print_sigmask("After clearing all blocks", &current_mask);

    return 0;
}
