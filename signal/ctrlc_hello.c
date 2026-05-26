#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int sig) {
    printf("hello world\n");
    fflush(stdout);
}

int main() {
    signal(SIGINT, sigint_handler);

    printf("Press Ctrl+C to print hello world. Press Ctrl+\\ to quit.\n");
    while (1) {
        pause();
    }
    return 0;
}
