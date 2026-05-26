#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
 * Handler SIGCHLD: gọi waitpid() với WNOHANG trong vòng lặp để
 * reap tất cả child đã kết thúc, tránh zombie process.
 */
void sigchld_handler(int sig) {
    pid_t pid;
    int status;

    /* Dùng WNOHANG để không block nếu chưa có child nào kết thúc.
       Vòng lặp để reap nhiều child cùng lúc (signal không xếp hàng). */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status))
            printf("Child %d exited with status %d\n",
                   pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Child %d killed by signal %d\n",
                   pid, WTERMSIG(status));
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    /* SA_RESTART: tự restart các syscall bị ngắt bởi signal */
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    printf("Parent PID = %d\n", getpid());

    /* Tạo 3 tiến trình con */
    for (int i = 1; i <= 3; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            printf("  Child %d (PID=%d) starting, will sleep %d sec\n",
                   i, getpid(), i);
            sleep(i);
            printf("  Child %d (PID=%d) exiting\n", i, getpid());
            exit(i * 10);
        }
    }

    /* Parent chờ — SIGCHLD handler sẽ tự reap các con */
    printf("Parent waiting... (no zombie will appear)\n");
    while (1)
        pause();

    return 0;
}
