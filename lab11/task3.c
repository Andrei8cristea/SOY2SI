#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define M 5   //nr cititori
#define K 3   //nr scriitori

int a = 0;
pthread_rwlock_t rwlock;

void *reader(void *arg) {
    int id = *((int *)arg);
    free(arg);

    pthread_rwlock_rdlock(&rwlock);
    printf("Reader %d read value: %d\n", id, a);
    pthread_rwlock_unlock(&rwlock);

    return NULL;
}

void *writer(void *arg) {
    int id = *((int *)arg);
    free(arg);

    pthread_rwlock_wrlock(&rwlock);
    a = id;//modific variabila glbl protejata
    printf("Writer %d wrote value: %d\n", id, a);
    pthread_rwlock_unlock(&rwlock);

    return NULL;
}

int main(void) {
    srand(time(NULL));
    pthread_rwlock_init(&rwlock, NULL);

    int total = M + K;
    int order[total];

    //umplu vectorulcu 0 = reader 1 = writer
    for (int i = 0; i < M; i++) order[i] = 0;
    for (int i = M; i < total; i++) order[i] = 1;

    //Fisher-Yates shuffle
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }

    pthread_t threads[total];
    int reader_id = 1;
    int writer_id = 1;

    //pornesc firele in ordinea shuffled
    for (int i = 0; i < total; i++) {
        int *id = malloc(sizeof(int));

        if (order[i] == 0) {
            *id = reader_id++;
            pthread_create(&threads[i], NULL, reader, id);
        } else {
            *id = writer_id++;
            pthread_create(&threads[i], NULL, writer, id);
        }

        usleep((rand() % 40) );
    }

    //join pe toate firele
    for (int i = 0; i < total; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);
    return 0;
}
