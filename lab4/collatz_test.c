#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    printf("pid parinte =  %d\n", getpid());

    for (int i = 1; i < argc; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("eroare la fork");
        }

        if (pid == 0) {
            //copil
            char *args[] = {"./collatz", argv[i], NULL};
            execve("./collatz", args, NULL);
            perror("eroare la execve");
        }
    }

    //parinte
    for (int i = 1; i < argc; i++) {
        wait(NULL);
    }

    return 0;
}