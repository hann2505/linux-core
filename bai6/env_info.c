#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char *home;
    char *user;

    char path[1024];
    ssize_t len;

    // Lấy biến môi trường
    home = getenv("HOME");
    user = getenv("USER");

    printf("HOME directory : %s\n", home);
    printf("USER name      : %s\n", user);

    // Lấy đường dẫn file thực thi
    len = readlink("/proc/self/exe", path, sizeof(path) - 1);

    if (len != -1)
    {
        path[len] = '\0';
        printf("Program path   : %s\n", path);
    }
    else
    {
        perror("readlink");
    }

    return 0;
}
