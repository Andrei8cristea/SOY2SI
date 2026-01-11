#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_THREADS 5

int barrier_n = 0;
int barrier_count = 0;

pthread_mutex_t barrier_mutex;
pthread_cond_t barrier_cond;

void barrier_init(int n) {
    barrier_n = n;
    barrier_count = 0;
    pthread_mutex_init(&barrier_mutex, NULL);
    pthread_cond_init(&barrier_cond, NULL);
}

void barrier_destroy() {
    pthread_mutex_destroy(&barrier_mutex);
    pthread_cond_destroy(&barrier_cond);
}

void barrier_point() {
    pthread_mutex_lock(&barrier_mutex);

    barrier_count++;
    //threadul curent a ajuns la bariera deci incrementez

    if (barrier_count == barrier_n) {
        //toate threadurile au ajuns la bariera
        pthread_cond_broadcast(&barrier_cond);
    } else {
        //restul asteapta
        while (barrier_count < barrier_n) {
            pthread_cond_wait(&barrier_cond, &barrier_mutex);
        }
    }

    pthread_mutex_unlock(&barrier_mutex);
}

void *tfun(void *arg) {
    int id = *((int *)arg);
    free(arg);

    printf("%d reached the barrier\n", id);
    barrier_point();
    printf("%d passed the barrier\n", id);

    return NULL;
}

int main(void) {
    srand((unsigned int)time(NULL));
    int num_threads = MAX_THREADS;

    barrier_init(num_threads);

    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;

        //creez threadurile cu intarzieri aleatoare diferite
        pthread_create(&threads[i], NULL, tfun, arg);
        usleep((rand() % 500 + 500) * 1000);
    }

    //astept toate threadurile
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    barrier_destroy();
    return 0;
}
