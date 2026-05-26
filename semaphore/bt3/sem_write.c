#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define ITERATIONS 10   // mỗi thread tăng biến bao nhiêu lần

static int counter = 0;

/* 2 semaphore bảo vệ 2 file riêng biệt */
static sem_t sem_file0;
static sem_t sem_file1;
static FILE *fp[2];

/* mutex bảo vệ việc đọc-ghi counter */
static pthread_mutex_t cnt_mutex = PTHREAD_MUTEX_INITIALIZER;

void safe_write_file(int file_idx, int value, int thread_id) {
    sem_t *sem = (file_idx == 0) ? &sem_file0 : &sem_file1;

    sem_wait(sem);
    fprintf(fp[file_idx], "thread=%d  value=%d\n", thread_id, value);
    fflush(fp[file_idx]);
    sem_post(sem);
}

void *worker(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        /* tăng biến chung */
        pthread_mutex_lock(&cnt_mutex);
        counter++;
        int val = counter;
        pthread_mutex_unlock(&cnt_mutex);

        /* ghi vào file theo giá trị chẵn/lẻ */
        int file_idx = (val % 2 == 0) ? 0 : 1;
        safe_write_file(file_idx, val, id);
    }
    return NULL;
}

int main() {
    sem_init(&sem_file0, 0, 1);
    sem_init(&sem_file1, 0, 1);

    fp[0] = fopen("out_even.txt", "w");
    fp[1] = fopen("out_odd.txt",  "w");
    if (!fp[0] || !fp[1]) { perror("fopen"); return 1; }

    pthread_t threads[3];
    int ids[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++)
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    for (int i = 0; i < 3; i++)
        pthread_join(threads[i], NULL);

    fclose(fp[0]);
    fclose(fp[1]);
    sem_destroy(&sem_file0);
    sem_destroy(&sem_file1);
    pthread_mutex_destroy(&cnt_mutex);

    printf("Done. counter = %d\n", counter);
    printf("Even values -> out_even.txt\n");
    printf("Odd  values -> out_odd.txt\n");
    return 0;
}
