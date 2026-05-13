#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int show_all = 0;
    int show_long = 0;

    // Không có tham số
    if (argc == 1)
    {
        printf("Hiển thị file thông thường\n");
        return 0;
    }

    // Duyệt từng tham số
    for (int i = 1; i < argc; i++)
    {
        // Kiểm tra có ký tự a không
        if (strchr(argv[i], 'a') != NULL)
        {
            show_all = 1;
        }

        // Kiểm tra có ký tự l không
        if (strchr(argv[i], 'l') != NULL)
        {
            show_long = 1;
        }
    }

    // Hiển thị kết quả
    if (show_long)
    {
        printf("Hiển thị full property (-l)\n");
    }

    if (show_all)
    {
        printf("Hiển thị file ẩn (-a)\n");
    }

    return 0;
}
