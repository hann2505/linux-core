#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    char *filepath = argv[1];

    // Mở file để ghi
    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0)
    {
        perror("open");
        return 2;
    }

    char msg[] = "hello world\n";

    ssize_t bytes = write(fd, msg, strlen(msg));

    if (bytes < 0)
    {
        perror("write");
        close(fd);
        return 3;
    }

    close(fd);

    printf("Child: ghi file thanh cong\n");

    return 0;
}
