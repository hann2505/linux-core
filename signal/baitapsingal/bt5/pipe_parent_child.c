#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int fd[2];
    int val = 0;

    pipe(fd);   /* tạo pipe: fd[0]=đọc, fd[1]=ghi */

    if (fork() != 0) {
        /* Parent: chỉ ghi → đóng đầu đọc */
        close(fd[0]);

        val = 100;
        write(fd[1], &val, sizeof(val));
        printf("Parent(%d) send value: %d\n", getpid(), val);

        close(fd[1]);
        wait(NULL);
    } else {
        /* Child: chỉ đọc → đóng đầu ghi */
        close(fd[1]);

        read(fd[0], &val, sizeof(val));   /* block cho đến khi có dữ liệu */
        printf("Child(%d) received value: %d\n", getpid(), val);

        close(fd[0]);
    }
    exit(0);
}
