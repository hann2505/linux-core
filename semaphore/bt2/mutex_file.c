#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#define OUTPUT_FILE "shared_output.txt"
#define LINES_PER_THREAD 10

static FILE *shared_fp = NULL;
static pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Wrapper function thay cho fprintf trực tiếp */
void safe_write(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    pthread_mutex_lock(&file_mutex);
    vfprintf(shared_fp, fmt, args);
    fflush(shared_fp);
    pthread_mutex_unlock(&file_mutex);

    va_end(args);
}

void *writer(void *arg) {
    int id = *(int *)arg;
    for (int i = 1; i <= LINES_PER_THREAD; i++) {
        safe_write("Thread %d - line %d\n", id, i);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    shared_fp = fopen(OUTPUT_FILE, "w");
    if (!shared_fp) { perror("fopen"); return 1; }

    pthread_create(&t1, NULL, writer, &id1);
    pthread_create(&t2, NULL, writer, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    fclose(shared_fp);
    pthread_mutex_destroy(&file_mutex);

    printf("Done. Check '%s'\n", OUTPUT_FILE);
    return 0;
}
