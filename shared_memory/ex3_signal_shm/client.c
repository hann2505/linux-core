#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "common.h"

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        fprintf(stderr, "Error: Cannot open shared memory. Make sure the Server is running first!\n");
        exit(EXIT_FAILURE);
    }

    struct shared_layout *shm_ptr = (struct shared_layout*)mmap(NULL, sizeof(struct shared_layout), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    close(shm_fd);

    printf("=== Connected to Server via Shared Memory ===\n");
    printf("Server PID: %d\n\n", shm_ptr->server_pid);

    char choice[10];
    while (1) {
        struct data input_data;
        memset(&input_data, 0, sizeof(struct data));

        printf("Enter Name: ");
        if (fgets(input_data.name, sizeof(input_data.name), stdin) == NULL) {
            break;
        }
        // Remove trailing newline
        input_data.name[strcspn(input_data.name, "\n")] = '\0';

        printf("Enter Age: ");
        char age_str[20];
        if (fgets(age_str, sizeof(age_str), stdin) == NULL) {
            break;
        }
        input_data.age = atoi(age_str);

        // Write user data to shared memory
        shm_ptr->user_data = input_data;

        // Wake up server using SIGUSR1
        printf("Sending SIGUSR1 to Server (PID %d)...\n", shm_ptr->server_pid);
        if (kill(shm_ptr->server_pid, SIGUSR1) < 0) {
            perror("kill");
        } else {
            printf("Signal sent successfully.\n");
        }

        printf("Do you want to enter another record? (y/n): ");
        if (fgets(choice, sizeof(choice), stdin) == NULL) {
            break;
        }
        if (choice[0] != 'y' && choice[0] != 'Y') {
            break;
        }
        printf("\n");
    }

    munmap(shm_ptr, sizeof(struct shared_layout));
    return 0;
}
