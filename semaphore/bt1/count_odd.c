#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define LIMIT       100000000000LL  // 100 tỷ
#define NUM_THREADS 8

typedef struct {
    long long start;
    long long end;
    long long count;
} ThreadArg;

void *count_odd_thread(void *arg) {
    ThreadArg *a = (ThreadArg *)arg;
    long long cnt = 0;
    // Nếu start chẵn thì bắt đầu từ start+1
    long long s = (a->start % 2 == 0) ? a->start + 1 : a->start;
    for (long long i = s; i <= a->end; i += 2)
        cnt++;
    a->count = cnt;
    return NULL;
}

// Trường hợp 1: multithread
long long count_odd_multithread() {
    pthread_t threads[NUM_THREADS];
    ThreadArg args[NUM_THREADS];
    long long chunk = LIMIT / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].start = i * chunk + 1;
        args[i].end   = (i == NUM_THREADS - 1) ? LIMIT : (i + 1) * chunk;
        args[i].count = 0;
        pthread_create(&threads[i], NULL, count_odd_thread, &args[i]);
    }

    long long total = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total += args[i].count;
    }
    return total;
}

// Trường hợp 2: single thread
long long count_odd_single() {
    long long cnt = 0;
    for (long long i = 1; i <= LIMIT; i += 2)
        cnt++;
    return cnt;
}

double elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0
         + (end.tv_nsec - start.tv_nsec) / 1e6;
}

int main() {
    struct timespec t0, t1;
    long long result;

    printf("Đếm số lẻ từ 1 đến 100 tỷ\n");
    printf("Số thread: %d\n\n", NUM_THREADS);

    // Single thread
    clock_gettime(CLOCK_MONOTONIC, &t0);
    result = count_odd_single();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("[Single] Kết quả: %lld | Thời gian: %.2f ms\n", result, elapsed_ms(t0, t1));

    // Multithread
    clock_gettime(CLOCK_MONOTONIC, &t0);
    result = count_odd_multithread();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("[Multi ] Kết quả: %lld | Thời gian: %.2f ms\n", result, elapsed_ms(t0, t1));

    return 0;
}
