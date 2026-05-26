#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("SIGINT\n");
        exit(3);
    }
    printf("SIGINT is now ignored. Press Ctrl+C - nothing will happen.\n");
    printf("Use Ctrl+\\ (SIGQUIT) or 'kill -9 <PID>' to exit.\n");
    while (1)
        sleep(1);
    return 0;
}
