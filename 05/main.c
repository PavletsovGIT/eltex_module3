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

#define BUFFSIZE 20  // Увеличим размер буфера
#define TIME_TO_SLEEP 1

int flag = 1; // 1 - Процесс-потомок находится в ожидании, 0 - процесс потомок может открыть файл и прочитать данные

void sigusr1_handler(int sig) { flag = 1; } // Обработчик сигнала SIGUSR1
void sigusr2_handler(int sig) { flag = 0; } // Обработчик сигнала SIGUSR2

int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("Program needs 1 argument - count of random numbers");
        exit(EXIT_FAILURE);
    }

    int n, count = 0, rand_num;  // n - Кол-во рандомных чисел, count - Счетчик сгенерированных/полученных чисел
    int pid;
    int fd_channel, fd_file;  // Дескриптор для fifo
    ssize_t bytes_read = 0;
    char write_buff[BUFFSIZE], read_buff[BUFFSIZE];  // Буфферы для чтения и записи
    char channel[] = "/tmp/fifo0001.1";
    char file[] = "file.txt";

    n = atoi(argv[1]);
    srand(time(NULL));

    unlink(channel); // Удаляем файл, если он уже есть

    if (mkfifo(channel, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {  // Создаём канал
        perror("Can't create channel");
        exit(EXIT_FAILURE);
    }

    switch (pid = fork()) {
        case -1:
            perror("fork failed");
            exit(EXIT_FAILURE);
        case 0: // Дочерний процесс
            printf("CHILD PID = %d\n", getpid());

            // Назначаем обработчики сигналов
            signal(SIGUSR1, sigusr1_handler);  // Блокировка доступа
            signal(SIGUSR2, sigusr2_handler);  // Разрешение доступа

            while (count < n) {
                while (flag) {  // Ждём SIGUSR2 для того, чтобы начать читать файл
                    sleep(TIME_TO_SLEEP);  // Даём время родителю записать данные
                }
                flag = 1;  // Возвращаем флаг в состояние ожидания

                if ((fd_file = open(file, O_RDONLY)) == -1) {  // Открываем файл для чтения
                    perror("CHILD: Can't open file to read");
                    exit(EXIT_FAILURE);
                }
                bytes_read = read(fd_file, read_buff, BUFFSIZE);  // Считываем данные из файла
                close(fd_file);

                if (bytes_read > 0) {
                    printf("PID: %d : message = %s\n", getpid(), read_buff);  // Выводим данные из файла
                }

                rand_num = rand() % 99;  // Генерируем случайное число
                if ((fd_channel = open(channel, O_WRONLY)) == -1) {  // Открываем канал для записи
                    perror("CHILD: Can't open channel to write");
                    exit(EXIT_FAILURE);
                }
                write(fd_channel, &rand_num, sizeof(int));  // Отправляем случайное число родителю
                close(fd_channel);

                count++;
            }
            exit(EXIT_SUCCESS); // Завершаем дочерний процесс
            break;

        default: // Родительский процесс
            printf("PARENT: CHILD PID = %d\n", pid);
            printf("PARENT PID = %d\n", getpid());

            while (count < n) { //НАДО ВЫЗЫВАТЬ SIGUSR1
                snprintf(write_buff, BUFFSIZE, "data%d", count);  // Формируем строку для записи в файл
                if ((fd_file = open(file, O_WRONLY | O_TRUNC)) == -1) {  // Открываем файл для записи
                    perror("PARENT: Can't open file to write");
                    exit(EXIT_FAILURE);
                }
                write(fd_file, write_buff, strlen(write_buff));  // Записываем данные в файл
                close(fd_file);

                kill(pid, SIGUSR2);  // Отправляем сигнал дочернему процессу, что файл можно читать

                sleep(TIME_TO_SLEEP);  // Даём время дочернему процессу прочитать файл

                if ((fd_channel = open(channel, O_RDONLY)) == -1) {  // Открываем канал для чтения
                    perror("PARENT: Can't open channel to read");
                    exit(EXIT_FAILURE);
                }
                read(fd_channel, &rand_num, sizeof(int));  // Считываем случайное число
                printf("PID: %d : RAND NUM = %d\n", getpid(), rand_num);  // Выводим случайное число
                close(fd_channel);

                kill(pid, SIGUSR1);  // Отправляем сигнал дочернему процессу, что родитель снова блокирует доступ

                count++;
            }

            // Ждём завершения дочернего процесса
            int status;
            waitpid(pid, &status, 0); // Родитель ожидает завершения дочернего процесса

            if (WIFEXITED(status)) {
                printf("Child process exited with status %d\n", WEXITSTATUS(status));
            } else {
                printf("Child process did not exit normally\n");
            }
            break;
    }
    exit(EXIT_SUCCESS);
}
