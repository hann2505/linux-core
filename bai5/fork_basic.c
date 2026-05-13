#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        printf("Fork failed\n");
        return -1;
    }

    // Process con
    if (pid == 0) {
        printf("I am child\n");
        printf("Child PID  : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());

        sleep(30); // để còn thấy trên ps
    }

    // Process cha
    else {
        printf("I am parent\n");
        printf("Parent PID : %d\n", getpid());
        printf("Child PID  : %d\n", pid);

        sleep(30);
    }

    return 0;
}
