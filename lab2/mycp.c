#include <fcntl.h>      // open
#include <unistd.h>     // read, write, close
#include <sys/stat.h>   // permisiuni fișier
#include <stdlib.h>     // exit
#include <errno.h>      // errno
#include <string.h>     // strerror
#include <stdio.h>      // doar pentru perror (opțional)

#define BUF_SIZE 4096

int main(int argc, char *argv[]){
	if(argc != 3){
		write(STDERR_FILENO, "NU s au oferit 3 argumente\n", 28);
		exit(1);
	}

	int src_fd = open(argv[1],O_RDONLY);
	if(src_fd < 0){
		perror("Eroare la deschiderea fisierului sursa");
		exit(1);
	}

	int dest_fd =  open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(dest_fd < 0 ){
		perror("Eroare la deshiderea fisierului destinatie!!!");
		close(src_fd);
		exit(1);
	}

	char buffer[BUF_SIZE];
	ssize_t bytes_read, bytes_written;
	while ((bytes_read = read(src_fd,buffer, BUF_SIZE)) > 0){
		bytes_written = write(dest_fd, buffer, bytes_read);
		if (bytes_written != bytes_read){
			write(STDERR_FILENO, "Eroare la scrierea in fisier/n",28);
			close(src_fd);
            close(dest_fd);
            exit(1);
		}
	}

	close(src_fd);
	close(dest_fd);
    write(STDOUT_FILENO, "Succes!!!\n", 10);      

	

}
