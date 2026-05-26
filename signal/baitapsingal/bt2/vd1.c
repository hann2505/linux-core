#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void catch(int sig) {
    printf(" I received signal %d \n ", sig);
}

int main() {
    if (signal(SIGINT, catch) == SIG_ERR) {
        perror("SIGINT\n");
        exit(3);
    }
    while (1)
        sleep(1);
    return 0;
}
