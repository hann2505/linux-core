#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigusr1_handler(int sig) {
    printf("Woken up by SIGUSR1! Resuming...\n");
    fflush(stdout);
}

int main() {
    signal(SIGUSR1, sigusr1_handler);

    printf("Sleeper PID: %d\n", getpid());
    printf("Sleeping indefinitely. Send SIGUSR1 to wake me up.\n");
    fflush(stdout);

    while (1) {
        pause();
        printf("Back to sleep...\n");
        fflush(stdout);
    }
    return 0;
}
