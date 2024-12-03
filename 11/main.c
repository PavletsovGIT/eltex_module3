#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFFSIZE 20

const char *CHANNEL_NAME = "/tmp/fifo0001.1";
const char *FILE_NAME = "file.txt";

sem_t* read_sem, * write_sem, * sem;

void child_proccess(int num_rands);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Program needs 2 argument - <count of random numbers> <number of child proccesses>\n");
        exit(EXIT_FAILURE);
    }

    const int num_rands = atoi(argv[1]), 
              num_procs = atoi(argv[2]);
    int i = 0, count = 0, rand_num, fd_channel, fd_file, err;
    int pids[num_procs];
    char write_buf[BUFFSIZE], read_buf[BUFFSIZE];

    srand(time(NULL));
    unlink(CHANNEL_NAME);
    
    if (mkfifo(CHANNEL_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    read_sem = (sem_t*)malloc(sizeof(sem_t));
    if ((err = sem_init(read_sem, 1, num_procs)) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    write_sem = (sem_t*)malloc(sizeof(sem_t));
    if ((err = sem_init(write_sem, 1, 1)) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    sem = (sem_t*)malloc(sizeof(sem_t));
    if ((err = sem_init(sem, 1, num_procs)) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < num_procs; i++) {
        int pid;
        pid = fork();
        if (pid == 0) {
            child_proccess(num_rands);
            exit(EXIT_SUCCESS);
        }
        pids[i] = pid;
    }

    printf("PARENT PID = %d\n", getpid());

    /*
    if ((fd_channel = open(CHANNEL_NAME, O_RDONLY)) == -1) {
        perror("PARENT: Can't open channel to read");
        exit(EXIT_FAILURE);
    }*/

    while (count < num_rands) {
        for (i = 0; i < num_procs; i++) sem_wait(sem);
        snprintf(write_buf, BUFFSIZE, "data%d", count);
        if ((fd_file = open(FILE_NAME, O_WRONLY | O_TRUNC | O_CREAT, 0666)) == -1) {
            perror("PARENT: Can't open file to write");
            sem_post(write_sem);
            exit(EXIT_FAILURE);
        }
        write(fd_file, write_buf, strlen(write_buf));
        close(fd_file);
        for (i = 0; i < num_procs; i++) sem_post(sem);
        count++;
    }

    //close(fd_channel);
    puts("parent wait");
    for (i = 0; i < num_procs; ++i) {
        if (pids[i] > 0) waitpid(pids[i], NULL, 0);
    }
    puts("parent stop");
    sem_close(read_sem);
    sem_close(write_sem);
    unlink(CHANNEL_NAME);

    exit(EXIT_SUCCESS);
}

void child_proccess(int num_rands) {
    sleep(5);

    int count = 0, fd_file;
    char read_buf[BUFFSIZE];

    while (count < num_rands) {
        sem_wait(read_sem);
        if (sem_trywait(write_sem)) sem_post(write_sem);

        fd_file = open(FILE_NAME, O_RDONLY);
        if (fd_file == -1) {
            perror("CHILD: Can't open file to read");
            sem_post(read_sem);  // Освобождаем семафор
            exit(EXIT_FAILURE);
        }

        read(fd_file, read_buf, BUFFSIZE);
        close(fd_file);
        sem_post(read_sem);  // Освобождаем семафор

        printf("PID: %d : message = %s\n", getpid(), read_buf);
        count++;
    }
}