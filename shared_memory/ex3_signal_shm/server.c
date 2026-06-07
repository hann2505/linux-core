#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include "common.h"

struct shared_layout *shm_ptr = NULL;
volatile sig_atomic_t sig_received = 0;
volatile sig_atomic_t keep_running = 1;

void handle_sigusr1(int sig) {
    (void)sig;
    sig_received = 1;
}

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

int main() {
    // Unlink any existing shared memory of the same name to start fresh
    shm_unlink(SHM_NAME);

    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(struct shared_layout)) < 0) {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    shm_ptr = (struct shared_layout*)mmap(NULL, sizeof(struct shared_layout), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    close(shm_fd); // Close fd since map is active

    // Write server PID to shared memory
    shm_ptr->server_pid = getpid();
    memset(&shm_ptr->user_data, 0, sizeof(struct data));

    // Register signal handlers
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
        perror("sigaction SIGUSR1");
        munmap(shm_ptr, sizeof(struct shared_layout));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    struct sigaction sa_int;
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) < 0) {
        perror("sigaction SIGINT");
        munmap(shm_ptr, sizeof(struct shared_layout));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    printf("Server PID: %d\n", getpid());
    printf("Server initialized shared memory: %s\n", SHM_NAME);
    printf("Waiting for client signals (SIGUSR1)... Press Ctrl+C to exit.\n\n");

    while (keep_running) {
        printf("Server: Going to sleep...\n");
        fflush(stdout);

        // Suspend until a signal is received
        pause();

        if (!keep_running) {
            break;
        }

        if (sig_received) {
            sig_received = 0; // Reset flag

            printf("Server: Waking up! Reading data from shared memory...\n");
            
            // Read from shared memory
            char name[75];
            int age;
            
            // Safe copy of name (null-terminated)
            strncpy(name, shm_ptr->user_data.name, sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';
            age = shm_ptr->user_data.age;

            printf("Server: Received Name = \"%s\", Age = %d\n", name, age);

            // Log to file
            FILE *lf = fopen(LOG_FILE, "a");
            if (lf != NULL) {
                time_t now = time(NULL);
                char *time_str = ctime(&now);
                // Remove newline from time string
                time_str[strlen(time_str) - 1] = '\0';
                
                fprintf(lf, "[%s] Name: %s, Age: %d\n", time_str, name, age);
                fclose(lf);
                printf("Server: Logged successfully to %s\n\n", LOG_FILE);
            } else {
                perror("fopen log file");
            }
        }
    }

    printf("\nServer exiting, cleaning up shared memory...\n");
    munmap(shm_ptr, sizeof(struct shared_layout));
    shm_unlink(SHM_NAME);

    return 0;
}
