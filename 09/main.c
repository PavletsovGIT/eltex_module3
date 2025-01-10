#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUF_SIZE  	  100
#define SEM_COUNT 	  1 /* Count of needed semaphores */
#define PATH_NAME 	  "./file.txt" /* Name of file for IPC */
#define SEM_READ_KEY  "/tmp/sem-read-key"
#define SEM_WRITE_KEY "/tmp/sem-write-key"

void parent_process(int read_sem, int write_sem, int count_rn);
void child_process(int read_sem, int write_sem, int count_rn);

/* Операция над семафором */
void sem_op(int semid, int op) {
    struct sembuf sb = {0, op, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

static int count_pr; /* Кол-во процессов */

int main(int argc, char *argv[]) 
{
	if (argc != 3) {
        fprintf(stderr, "Usage: %s <count of random numbers> <count of child processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int count_rn, pid, status, fd, i;
    key_t sem_key;
    int read_sem, write_sem;

    srand(time(NULL)); /* Инициализация генератора случайных чисел */

    count_rn = atoi(argv[1]); /* Количество случайных чисел */
    count_pr = atoi(argv[2]); /* Кол-во процессов читающих файл */

    fd = open(SEM_READ_KEY, O_CREAT);
    close(fd);

    fd = open(SEM_WRITE_KEY, O_CREAT);
    close(fd);

    /* Инициализация семафора чтения */
    if ((sem_key = ftok(SEM_READ_KEY, 'a')) == -1) {
    	perror("ftok");
    	exit(EXIT_FAILURE);
    }
    if ((read_sem = semget(sem_key, 1, 0660 | IPC_CREAT)) == -1) {
    	perror("semget");
    	exit(EXIT_FAILURE);
    }
    semctl(read_sem, 0, SETVAL, 0); // Изначально заблокирован

    /* Инициализация семафора записи */
    if ((sem_key = ftok(SEM_WRITE_KEY, 'a')) == -1) {
    	perror("ftok");
    	exit(EXIT_FAILURE);
    }
    if ((write_sem = semget(sem_key, 1, 0660 | IPC_CREAT)) == -1) {
    	perror("semget");
    	exit(EXIT_FAILURE);
    }
    semctl(write_sem, 0, SETVAL, count_pr); // Изначально разблокирован

    for (i = 0; i < count_pr; i++) {
    	/* Создание дочернего процесса */
    	switch (pid = fork()) {
    		case -1:
    			perror("fork");
    			exit(EXIT_FAILURE);
    		case 0: /* Дочерний процесс */
    			child_process(read_sem, write_sem, count_rn);
    			exit(EXIT_SUCCESS);
    		default:
    			break;
    	}
    }

    parent_process(read_sem, write_sem, count_rn);
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
   	    printf("Child process exited with status %d\n", WEXITSTATUS(status));
    } else {
        printf("Child process did not exit normally\n");
   	}

    /* Удаление семафоров */
	semctl(read_sem, 0, IPC_RMID);
    semctl(write_sem, 0, IPC_RMID);

    remove(PATH_NAME);
    remove(SEM_READ_KEY);
    remove(SEM_WRITE_KEY);
	exit(EXIT_SUCCESS);
}

void parent_process(int read_sem, int write_sem, int count_rn)
{
    int fd;
    char buf[BUF_SIZE];

    for (int i = 0; i < count_rn; i++) {
        sem_op(write_sem, -count_pr); // Ожидание разблокировки записи

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

        printf("Parent PID %d,\twrote:\t%s\n", getpid(), buf);

        sem_op(read_sem, count_pr); // Разблокировка чтения
    }
}

void child_process(int read_sem, int write_sem, int count_rn)
{
    int fd;
    char buf[BUF_SIZE];

    for (int i = 0; i < count_rn; i++) {
        sem_op(read_sem, -1); // Ожидание разблокировки чтения

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

        printf("Child PID %d,\tread:\t%s\n", getpid(), buf);

        sem_op(write_sem, 1); // Разблокировка записи
    }
}
