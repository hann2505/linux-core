#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>

#define MAX_MSG_LEN 256
#define MAX_MESSAGES 1000
#define FILE_PATH "chat_history.dat"

typedef struct {
    char sender[32];
    char text[MAX_MSG_LEN];
} Message;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int msg_count;
    Message messages[MAX_MESSAGES];
} ChatRoom;

#endif // COMMON_H
