#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {

    int fd;

    fd = open("output.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);

    if (fd < 0) {
        perror("open");
        return -1;
    }

    pid_t pid = fork();

    if (pid == 0) {

        // Child
        for (int i = 0; i < 5; i++) {

            write(fd, "Child writing\n", 15);

            sleep(1);
        }

    } else {

        // Parent
        for (int i = 0; i < 5; i++) {

            write(fd, "Parent writing\n", 16);

            sleep(1);
        }
    }

    close(fd);

    return 0;
}
