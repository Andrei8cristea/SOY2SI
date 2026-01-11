#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define M 5   //nr de cititori
#define K 3   //nr de scriitori

// var globala protejata
int a = 0;

//read write lock global
pthread_rwlock_t rwlock;


// ---------------------- CITITOR ----------------------
void *reader(void *arg) {
    int id = *((int *)arg);
    free(arg);

    //blochez pt citire - permit mai  multi cititori simultan
    //dar blochez scriitorii
    pthread_rwlock_rdlock(&rwlock);

    printf("Reader %d read value: %d\n", id, a);

    pthread_rwlock_unlock(&rwlock);
    return NULL;
}


// ---------------------- SCRIITOR ----------------------
void *writer(void *arg) {
    int id = *((int *)arg);
    free(arg);

    //blochez pt scriere
    pthread_rwlock_wrlock(&rwlock);

    a = id;  //scriitorul scrie id ul  propriu in var glb protejata
    printf("Writer %d wrote value: %d\n", id, a);

    pthread_rwlock_unlock(&rwlock);
    return NULL;
}


// ---------------------- MAIN ----------------------
int main(void) {
    srand(time(NULL));

    pthread_rwlock_init(&rwlock, NULL);

    pthread_t readers[M];
    pthread_t writers[K];

    //pornesc cititorii si scriitorii in ordine aleatorie
    for (int i = 0; i < M + K; i++) {
        int delay = (rand() % 40);

        if (rand() % 2 == 0 && i < M) {
            //pornesc cititor
            int *id = malloc(sizeof(int));
            *id = i + 1;
            pthread_create(&readers[i], NULL, reader, id);
        } else if (i < K) {
            //pornesc scriitor
            int *id = malloc(sizeof(int));
            *id = i + 1;
            pthread_create(&writers[i], NULL, writer, id);
        }

        usleep(delay);
    }

    //join cititori
    for (int i = 0; i < M; i++) {
        pthread_join(readers[i], NULL);
    }

    //join scriitori
    for (int i = 0; i < K; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);
    return 0;
}
