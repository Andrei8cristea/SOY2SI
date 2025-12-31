#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_WORD_LENGTH 1024
#define MAX_PERM_LINE_LENGTH 8192

//./encriptor <inputFile>                    => encriptor
//./encriptor <inputFile> <permutationFile>  => decriptor

void gen_permutation(int *perm, int length){
    for(int i = 0; i < length; i++){
        perm[i] = i;
    }

    //Fisher-Yates shuffle alg
    for( int i = length - 1; i > 0; i--){
        int j = rand() % (i + 1);
        //swap
        int temp = perm[i];
        perm[i] = perm[j];
        perm[j] = temp;
    }
    //parcurg vectorul de la coada la cap si dau swap cu un index random mai mic decat cel curent
}

int main(int argc, char *argv[]){
    if (argc == 2){
        //////////////////////
        ////  ENCRIPTARE  ////
        //////////////////////


        //deschid fisier input
        int fd = open(argv[1], O_RDONLY);
        if(fd == -1){
            perror("Eroare deschidere fisier input!!\n");
            return 1;
        }

        //iau lungimea fisierului
        struct stat file_stat;

        if(fstat(fd, &file_stat) == -1){
            perror("Eroare obtinere dimensiune fisier input!!\n");
            close(fd);
            return 1;
        }
        long file_size = file_stat.st_size;

        if(file_size == 0){
            printf("Eroare fisierul este gol!!\n");
            close(fd);
            return 1;
        }
        
        //mapez fisierul in memorie
        char *file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if(file_content == MAP_FAILED){
            perror("Eroare mapare fisier!!\n");
            close(fd);
            return 1;
        }//acum tin in file_content tot fisierul

        //deschid si verific fisiere output
        int enc_fd = open("encrypted.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        int perm_fd = open("permutations.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(enc_fd == -1 || perm_fd == -1){
            perror("Eroare deschidere fisiere output!!\n");
            munmap(file_content, file_size);
            close(fd);
            return 1;
        }

        //parcurg tot contentul
        //iau pt fiecare cuvant start, end si length
        //pt fiecare cuvant creez un proces care il encripteaza
        int i = 0;
        while(i < file_size){
            while (i < file_size && (file_content[i] == '\n'
                                  || file_content[i] == ' '
                                  || file_content[i] == '\t')){
                i++;
            }

            if (i >= file_size){
                break;
            }

            int start = i;

            while (i < file_size && (file_content[i] != '\n'
                                  &&  file_content[i] != ' '
                                  && file_content[i] != '\t')){
                i++;
            }

            int end = i;

            int len_word = end - start;

            //aici stiu cuvantul si pot creea procesul


            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("Eroare creare pipe!!\n");
                munmap(file_content, file_size);
                close(fd);
                close(enc_fd);
                close(perm_fd);
                return 1;
            }

            pid_t pid = fork();

            if (pid < 0) {
                perror("Eroare fork!!\n");
                munmap(file_content, file_size);
                close(fd);
                close(enc_fd);
                close(perm_fd);
                return 1;
            }


            if(pid == 0){

            //###########
            //#  COPIL  #
            //###########

            close(pipefd[0]); //inchid citirea => copilul nu citeste

            //copiez cuvantul intr un buffer
            char word[len_word + 1];
            memcpy(word, file_content + start, len_word);
            word[len_word] = '\0';


            //generez permutarea
            srand(getpid() ^ (unsigned int)time(NULL)); //initializare seed pt rand
            int perm[len_word];
            gen_permutation(perm, len_word);

            //aplic permutarea
            char encrypted_word[len_word + 1];
            for(int j = 0; j < len_word; j++){
                encrypted_word[j] = word[perm[j]];
            }
            encrypted_word[len_word] = '\0';


            //trimit parintelui lungimea, cuvantul encriptatt, permutarea folosita
            write(pipefd[1],&len_word, sizeof(int) ); 
            write(pipefd[1], encrypted_word, len_word);
            write(pipefd[1], perm, sizeof(int) * len_word);
            close(pipefd[1]); //inchid scrierea
            exit(0);//termin copilul ca sa nu treaca la urmatorul cuvant si sa 
                    //devina si el parinte
            }

            else{

            //#############
            //#  PARINTE  #
            //#############


            close(pipefd[1]); //inchid scrierea => parintele nu scrie

            //citesc datele din copil
            int len_from_child;
            read(pipefd[0], &len_from_child, sizeof(int));

            char encrypted_word_from_child[len_from_child + 1];
            read(pipefd[0], encrypted_word_from_child, len_from_child);
            encrypted_word_from_child[len_from_child] = '\0';

            int perm_from_child[len_from_child];
            read(pipefd[0], perm_from_child, sizeof(int) * len_from_child);
            
            
            close(pipefd[0]); 
            
            //scriu in fisierele de output

            write(enc_fd, encrypted_word_from_child, len_from_child);
            write(enc_fd, " ", 1); //separ cuvintele prin spatiu
                

            //scriu permutarea in fisierul de permutari
            for(int k = 0; k < len_from_child; k++){
                char buffer[20];
                int written = sprintf(buffer, "%d ", perm_from_child[k]);
                write(perm_fd, buffer, written);
            }
            write(perm_fd, "\n", 1); //separ permutarile prin



            //astept copilul
            wait(NULL);

            }
        
        
        }




        write(enc_fd, "\n\n", 2);
        close(enc_fd);
        close(perm_fd);
        munmap(file_content, file_size);
        close(fd);

    }
    else if (argc == 3){
        //////////////////////
        ////  DECRIPTARE  ////
        //////////////////////

        int fd_encrypted = open(argv[1], O_RDONLY);
        int fd_permutations = open(argv[2], O_RDONLY);
        if(fd_encrypted == -1 || fd_permutations == -1){
            perror("Eroare deschidere fisiere input pentru decriptare!!\n");
            return 1;
        }

        //fac decriptarea in fisierul decrypted.txt
        int fd_decrypted = open("decrypted.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(fd_decrypted == -1){
            perror("Eroare creare fisier decrypted.txt!!\n");
            close(fd_encrypted);
            close(fd_permutations);
            return 1;
        }

        char c;
        char encrypted_word[MAX_WORD_LENGTH];
        char perm_line[MAX_PERM_LINE_LENGTH];

        while(1){

            //citesc un cuvant encriptat

            int pos = 0;

            //sar peste whitespaces
            int bytes = 0;
            while ((bytes = read(fd_encrypted, &c, 1)) == 1 &&
                (c == ' ' || c == '\n' || c == '\t')) {
            }

            //daca am ajuns la eof ies
            if (bytes == 0) break;


            // primul caracter din cuvant
            encrypted_word[pos++] = c;

            //citesc restul cuvantului pana la urmatorul whitespace
            while (read(fd_encrypted, &c, 1) == 1 &&
                c != ' ' && c != '\n' && c != '\t') {
                encrypted_word[pos++] = c;
            }

            encrypted_word[pos] = '\0';

            //daca  nu citesc nimic ies din loop
            if (pos == 0) {
                break;
            }

            // citesc lina de permutare
            int p = 0;
            while (read(fd_permutations, &c, 1) == 1 && c != '\n') {
                perm_line[p++] = c;
            }
            perm_line[p] = '\0';


            //acum stiu cuvantul encriptat si permutarea folosita  
            
            //constriuiesc vectorul de permutare
            int perm[MAX_WORD_LENGTH];
            int len = 0;
            char *token = strtok(perm_line, " ");
            while(token != NULL){
                perm[len++] = atoi(token);//atoi converteste string la int
                token = strtok(NULL, " ");
            }

            //construiesc inversa permutarii
            int inv_perm[MAX_WORD_LENGTH];
            for(int i = 0; i < len; i++){
                inv_perm[perm[i]] = i;
            }

            char decrypted_word[MAX_WORD_LENGTH];
            for(int i = 0; i < len; i++){
                decrypted_word[i] = encrypted_word[inv_perm[i]];
            }
            decrypted_word[len] = '\0';

            //scriu cuvantul decriptat in fisier
            write(fd_decrypted, decrypted_word, len);
            write(fd_decrypted, " ", 1); //separ cuvintele prin sp

        
        }

        close(fd_encrypted);
        close(fd_permutations);
        close(fd_decrypted);


    }
    else{
        printf("Numar argumente invalid!!\n");
        return 1;
    }



}