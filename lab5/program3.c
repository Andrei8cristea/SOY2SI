#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <errno.h>


void handler(int){
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        char buf[64];
        int n = snprintf(buf, sizeof buf, "Kid exit %d\n", pid);
        if (n > 0) write(STDOUT_FILENO, buf, n);
        // i had to use a wirte syscall beacause printf caused unexpected behaviour
    }
    //waitpid = wait for a specfic child to change it s state 
    //pid == -1 -> means whatever child 
    //wnohang makes the waitpid non-blocking if there s no kids it will return 0


    
    

}

int main() {




    signal(SIGCHLD, handler); // when a child exits it sends sigchld 
                              //to kernel

    for(int i = 0; i < 100; i++){
        int p = fork();
        if(p == 0){
            printf("Child number %d\n", i);
            fflush(stdout);
            sleep(1);
            _exit(0);
        }
        else{
            printf("parent pid=%d created child %d (i=%d)\n", getpid(), p, i);
            fflush(stdout);
        }
    }


    while(1) pause(); // wait for kids to exit

    return 0;

}

/*
- if 2 kids die almost simoultaneously then the parent can recieve only one
  one sigchld, not one for each and every child  

- solution is to loop the waitpid () until it returns a pid <= 0

*/
