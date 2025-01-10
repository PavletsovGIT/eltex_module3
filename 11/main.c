#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUF_SIZE        1024
#define PATH_NAME       "./file.txt"
static int proc_count = 1; /* Count of child processes */
sem_t read_sem, write_sem;

void parent_process(int);
void child_process(int);

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <count of random numbers> <count of read processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int rand_count, pid, status, i;

    /* Initialization */
    srand(time(NULL));
    rand_count = atoi(argv[1]);
    proc_count = atoi(argv[2]);

    if (sem_init(&read_sem, 0, 0) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&write_sem, 0, proc_count) == -1) { /* Open from start */
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < proc_count; i++) {
        switch(pid = fork()) {
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                child_process(rand_count);
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }

    parent_process(rand_count);
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        printf("Child process exited with status %d\n", WEXITSTATUS(status));
    } else {
        printf("Child process did not exit normally\n");
    }

    sem_destroy(&read_sem);
    sem_destroy(&write_sem);
    remove(PATH_NAME);

    exit(EXIT_SUCCESS);
}

void parent_process(int rand_count)
{
    int fd, j;
    char buf[BUF_SIZE];

    for (int i = 0; i < rand_count; i++) {
        for (j = 0; j < proc_count; j++) {
            sem_wait(&write_sem); // Ожидание разблокировки записи
        }
        
        /* Генерация случайного числа */
        int random_num = rand() % 100;
        snprintf(buf, BUF_SIZE, "%d", random_num);

        /* Запись числа в файл */
        if ((fd = open(PATH_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        if (write(fd, buf, strlen(buf)) == -1) {
            perror("write");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);

        printf("Parent wrote: %s\n", buf);

        for (j = 0; j < proc_count; j++) {
            sem_post(&read_sem); // Разблокировка чтения
        }
    }
}

void child_process(int rand_count)
{
    int fd, j;
    char buf[BUF_SIZE];

    for (int i = 0; i < rand_count; i++) {
        sem_wait(&read_sem); // Ожидание разблокировки чтения

        /* Чтение числа из файла */
        if ((fd = open(PATH_NAME, O_RDONLY)) == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        if (read(fd, buf, BUF_SIZE) == -1) {
            perror("read");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);

        printf("Child read: %s\n", buf);

        sem_post(&write_sem); // Разблокировка записи
    }
}