#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* Tên signal tương ứng với số hiệu (0-31) */
static const char *sig_names[] = {
    "Unknown",  "SIGHUP",  "SIGINT",  "SIGQUIT", "SIGILL",
    "SIGTRAP",  "SIGABRT", "SIGBUS",  "SIGFPE",  "SIGKILL",
    "SIGUSR1",  "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM",
    "SIGTERM",  "SIGSTKFLT","SIGCHLD","SIGCONT", "SIGSTOP",
    "SIGTSTP",  "SIGTTIN", "SIGTTOU", "SIGURG",  "SIGXCPU",
    "SIGXFSZ",  "SIGVTALRM","SIGPROF","SIGWINCH","SIGIO",
    "SIGPWR",   "SIGSYS"
};

void handler(int sig) {
    const char *name = (sig >= 0 && sig <= 31) ? sig_names[sig] : "Unknown";
    /* Dùng write() thay printf() vì printf không async-signal-safe */
    char buf[64];
    int len = snprintf(buf, sizeof(buf),
                       "Received signal %d (%s)\n", sig, name);
    write(STDOUT_FILENO, buf, len);
}

int main() {
    /* Đăng ký handler cho tất cả signal có thể catch
       SIGKILL (9) và SIGSTOP (19) KHÔNG thể catch/ignore */
    int catchable[] = {
        SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT,
        SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2, SIGPIPE,
        SIGALRM, SIGTERM, SIGCHLD, SIGCONT,
        SIGTSTP, SIGTTIN, SIGTTOU
    };
    int n = sizeof(catchable) / sizeof(catchable[0]);

    for (int i = 0; i < n; i++)
        signal(catchable[i], handler);

    printf("PID = %d\n", getpid());
    printf("Listening for signals... (SIGKILL/SIGSTOP cannot be caught)\n");
    printf("Send signals with: kill -<SIG> %d\n", getpid());
    printf("Press Ctrl+C (SIGINT) or Ctrl+\\ (SIGQUIT) to test.\n\n");

    while (1)
        pause();

    return 0;
}
