#include <stdio.h>
#include "libhello.h"

void print_greeting(const char *greeting, const char *user) {
    printf("\n=========================================\n");
    printf("  %s, %s!\n", greeting, user);
    printf("=========================================\n\n");
}
