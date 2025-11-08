#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("eroare la fork");
    }

    if (pid == 0) {
        //proces copil
        
        char *args[] = {"/usr/bin/ls", NULL}; 
        printf("PID proces copil: %d\n", getpid());
        execve("/usr/bin/ls", args, NULL);

        perror("eroare execve");
    } else {
        //proces parinte
        wait(NULL);
        printf("PID proces initial: %d\n", getpid());
        printf("proces copil cu PID: %d terminat\n", pid);
    }

    return 0;
}