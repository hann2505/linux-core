#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

int main(int argc, char *argv[])
{
    FILE *fp;
    char line[MAX_LINE];

    // argv[1] = file script
    // argv[2] = tham số truyền vào ($1)

    if (argc < 2)
    {
        printf("Usage: %s <script> [arg]\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        perror("fopen");
        return 1;
    }

    while (fgets(line, sizeof(line), fp))
    {
        // bỏ dòng shebang
        if (strncmp(line, "#!", 2) == 0)
            continue;

        // thay $1 bằng tham số
        if (argc >= 3)
        {
            char *pos = strstr(line, "$1");

            if (pos)
            {
                char temp[MAX_LINE];

                *pos = '\0';

                snprintf(temp,
                         sizeof(temp),
                         "%s%s%s",
                         line,
                         argv[2],
                         pos + 2);

                strcpy(line, temp);
            }
        }

        // xóa newline
        line[strcspn(line, "\n")] = 0;

        printf("Execute: %s\n", line);

        system(line);
    }

    fclose(fp);

    return 0;
}
