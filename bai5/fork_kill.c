#include <stdio.h>
#include <unistd.h>

int main() {

    pid_t pid = fork();

    if (pid == 0) {

        // Child
        while (1) {

            printf("Child running PID = %d\n", getpid());

            sleep(2);
        }

    } else {

        // Parent
        while (1) {

            printf("Parent running PID = %d\n", getpid());

            sleep(2);
        }
    }

    return 0;
}
