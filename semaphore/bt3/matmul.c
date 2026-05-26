#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N           200
#define NUM_MATRICES 10
#define NUM_THREADS  4

typedef int Matrix[N][N];

static Matrix matrices[NUM_MATRICES];
static Matrix result;   /* kết quả tích lũy */
static Matrix temp;     /* buffer tạm cho từng phép nhân */

/* --- Nhân ma trận multithread --- */
typedef struct {
    int row_start;
    int row_end;
    int (*A)[N];
    int (*B)[N];
    int (*C)[N];
} MulArg;

void *mul_rows(void *arg) {
    MulArg *a = (MulArg *)arg;
    for (int i = a->row_start; i < a->row_end; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++)
                sum += a->A[i][k] * a->B[k][j];
            a->C[i][j] = sum;
        }
    }
    return NULL;
}

/* Nhân A*B -> C dùng NUM_THREADS thread */
void matmul_mt(int (*A)[N], int (*B)[N], int (*C)[N]) {
    pthread_t threads[NUM_THREADS];
    MulArg   args[NUM_THREADS];
    int chunk = N / NUM_THREADS;

    for (int t = 0; t < NUM_THREADS; t++) {
        args[t].row_start = t * chunk;
        args[t].row_end   = (t == NUM_THREADS - 1) ? N : (t + 1) * chunk;
        args[t].A = A;
        args[t].B = B;
        args[t].C = C;
        pthread_create(&threads[t], NULL, mul_rows, &args[t]);
    }
    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(threads[t], NULL);
}

void init_random(Matrix m) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            m[i][j] = rand() % 10;
}

double elapsed_ms(struct timespec s, struct timespec e) {
    return (e.tv_sec - s.tv_sec) * 1000.0
         + (e.tv_nsec - s.tv_nsec) / 1e6;
}

int main() {
    srand(42);
    printf("Khởi tạo %d ma trận %dx%d...\n", NUM_MATRICES, N, N);
    for (int i = 0; i < NUM_MATRICES; i++)
        init_random(matrices[i]);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    /* result = matrices[0] */
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            result[i][j] = matrices[0][i][j];

    /* result = result * matrices[i] lần lượt */
    for (int i = 1; i < NUM_MATRICES; i++) {
        matmul_mt(result, matrices[i], temp);
        /* copy temp -> result */
        for (int r = 0; r < N; r++)
            for (int c = 0; c < N; c++)
                result[r][c] = temp[r][c];
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("Nhân xong %d ma trận (%d thread) | Thời gian: %.2f ms\n",
           NUM_MATRICES, NUM_THREADS, elapsed_ms(t0, t1));
    printf("result[0][0] = %d\n", result[0][0]);

    return 0;
}
