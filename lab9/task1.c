
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *reverse_thread(void *arg){
    char *s = arg;
    size_t len = strlen(s);
    char *reversed = malloc(len + 1);
    //copipez caracter cu caracter in ordine inversa
    for (size_t i = 0; i < len; i++) {
        reversed[i] = s[len - 1 - i];
    }
    reversed[len] = '\0';
    return reversed;
}

int main(int argc, char**argv){



    if (argc != 2) {
        fprintf(stderr, "argument number error", argv[0]);
        return 1;
    }

    pthread_t thread;
    //creez threadul ii transmit stringul ca argument si apelez functia 
    // de inversare de caractere
    if(pthread_create(&thread, NULL, reverse_thread, argv[1]) != 0) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    void *result;
    //pun rezultatul intr un pointer de tip void
    if(pthread_join(thread, &result) != 0) {
        fprintf(stderr, "Error joining thread\n");
        return 1;
    }

    char *reversed_str = (char *)result;
    printf("Reversed string: %s\n", reversed_str);
    free(reversed_str);


    return 0;
}