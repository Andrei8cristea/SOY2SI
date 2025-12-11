#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

long a = 0;

void *increment_thread(void *arg){
    for (long i = 0; i < 1000000; i++) {
        a++;
    }
    return NULL;
}

int main(void){

    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment_thread, NULL);
    pthread_create(&t2, NULL, increment_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("a = %ld\n", a);


    return 0;
}



//a++ se descompune in cel putin trei pasi:
//     - citire
//     - incrementare
//     - scriere
//daca doua thread-uri citesc simultan aceeasi valoare a lui a,
//ambele o vor incrementa si ambele vor scrie inapoi aceeasi
//valoare incrementata, rezultand intr-un numar final mai mic decat cel asteptat.