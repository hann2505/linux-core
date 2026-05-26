#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_FILES     10
#define NUMS_PER_FILE 5000000  // 5 triệu số

typedef struct {
    int file_index;
} ThreadArg;

void write_file(int idx) {
    char filename[32];
    snprintf(filename, sizeof(filename), "output_%02d.txt", idx);

    FILE *f = fopen(filename, "w");
    if (!f) { perror("fopen"); return; }

    // Mỗi thread có seed riêng để tránh trùng
    unsigned int seed = (unsigned int)(time(NULL)) ^ (idx * 1234567);
    for (int i = 0; i < NUMS_PER_FILE; i++) {
        fprintf(f, "%d\n", rand_r(&seed) % 10);
    }
    fclose(f);
}

void *thread_write(void *arg) {
    ThreadArg *a = (ThreadArg *)arg;
    write_file(a->file_index);
    return NULL;
}

double elapsed_ms(struct timespec s, struct timespec e) {
    return (e.tv_sec - s.tv_sec) * 1000.0
         + (e.tv_nsec - s.tv_nsec) / 1e6;
}

int main() {
    struct timespec t0, t1;

    printf("Tạo %d file, mỗi file %d số ngẫu nhiên (0-9)\n\n", NUM_FILES, NUMS_PER_FILE);

    // Single thread
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < NUM_FILES; i++)
        write_file(i);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("[Single] Thời gian: %.2f ms\n", elapsed_ms(t0, t1));

    // Multithread
    pthread_t threads[NUM_FILES];
    ThreadArg args[NUM_FILES];

    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < NUM_FILES; i++) {
        args[i].file_index = i;
        pthread_create(&threads[i], NULL, thread_write, &args[i]);
    }
    for (int i = 0; i < NUM_FILES; i++)
        pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("[Multi ] Thời gian: %.2f ms\n", elapsed_ms(t0, t1));

    return 0;
}
