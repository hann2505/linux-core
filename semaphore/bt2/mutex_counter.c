#include <stdio.h>
#include <pthread.h>

#define NUM_INCREMENTS 1000000

long long counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    printf("Thread %d done\n", id);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    pthread_create(&t1, NULL, increment, &id1);
    pthread_create(&t2, NULL, increment, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);

    printf("Expected: %d\n", NUM_INCREMENTS * 2);
    printf("Result  : %lld\n", counter);
    printf("Match   : %s\n", counter == NUM_INCREMENTS * 2 ? "YES" : "NO");

    return 0;
}
