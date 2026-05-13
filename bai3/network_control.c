#include <stdio.h>
#include <stdlib.h>

int main() {
    char interface[50];
    char ip[50];
    char command[200];
    int choice;

    printf("Nhap ten card mang (vd: eth0): ");
    scanf("%s", interface);

    while (1) {
        printf("\n===== MENU =====\n");
        printf("1. UP interface\n");
        printf("2. DOWN interface\n");
        printf("3. Set IP\n");
        printf("4. Exit\n");
        printf("Chon: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                sprintf(command, "sudo ip link set %s up", interface);
                system(command);
                printf("Interface %s da UP\n", interface);
                break;

            case 2:
                sprintf(command, "sudo ip link set %s down", interface);
                system(command);
                printf("Interface %s da DOWN\n", interface);
                break;

            case 3:
                printf("Nhap IP (vd: 192.168.1.10/24): ");
                scanf("%s", ip);

                sprintf(command,
                        "sudo ip addr add %s dev %s",
                        ip,
                        interface);

                system(command);

                printf("Da set IP %s cho %s\n", ip, interface);
                break;

            case 4:
                return 0;

            default:
                printf("Lua chon khong hop le\n");
        }
    }

    return 0;
}

