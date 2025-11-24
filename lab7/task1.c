#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){

    int fd[2];
    

    if (pipe(fd) < 0){
        perror("eroare la pipe!");
        return 1;
    }

    pid_t pid;
    pid = fork();
    if(pid < 0) {
        perror("eroare la fork!");
        return 1;
    }

    if(pid == 0){
        //sunt in copil aici

        close(fd[1]); //nu scriu

        char buf[4096];
        read(fd[0], buf, sizeof(buf));
        close(fd[0]); //nu mai citesc

        int sum = 0;
        char* token = strtok(buf, " ");
        while (token){
            sum += atoi(token);//atoi = toInt
            token = strtok(NULL, " ");
        }

        printf("Child: sum = %d\n", sum);



    } else {
        //sunt in parinte aici
        close(fd[0]);

        char numbers [] = "1 2 3 4 5";

        write (fd[1], numbers, strlen(numbers)+1); //+1 pentru NULL

        close(fd[1]);


    }

    return 0;

}


