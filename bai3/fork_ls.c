#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    // Process con
    if (pid == 0) {

        printf("Child process running...\n");

        // redirect output ls vào file
        int ret = system("ls -l > output.txt");

        // kiểm tra system success
        if (ret == 0) {
            exit(0);
        } else {
            exit(1);
        }
    }

    // Process cha
    else {

        wait(&status);

        if (WIFEXITED(status)) {

            int exit_code = WEXITSTATUS(status);

            if (exit_code == 0) {

                printf("\nChild success!\n");
                printf("Noi dung file output.txt:\n\n");

                FILE *fp = fopen("output.txt", "r");

                if (fp == NULL) {
                    perror("Cannot open file");
                    return 1;
                }

                char buffer[256];

                while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                    printf("%s", buffer);
                }

                fclose(fp);
            }

            else {
                printf("Child failed!\n");
            }
        }
    }

    return 0;
}
