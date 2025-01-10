#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>  // Для waitpid

#include "logger.h"

#define BUFFSIZE 1024  // Увеличим размер буфера
#define TIME_TO_SLEEP 1

static int flag = 0; // 1 - Процесс-потомок находится в ожидании, 0 - процесс потомок может открыть файл и прочитать данные

void sigusr1_handler(int sig); // Обработчик сигнала SIGUSR1
void sigusr2_handler(int sig); // Обработчик сигнала SIGUSR2

void parent_process(int child_pid, int count_nums, const char *pathname);
void child_process(int count_nums, const char *pathname);

void wait_varx(int *variable, int value); /* Блокирует работу процесса */
void error(const char *strerr);

int main(int argc, char* argv[]) 
{
    if (freopen("file.log", "w+", stderr) == NULL) {
        error("freopen");
    }
    
    LI("func start")
    if (argc != 2) {
        perror("Program needs 1 argument - count of random numbers");
        exit(EXIT_FAILURE);
    }

    int n, status, pid, fd; 
    char file[] = "file.txt";

    remove(file);
    if ((fd = open(file, O_CREAT, 0660)) == -1) {
        error("open");
    }
    close(fd);

    srand(time(NULL));
    signal(SIGUSR1, sigusr1_handler);  // Блокировка доступа
    signal(SIGUSR2, sigusr2_handler);  // Разрешение доступа

    n = atoi(argv[1]);
    flag = 1;

    switch (pid = fork()) {
        case -1:
            perror("fork failed");
            exit(EXIT_FAILURE);
        case 0: // Дочерний процесс
            child_process(n, file);
            exit(EXIT_SUCCESS);

        default: // Родительский процесс
            parent_process(pid, n, file);
            break;
    }

    /* Ожидаем окончание доченрнего процесса */
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Child process exited with status %d\n", WEXITSTATUS(status));
    } else {
        printf("Child process did not exit normally\n");
    }

    remove("file.txt");

    LI("func end")
    exit(EXIT_SUCCESS);
}

void parent_process(int child_pid, int count_nums, const char* pathname)
{
    LI("func start")
    int i, fd, num;
    char *buf;
    FILE *file = fopen(pathname, "w+");
    for (i = 0; i < count_nums; i++) {
        kill(child_pid, SIGUSR1); /* Блокирует доступ к файлу */
        num = rand();
        sprintf(buf, "%d", num);
        fputs(buf, file);
        kill(child_pid, SIGUSR2); /* Сообщает о том что записал */
        sleep(1);
        fgets(buf, BUFFSIZE, file);
        sscanf(buf, "%d", &num);
        printf("parent: num from child %d", num);
    }
    fclose(file);
    LI("func end")
}

void child_process(int count_nums, const char* pathname)
{
    LI("func start")
    int i, num, fd;
    char *buf[BUFFSIZE];
    FILE *file = fopen(pathname, "w+");
    for (i = 0; i < count_nums; i++) {
        fgets(buf, BUFFSIZE, file);
        sscanf(buf, "%d", &num);
        printf("child: num from parent");
        num = rand();
        memset(buf, 0, BUFFSIZE);
        sprintf(buf, "%d", num);
        fputs(buf, file);
    }
    fclose(file);
    LI("func end")
}

void sigusr1_handler(int sig) 
{
    LI("func start")
    kill(getpid(), SIGSTOP); 
    LI("func end")
} // Обработчик сигнала SIGUSR1

void sigusr2_handler(int sig) 
{ 
    LI("func start")
    kill(getpid(), SIGCONT); 
    LI("func end")
} // Обработчик сигнала SIGUSR2

void wait_var(int *variable, int value)
{
    LI("func start")
    while (*(variable) != value) {}
    LI("func end")
}

void error(const char *strerr)
{
    LI("func start")
    perror(strerr);
    LI("func end")
    exit(EXIT_FAILURE);
}