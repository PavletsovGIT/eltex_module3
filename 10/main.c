#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFFSIZE 20
#define TIME_TO_SLEEP 1

const char *SEM_NAME = "/file_semaphore";  // Именованный семафор
const char *CHANNEL_NAME = "/tmp/fifo0001.1";
const char *FILE_NAME = "file.txt";

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Program needs 1 argument - count of random numbers\n");
        exit(EXIT_FAILURE);
    }

    int n, count = 0, rand_num;
    int pid;
    int fd_channel, fd_file;
    char write_buff[BUFFSIZE], read_buff[BUFFSIZE];
    sem_t *sem;

    n = atoi(argv[1]);
    srand(time(NULL));

    unlink(CHANNEL_NAME);

    if (mkfifo(CHANNEL_NAME, 0666) == -1) {
        perror("Can't create channel");
        exit(EXIT_FAILURE);
    }

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Failed to create semaphore");
        exit(EXIT_FAILURE);
    }

    switch (pid = fork()) {
        case -1:
            perror("fork failed");
            exit(EXIT_FAILURE);
        case 0:  // Дочерний процесс
            printf("CHILD PID = %d\n", getpid());
            fd_channel = open(CHANNEL_NAME, O_WRONLY);
            if (fd_channel == -1) {
                perror("CHILD: Can't open channel to write");
                exit(EXIT_FAILURE);
            }

            while (count < n) {
                sleep(0.5);
                sem_wait(sem);  // Захват семафора
                fd_file = open(FILE_NAME, O_RDONLY);
                if (fd_file == -1) {
                    perror("CHILD: Can't open file to read");
                    sem_post(sem);  // Освобождаем семафор
                    exit(EXIT_FAILURE);
                }

                read(fd_file, read_buff, BUFFSIZE);
                close(fd_file);
                sem_post(sem);  // Освобождаем семафор

                printf("PID: %d : message = %s\n", getpid(), read_buff);

                rand_num = rand() % 99;
                write(fd_channel, &rand_num, sizeof(int));
                count++;
            }

            close(fd_channel);
            exit(EXIT_SUCCESS);
        default:  // Родительский процесс
            printf("PARENT PID = %d\n", getpid());
            fd_channel = open(CHANNEL_NAME, O_RDONLY);
            if (fd_channel == -1) {
                perror("PARENT: Can't open channel to read");
                exit(EXIT_FAILURE);
            }

            while (count < n) {
                snprintf(write_buff, BUFFSIZE, "data%d", count);

                sem_wait(sem);  // Захват семафора
                fd_file = open(FILE_NAME, O_WRONLY | O_TRUNC | O_CREAT, 0666);
                if (fd_file == -1) {
                    perror("PARENT: Can't open file to write");
                    sem_post(sem);
                    exit(EXIT_FAILURE);
                }

                write(fd_file, write_buff, strlen(write_buff));
                close(fd_file);
                sem_post(sem);  // Освобождаем семафор

                read(fd_channel, &rand_num, sizeof(int));
                printf("PID: %d : RAND NUM = %d\n", getpid(), rand_num);
                count++;
            }

            close(fd_channel);
            wait(NULL);  // Ожидаем завершения дочернего процесса
            sem_close(sem);
            sem_unlink(SEM_NAME);
            unlink(CHANNEL_NAME);
            break;
    }

    return 0;
}
