#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    // Tiến trình con
    if (pid == 0)
    {
        execl("./child", "child", argv[1], NULL);

        // Nếu exec lỗi
        perror("execl");
        exit(1);
    }
    else
    {
        // Tiến trình cha
        int status;

        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            int code = WEXITSTATUS(status);

            printf("Parent: child exited with code %d\n", code);

            if (code == 0)
                printf("Parent: ghi file thanh cong\n");
            else
                printf("Parent: ghi file that bai\n");
        }
        else
        {
            printf("Parent: child terminated abnormally\n");
        }
    }

    return 0;
}
