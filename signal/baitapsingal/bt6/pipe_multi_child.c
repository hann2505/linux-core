#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    int i, val;

    if (pipe(fd) == -1) {
        printf("error pipe\n");
        exit(2);
    }

    /* Tạo 3 tiến trình con (i = 1, 2, 3) */
    for (i = 1; i < 4; i++) {
        switch (fork()) {
        case -1:
            printf("error fork %d\n", i);
            exit(1);
        case 0:
            /* Child: ghi giá trị i*100 vào pipe rồi thoát */
            close(fd[0]);
            val = i * 100;
            write(fd[1], &val, sizeof(val));
            close(fd[1]);
            printf("child thu %d , (pid %d)  send value %d\n",
                   i, getpid(), val);
            exit(0);
        default:
            break;
        }
    }

    /* Parent: đợi 3s để các con kịp ghi, rồi đọc hết */
    printf("parent pid %d\n", getpid());
    sleep(3);
    close(fd[1]);

    while (read(fd[0], &val, sizeof(val)) != 0)
        printf("Parent (%d) received value: %d\n", getpid(), val);

    close(fd[0]);

    /* Reap tất cả child */
    while (wait(NULL) > 0);

    exit(0);
}
