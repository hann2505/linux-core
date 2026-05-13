#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        printf("Open file failed!\n");
        return -1;
    }

    printf("Open file success!\n");

    close(fd);

    return 0;
}
