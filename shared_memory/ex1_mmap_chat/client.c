#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include "common.h"

#define MY_NAME "Client"

ChatRoom *room = NULL;
int local_read_index = 0;

void* receiver_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&room->mutex);
        while (room->msg_count <= local_read_index) {
            pthread_cond_wait(&room->cond, &room->mutex);
        }
        
        for (int i = local_read_index; i < room->msg_count; i++) {
            if (strcmp(room->messages[i].sender, MY_NAME) != 0) {
                printf("\r[%s]: %s\n%s: ", room->messages[i].sender, room->messages[i].text, MY_NAME);
                fflush(stdout);
            }
        }
        local_read_index = room->msg_count;
        pthread_mutex_unlock(&room->mutex);
    }
    return NULL;
}

int main() {
    int fd = open(FILE_PATH, O_RDWR, 0666);
    if (fd < 0) {
        fprintf(stderr, "Error: Cannot open chat file. Make sure the Server is running first!\n");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) < 0 || st.st_size < sizeof(ChatRoom)) {
        fprintf(stderr, "Error: Chat room is not initialized. Make sure the Server is running first!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    room = (ChatRoom*)mmap(NULL, sizeof(ChatRoom), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (room == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd); // Close fd since map is active

    // Print chat history
    pthread_mutex_lock(&room->mutex);
    printf("=== Chat History ===\n");
    for (int i = 0; i < room->msg_count; i++) {
        printf("[%s]: %s\n", room->messages[i].sender, room->messages[i].text);
    }
    printf("====================\n");
    printf("Type your messages below. Type '/exit' to quit.\n\n");
    local_read_index = room->msg_count;
    pthread_mutex_unlock(&room->mutex);

    // Spawn receiver thread
    pthread_t rx_tid;
    if (pthread_create(&rx_tid, NULL, receiver_thread, NULL) != 0) {
        perror("pthread_create");
        munmap(room, sizeof(ChatRoom));
        exit(EXIT_FAILURE);
    }

    // Main thread sender loop
    char input[MAX_MSG_LEN];
    printf("%s: ", MY_NAME);
    fflush(stdout);
    while (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0; // Remove trailing newline
        
        if (strlen(input) == 0) {
            printf("%s: ", MY_NAME);
            fflush(stdout);
            continue;
        }

        if (strcmp(input, "/exit") == 0) {
            break;
        }

        pthread_mutex_lock(&room->mutex);
        if (room->msg_count < MAX_MESSAGES) {
            strncpy(room->messages[room->msg_count].sender, MY_NAME, 32);
            strncpy(room->messages[room->msg_count].text, input, MAX_MSG_LEN);
            room->msg_count++;
            pthread_cond_broadcast(&room->cond);
        } else {
            printf("\n[System]: Chat room is full!\n");
        }
        pthread_mutex_unlock(&room->mutex);

        printf("%s: ", MY_NAME);
        fflush(stdout);
    }

    // Cancel receiver thread and clean up
    pthread_cancel(rx_tid);
    pthread_join(rx_tid, NULL);
    munmap(room, sizeof(ChatRoom));

    return 0;
}
