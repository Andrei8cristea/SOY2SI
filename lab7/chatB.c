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

    int in = open("fifo1", O_RDONLY);


    int out = open("fifo2", O_WRONLY);


    char sendbuf[100], recibuf[100];

    while(true){
        read(in, recibuf,sizeof(recibuf));
        printf ("A said: %s\n", recibuf);        

        printf("B: ");
        fgets(sendbuf, sizeof(sendbuf), stdin);
        write(out, sendbuf, strlen(sendbuf)+1);
    }

    return 0;
}


