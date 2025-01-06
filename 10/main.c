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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <count of random numbers>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int rand_count, pid, status;

    /* Initialization */
    srand(time(NULL));
    rand_count = atoi(argv[1]);

    if (sem_init(&read_sem, 0, 0) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&write_sem, 0, proc_count) == -1) { /* Open from start */
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    switch(pid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
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

    exit(EXIT_SUCCESS);
}

void parent_process(int rand_count)
{
    int fd;
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

        printf("Parent read: %s\n", buf);

        sem_post(&write_sem); // Разблокировка записи
    }
}

void child_process(int rand_count)
{
    int fd;
    char buf[BUF_SIZE];

    for (int i = 0; i < rand_count; i++) {
        sem_wait(&write_sem); // Ожидание разблокировки записи

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

        printf("Child wrote: %s\n", buf);

        sem_post(&read_sem); // Разблокировка чтения
    }
}