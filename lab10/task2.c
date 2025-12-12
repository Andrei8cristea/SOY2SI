#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define _POSIX_C_SOURCE 200809L
#define MAX_THREADS 5

int barrier_n = 0;

int barrier_count = 0;

pthread_mutex_t barrier_mutex;

sem_t barrier_sem;

//i initialize the barrier for n threads
void barrier_init(int n){
    barrier_n = n;
    barrier_count = 0;
    pthread_mutex_init(&barrier_mutex, NULL);
    sem_init(&barrier_sem, 0, 0);
}

//i destroy the barrier
void barrier_distroy(){
    pthread_mutex_destroy(&barrier_mutex);
    sem_destroy(&barrier_sem);
    
}

//each thread calls this to wait until all n threads arrive 
void barrier_point(){
    pthread_mutex_lock(&barrier_mutex);
    barrier_count++;
    if(barrier_count == barrier_n){
        for(int i = 0; i < barrier_n; i++){
            sem_post(&barrier_sem);
        }
    }
    pthread_mutex_unlock(&barrier_mutex);
    sem_wait(&barrier_sem);
}

//announces arrival, waits at the barrier, then announces passing
void *tfun(void *arg){
    int id = *((int *)arg);
    free(arg);

    printf("%d reached the barrier\n", id);
    barrier_point();
    printf("%d passed the barrier\n", id);

    return NULL;
}


int main(void){
    srand((unsigned int)time(NULL));
    int num_threads = MAX_THREADS;
    barrier_init(num_threads);

    pthread_t threads[num_threads];
    for(int i = 0; i < num_threads; i++){
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&threads[i], NULL, tfun, arg);
        usleep((rand() % 500 + 500) * 1000); //to mix the arrival times
    }

    //wait for all threads to finish
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
    }  

    //destroy the barrier
    barrier_distroy();
    return 0;
}






