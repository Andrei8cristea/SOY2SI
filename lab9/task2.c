#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int m, p, n;
double *A;
double *B;
double *C;

void *compute_element(void* arg){
    int *ij = (int*)arg;
    int i = ij[0];
    int j = ij[1];
    free(ij);

    double sum = 0.0;
    for(int k = 0; k < p; k++){
        sum += A[i * p + k] * B[k * n + j];
    }
    C[i * n + j] = sum;
    return NULL;
}

int main(void){

    scanf("%d %d %d", &m, &p, &n);

    A = malloc(sizeof(double) * m * p);
    B = malloc(sizeof(double) * p * n);
    C = malloc(sizeof(double) * m * n);

    for (int i = 0; i < m * p; ++i) scanf("%lf", &A[i]);
    for (int i = 0; i < p * n; ++i) scanf("%lf", &B[i]);


    pthread_t *threads = malloc(sizeof(pthread_t) * m * n);


    int index = 0;
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            int *ij = malloc(2 * sizeof(int));
            if (!ij) { perror("malloc"); return 6; }
            ij[0] = i;
            ij[1] = j;
            if (pthread_create(&threads[index], NULL, compute_element, ij)) {
                perror("pthread_create");
                free(ij);
                return 1;
            }
            index++;
        }
    }

    for(int t = 0; t < m * n; t++){
        pthread_join(threads[t], NULL);
    }

    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            printf("%f ", C[i * n + j]);
        }
        printf("\n");
    }

    free(A);
    free(B);
    free(C);
    free(threads);
    return 0;
}
