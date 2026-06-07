#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <unistd.h>

#define SHM_NAME "/ex3_sig_shm"
#define LOG_FILE "server_log.txt"

struct data {
    char name[75];
    int age;
};

struct shared_layout {
    pid_t server_pid;
    struct data user_data;
};

#endif // COMMON_H
