#include <stdio.h>
#include <unistd.h>

int main() {

    int x = 100;

    pid_t pid = fork();

    if (pid == 0) {

        x += 50;

        printf("Child process\n");
        printf("x = %d\n", x);

    } else {

        x -= 50;

        printf("Parent process\n");
        printf("x = %d\n", x);
    }

    return 0;
}
