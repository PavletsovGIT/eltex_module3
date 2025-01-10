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

#include "logger.h"

#define BUF_SIZE        1024
#define PATH_NAME       "./file.txt"
static int proc_count = 1; /* Count of child processes */
sem_t read_sem, write_sem;

void error(const char *str);

void parent_process(int);
void child_process(int);

int main(int argc, char* argv[])
{
    if (freopen("file.log", "w+", stderr) == NULL) {
        error("freopen");
    }
    LI("func start")
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <count of random numbers>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int rand_count, pid, status;

    /* Initialization */
    srand(time(NULL));
    rand_count = atoi(argv[1]);

    if (sem_init(&read_sem, 1, 0) == -1) {
        error("sem_init");
    }

    if (sem_init(&write_sem, 1, 1) == -1) { /* Open from start */
        error("sem_init");
    }

    switch(pid = fork()) {
        case -1:
           error("fork");
        case 0:
            child_process(rand_count);
            exit(EXIT_SUCCESS);
        default:
            parent_process(rand_count);
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                printf("Child process exited with status %d\n", WEXITSTATUS(status));
            } else {
                printf("Child process did not exit normally\n");
            }
            break;
    }

    sem_destroy(&read_sem);
    sem_destroy(&write_sem);
    remove(PATH_NAME);

    LI("func end")

    exit(EXIT_SUCCESS);
}

void error(const char *str)
{
    LI("func start")
    perror(str);
    LI("func end")
    exit(EXIT_FAILURE);
}

void parent_process(int rand_count)
{
    LI("func start")
    int fd;
    char buf[BUF_SIZE];

    for (int i = 0; i < rand_count; i++) {
        // printf("Parent log: write_sem = %d\n", write_sem.value);
        if (sem_wait(&read_sem) == -1) {
            error("sem_wait");
        } // Ожидание разблокировки чтения
        // printf("Parent log: write_sem = %d\n", write_sem.value);

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

        printf("Parent read: %s\n", buf);

        // printf("Parent log: read_sem = %d\n", read_sem.value);
        if (sem_post(&write_sem) == -1) {
            error("sem_post");
        } // Разблокировка записи
        // printf("Parent log: read_sem = %d\n", read_sem.value);
    }
    LI("func end")
}

void child_process(int rand_count)
{
    LI("func start")
    int fd;
    char buf[BUF_SIZE];

    for (int i = 0; i < rand_count; i++) {
        // printf("Child log: write_sem = %d\n", write_sem.value);
        if (sem_wait(&write_sem) == -1) {
            error("sem_wait");
        } // Ожидание разблокировки записи
        // printf("Child log: write_sem = %d\n", write_sem.value);

        /* Генерация случайного числа */
        int random_num = rand() % 100;
        snprintf(buf, BUF_SIZE, "%d", random_num);
        printf("Child log: Number is generated\n");

        /* Запись числа в файл */
        if ((fd = open(PATH_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
            error("open");
        }
        if (write(fd, buf, strlen(buf)) == -1) {
            close(fd);
            error("write");
        }
        close(fd);

        printf("Child wrote: %s\n", buf);

        // printf("Child log: read_sem = %d\n", read_sem.value);
        if (sem_post(&read_sem) == -1) {
            error("sem_post");
        } // Разблокировка чтения
        // printf("Child log: read_sem = %d\n", read_sem.value);
    }
    LI("func end")
}