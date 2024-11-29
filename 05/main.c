#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFSIZE 3

int flag = 1; // 1 - Процесс-потомок находится в ожидании, 0 - процесс потомок может открыть файл и прочитать данные

void sigusr1_handler(int sig) {flag = 1;}
void sigusr2_handler(int sig) {flag = 0;}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		perror("programm needs 1 argument - count of random numbers");
		exit(EXIT_FAILURE);
	}
	
	int n, count = 0;// n - Кол-во рандомных чисел, count - Счетчик сгенерированных/полученных чисел
	int pid;
	int fd_fifo; // Дескриптор для fifo
	ssize_t bytes_read = 0;
	char write_buff[BUFFSIZE], read_buff[BUFFSIZE]; // Буфферы для чтения и записи (разнести их по процессам?)
	char file[] = "/tmp/fifo0001.1";
	char i_data = 0; // index of data
	char* file_str;
	
	n = atoi(argv[1]);
	srand(time(NULL));
	
	unlink(file); // Удаляем файл, если он уже есть
	
	if (mkfifo(file, S_IRWXU | S_IRWXG | S_IRWXO) == -1) { // Создаём канал
		perror("can't create channel");
		exit(EXIT_FAILURE);
	}
	
	switch (pid = fork()) {
		case -1:
			perror("fork not work");
			exit(EXIT_FAILURE);
		case 0: // Кейс child'а
			printf("CHILD PID = %d\n", getpid());
			
			if((fd_fifo=open(file, O_WRONLY)) == -1) {
				perror("can't open channel to writie");
				exit(EXIT_FAILURE);
			}
			
			while(count != n) {
				snprintf(write_buff, BUFFSIZE, "%d\0", rand() % 99); // Генерим рандомное число и пишем в буфер
				write(fd_fifo, write_buff, BUFFSIZE);
				count++;
			}
			
			close(fd_fifo);
		default: // Кейс parent'а
			printf("PARENT PID = %d\n", getpid());
			
			signal(SIGUSR1, sigusr1_handler); // Блокируем в
			
			
			if((fd_fifo=open(file, O_RDONLY)) == -1) {
				perror("can't open channel to read");
				exit(EXIT_FAILURE);
			}
			
			while (count != n) {
				bytes_read = read(fd_fifo, read_buff, BUFFSIZE);
				write(STDOUT_FILENO, read_buff, bytes_read);
				write(STDOUT_FILENO, "\n", 1);
				fflush(stdout);
				count++;
			}
			close(fd_fifo);
	}
}
