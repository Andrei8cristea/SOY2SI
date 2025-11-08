#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    int n = atoi(argv[1]);
    pid_t pid = fork();

    if (pid < 0){ 
        perror ("eroare la fork");
    }

    if (pid == 0){
        //copil
        printf("%d: ", n);
        while (n != 1){
            printf("%d ", n);
            if(n%2 == 0){
                n = n/2;
            }
            else n = 3 * n + 1;
        }
        printf("%d.\n", n);

    }
    else {
        //parinte
        wait(NULL);
        printf("copilul cu PID %d s a terminat\n", pid);

    }

    return 0;
}