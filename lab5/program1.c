#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main() {
    

    while(1){
        
        printf("PID: %d\n", getpid());
        fflush(stdout); //i have to use flush because printf is sincronous
        sleep(1);
    }

    return 0;

}

/**
kill <pid> = SIGTERM = "politely asks the process to terminate"
kill -SIGINT <pid> = kill -2 <pid> = ctrl + C 
kill -SIGKILL <pid> = kill -9 <pid> = forced terminate of the process
 
kill -SIGSTOP <pid> = kill -19 <pid> = pausing the process
kill -SIGCONT <pid> = kill -18 <pid> = continuing the paused process
*/