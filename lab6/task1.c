#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

void write_collatz(int n, char *buf, size_t sz) {
    size_t pos = 0;
    int ret;

    ret = snprintf(buf + pos, (pos < sz ? sz - pos : 0), "%d:", n);
    if (ret < 0) {
        return;
    }
    pos += (size_t)ret;

    int x = n;

    while (1) {
        ret = snprintf(buf + pos, (pos < sz ? sz - pos : 0), " %d", x);
        if (ret < 0) {
            return;
        }
        pos += (size_t)ret;

        if (x == 1) {
            break;
        }
        if (x % 2 == 0) {
            x = x / 2;
        } else {
            x = x * 3 + 1;
        }
    }

    snprintf(buf + pos, (pos < sz ? sz - pos : 0), ".\n");
}

//#################################################################
#define REGION_SIZE 4096
#define SHM_NAME "/shm_collatz_simple"
//#################################################################
int main(int argc, char *argv[]) {

    int children_counter = argc - 1;

    size_t total = REGION_SIZE * (size_t)children_counter;

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);

    if (fd < 0) {
        perror("SHM_OPEN");
        return 1;
    }

    if (ftruncate(fd, (off_t)total) < 0) {
        perror("FTRUNCATE");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    char *base = mmap(NULL, total, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (base == MAP_FAILED) {
        perror("MMAP");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    printf("Starting parent %d\n", getpid());
    fflush(stdout);

    for (int i = 0; i < children_counter; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("PID");
            munmap(base, total);
            close(fd);
            shm_unlink(SHM_NAME);
            return 1;
        }

        if (pid == 0) {
            //children

            off_t offset = (off_t)i * REGION_SIZE;
            char *region = mmap(NULL, REGION_SIZE, PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, offset);
            if (region == MAP_FAILED) {
                perror("MMAP CHILD");
                _exit(1);
            }

            printf("Done Parent %d Me %d\n", getppid(), getpid());
            fflush(stdout);

            unsigned long val_ul = strtoul(argv[i + 1], NULL, 10);
            int val = (int)val_ul;
            write_collatz(val, region, REGION_SIZE);

            munmap(region, REGION_SIZE);
            close(fd);
            _exit(0);
        }

        //parent -> continues the loop for creating all children

    }

    for (int i = 0; i < children_counter; i++) {
        wait(NULL);
    }

    for (int i = 0; i < children_counter; i++) {
        char *region = base + (size_t)i * REGION_SIZE;
        region[REGION_SIZE - 1] = '\0';
        printf("%s", region);
    }

    printf("Done Parent %d Me %d\n", getpid(), getppid());

    munmap(base, total);
    close(fd);
    shm_unlink(SHM_NAME);

    return 0;
}
