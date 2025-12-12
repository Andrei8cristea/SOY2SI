#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_RESOURCES 5
#define NUM_THREADS 8



int available_resources = MAX_RESOURCES;
pthread_mutex_t resource_mutex;

int decrease_count(int count){
    int ret = -1;
    //i lock the mutex before accessing the shared resource
    pthread_mutex_lock(&resource_mutex);
    if(available_resources >= count){ //check if enough resources are available
        available_resources -= count;
        ret = 0;
    } else {
        ret = -1;
    }
    pthread_mutex_unlock(&resource_mutex);//unlock so other threads can access the resource
    return ret;
}

int increase_count(int count){
    //lock the mutex before accessing the shared resource
    pthread_mutex_lock(&resource_mutex);
    available_resources += count; //release the resources
    pthread_mutex_unlock(&resource_mutex);
    return 0;
}

void *worker_thread(void *arg){
    int req = *((int *)arg);
    free(arg);

    for (int i = 0; i < 5; i++) {
        int want = (rand() % MAX_RESOURCES) + 1;
        while(decrease_count(want) != 0){//try to obtain the resources
            usleep(100000); //it waits before retrying
        }

        printf("Thread %d: Got %d resources, %d remaining\n", req, want, available_resources);
        usleep((rand() % 500 + 500) * 1000); //simulates work
        increase_count(want);
        printf("Thread %d: Released %d resources, %d remaining\n", req, want, available_resources);
        usleep((rand() % 500 + 500) * 1000); //simulates time before next request
    }
    return NULL;
}


int main(void){
    srand((unsigned int)time(NULL));

    pthread_mutex_init(&resource_mutex, NULL);//i initialized the mutex so it can be used

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++){
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&threads[i], NULL, worker_thread, arg);
    }

    for (int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&resource_mutex);//i destroyed the mutex after all threads have joined

    return 0;
}


