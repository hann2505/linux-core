#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
        return 1;
    }

    pid_t target_pid = atoi(argv[1]);

    if (kill(target_pid, SIGUSR1) == 0) {
        printf("Sent SIGUSR1 to process %d\n", target_pid);
    } else {
        perror("kill");
        return 1;
    }
    return 0;
}
