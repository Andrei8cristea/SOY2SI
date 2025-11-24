#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

int main(){
    if (mkfifo("fifo1", 0666) == -1 && errno != EEXIST) {
        perror("mkfifo fifo1");
        return 1;
    }
    if (mkfifo("fifo2", 0666) == -1 && errno != EEXIST) {
        perror("mkfifo fifo2");
        return 1;
    }

    int out = open("fifo1", O_WRONLY);

    int in = open("fifo2", O_RDONLY);


    char sendbuf[100], recibuf[100];

    while(true){
        printf("A: ");
        fgets(sendbuf, sizeof(sendbuf), stdin);
        //asteapta sa se dea un input
        //se termina cand se apasa enter
        //pune input ul in sendbuf
        write(out, sendbuf, strlen(sendbuf)+1);

        read(in, recibuf,sizeof(recibuf));
        printf ("B said: %s\n", recibuf);
    }

    return 0;
}


