#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int num){
    write (STDOUT_FILENO, "I won't die!\n", 13);
    //write is asynchronous
}

int main() {
    
    signal(SIGINT, handler);
    while(1){
        
        printf("PID: %d\n", getpid());
        fflush(stdout); //i have to use flush because printf is sincronous
        sleep(1);
    }

    return 0;

}

/**
I can still kill the process with:

kill <pid> = SIGTERM 
kill -SIGKILL <pid> = kill -9 <pid> 

 */

