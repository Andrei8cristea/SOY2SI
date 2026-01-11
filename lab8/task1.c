#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define BUF_SIZE 1024

int main() {
    int sockets[2];
    pid_t pid;
    char buffer[BUF_SIZE];

    //creez perechea de socketuri si verific daca s a creat cu succes
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        return 1;
    }

    pid = fork();

    //creez un proces copil si verific daca s a creat cu succes
    if(pid < 0) {
        perror("fork");
        return 1;
    }

    if(pid == 0) { //proces copil
        close(sockets[0]); //inchid socketul nefolosit

        printf("[COPIL] procesul copil a pornit (pid: %x)\n", getpid());

        //deschid fisierul si verific daca s-a deschis cu succes
        FILE *f = fopen("mesaj_copil.txt", "r");
        if (f == NULL) {
            perror("[COPIL] fopen");
            close(sockets[1]);
            return 1;
        }

        //citesc mesajul din fisier
        if(fgets(buffer, BUF_SIZE, f) != NULL) {
            printf("[COPIL] mesaj citit din fisier: %s\n", buffer);
            //trimit mesajul parintelui (folosesc strlen, nu buf_size)
            if(write(sockets[1], buffer, strlen(buffer)) < 0) {
                perror("[COPIL] write");
                fclose(f);
                close(sockets[1]);
                return 1;
            }
            printf("[COPIL] mesaj trimis parintelui\n");
        } else {
            perror("[COPIL] fgets");
        }
        fclose(f);

        //astept raspuns de la parinte
        memset(buffer, 0, BUF_SIZE);
        ssize_t n = read(sockets[1], buffer, BUF_SIZE - 1);
        if(n < 0) {
            perror("[COPIL] read");
        } else if(n == 0) {
            printf("[COPIL] parintele a inchis conexiunea\n");
        } else {
            buffer[n] = '\0';
            printf("[COPIL] am primit de la parinte: %s\n", buffer);
        }

        close(sockets[1]);
        exit(EXIT_SUCCESS);

    } else { //proces parinte
        close(sockets[1]); //inchid socketul nefolosit

        printf("[PARINTE] procesul parinte (pid: %x) asteapta mesaj de la copil...\n", getpid());

        //initializez bufferul
        memset(buffer, 0, BUF_SIZE);
        
        //citesc mesajul de la copil
        ssize_t n = read(sockets[0], buffer, BUF_SIZE - 1);
        
        if(n < 0) {
            perror("[PARINTE] read");
            close(sockets[0]);
            wait(NULL);
            return 1;
        } else if(n == 0) {
            printf("[PARINTE] copilul a inchis conexiunea\n");
        } else {
            buffer[n] = '\0'; //adaug terminator de sir
            printf("[PARINTE] mesaj primit de la copil: %s\n", buffer);
        }

        //citesc mesajul din fisierul parintelui
        FILE *f = fopen("mesaj_parinte.txt", "r");
        if (f == NULL) {
            perror("[PARINTE] fopen");
            strcpy(buffer, "salut copile! (mesaj default)\n");
        } else {
            if(fgets(buffer, BUF_SIZE, f) == NULL) {
                strcpy(buffer, "salut copile! (fisier gol)\n");
            }
            fclose(f);
        }

        //trimit mesajul catre copil
        printf("[PARINTE] trimit mesaj catre copil: %s\n", buffer);
        if(write(sockets[0], buffer, strlen(buffer)) < 0) {
            perror("[PARINTE] write");
        }

        close(sockets[0]);
        wait(NULL); //astept terminarea procesului copil
        printf("[PARINTE] procesul copil s-a terminat. parintele se inchide\n");
    }

    return 0;
}
