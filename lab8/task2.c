#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define BUF_SIZE 1024

int main(){

    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    ssize_t n;

    //creez socketul
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        perror("socket");
        return 1;
    }
    printf("Socket creat cu fd: %d\n", socket_fd);

    //setez adresa serverului
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0){
        perror("inet_pton");
        close(socket_fd);
        return 1;
    }

    //ma conectez la server
    if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("connect");
        close(socket_fd);
        return 1;
    }
    printf("Conectat la server %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Scrie measajul pentru server (exit pt a iesi):\n");
    
    while(1){
        //citesc mesajul de la tastatura
        printf("=> ");   fflush(stdout);
        memset(buffer, 0, BUF_SIZE);
        if(fgets(buffer, BUF_SIZE, stdin) == NULL){
            perror("fgets");
            break;
        }

        //verific daca utilizatorul vrea sa iasa
        if(strcmp(buffer, "exit\n") == 0){
            printf("Iesire din program.\n");
            break;
        }

        //trimit mesajul la server
        n = write(socket_fd, buffer, strlen(buffer));
        if(n < 0){
            perror("write");
            break;
        }

        //primesc raspunsul de la server
        memset(buffer, 0, BUF_SIZE);
        n = read(socket_fd, buffer, BUF_SIZE - 1);
        if(n < 0){
            perror("read");
            break;
        } else if(n == 0){
            printf("Serverul a inchis conexiunea.\n");
            break;
        } else {
            buffer[n] = '\0';
            printf("Raspuns de la server: %s\n", buffer);
        }

    }

    close(socket_fd);
    printf("socket inchis.\n");

    return 0;
}