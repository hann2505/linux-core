#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libhello.h"

#define CONFIG_PATH "/etc/myapp/myapp.conf"
#define LOCAL_CONFIG "myapp.conf"

int main() {
    FILE *f = fopen(CONFIG_PATH, "r");
    if (!f) {
        printf("[!] Khong the mo file cau hinh tai %s. Thu tim tai thu muc hien tai...\n", CONFIG_PATH);
        f = fopen(LOCAL_CONFIG, "r");
    }

    if (!f) {
        fprintf(stderr, "[!] Loi: Khong tim thay file cau hinh myapp.conf!\n");
        return 1;
    }

    char greeting[128] = "Hello";
    char user[128] = "User";
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        // Xóa ký tự xuống dòng
        line[strcspn(line, "\r\n")] = '\0';

        // Bỏ qua dòng trống hoặc chú thích
        if (line[0] == '#' || strlen(line) == 0) {
            continue;
        }

        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char *key = line;
            char *val = eq + 1;

            if (strcmp(key, "greeting") == 0) {
                strncpy(greeting, val, sizeof(greeting) - 1);
            } else if (strcmp(key, "user") == 0) {
                strncpy(user, val, sizeof(user) - 1);
            }
        }
    }
    fclose(f);

    // Gọi hàm từ shared library liên kết động
    print_greeting(greeting, user);

    return 0;
}
