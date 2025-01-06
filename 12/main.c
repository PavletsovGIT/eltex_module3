#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>

void sigint_handler(int); /* Обработчик сигнала SIGINT */
void error(char* msg);
/**
 * Инициализирует переменную типа key_t
 * Параметры:
 * 1. key_t * - указатель на ключ
 * 2. char * - pathname
 * 3. int - permissions
 * 4. int - project id
 */
void init_key(key_t *, char *, int, int); /* Инициализация key_t */
void sem_op(int, int);
void parent_process(char *, int, int);
void child_process(char *, int, int);

#define SHARED_MEMORY_KEY 	"/tmp/shared-memory-key"
#define SEM_PARENT_WORK_KEY "/tmp/sem-par-work-key"
#define SEM_CHILD_WORK_KEY 	"/tmp/sem-child-work-key"
#define PROJECT_ID			'a'
#define BUF_SIZE			256
#define MAX_INT_ARR_SIZE	10
#define MIN_INT_ARR_SIZE	2
#define MAX_DIGITS_IN_NUM	3
#define MAX_NUM 			999

static int count_handles 	= 0; /* Количество пройденных циклов обработки данных */

struct shared_memory {
    char buf[BUF_SIZE];
    int buffer_index;
    int buffer_print_index;
};

int main(void) 
{
	int pid, 
		status, 
		fd_shm, 
		shm_id,
		shm_flg;
	key_t s_key;
	char shared_buf[BUF_SIZE];
	char *shared_buf_ptr;
	int sem_par_w, sem_child_w; /* Semaphores parent work and child work */

	srand(time(NULL));
	signal(SIGINT, sigint_handler); /* Назначение обработчика сигнала SIGINT */

	/* Создвём и инициализируем файлы для работы программмы */
	/* Shared memory */
	if ((s_key = ftok(SHARED_MEMORY_KEY, PROJECT_ID)) == -1) {
		error("ftok");
	}
	if ((shm_id = shmget(s_key, BUF_SIZE, shm_flg)) == -1) {
		error("shmget");
	}

	/* Semaphore Parent Work */
	init_key(&s_key, SEM_PARENT_WORK_KEY, 0660 | O_CREAT, PROJECT_ID);
	if ((sem_par_w = semget(s_key, 1, 0660 | IPC_CREAT)) == -1) {
		error("semget");
	}
	/**
	 * Изначально раб=зблокирован, 
	 * потому что первый работает родительский процесс
	 */
	semctl(sem_par_w, 0, SETVAL, 1); 

	/* Semaphore Child Work */
	init_key(&s_key, SEM_CHILD_WORK_KEY, 0660 | O_CREAT, PROJECT_ID);
	if ((sem_child_w = semget(s_key, 1, 0660 | IPC_CREAT)) == -1) {
		error("semget");
	}
	semctl(sem_child_w, 0, SETVAL, 0);

	/**
	 * Что нужно доделать:
	 * 1. Дописать инициализацию семафоров и shared memory + 
	 * 2. Написать работу процесссов.
	 */

	switch (pid = fork()) { /* Создаём дочерний процесс */
		case -1:
			error("ftok");
		case 0:
			child_process(shared_buf_ptr, sem_par_w, sem_child_w);
			exit(EXIT_SUCCESS);
		default:
			break;
	}

	parent_process(shared_buf_ptr, sem_par_w, sem_child_w);

	/* Ожидаем окончание доченрнего процесса */
	waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Child process exited with status %d\n", WEXITSTATUS(status));
    } else {
        printf("Child process did not exit normally\n");
    }

    /* Убираем за собой */
    remove(SHARED_MEMORY_KEY);
    remove(SEM_PARENT_WORK_KEY);
    remove(SEM_CHILD_WORK_KEY);

    return 0;
}

void sigint_handler(int sign)
{
	printf("PID: %d\tCount of handle: %d", getpid(), count_handles);
	exit(EXIT_SUCCESS);
}

void error(char* msg) 
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void init_key(key_t *key_p, char *pathname, int perms, int proj_id) 
{
	int fd;
	if ((fd = open(pathname, perms)) == -1) {
		error("open");
	}
	if ((*(key_p) = ftok(pathname, proj_id)) == -1) {
		error("ftok");
	}
	// close(fd);
}

void sem_op(int semid, int op) {
    struct sembuf sb = {0, op, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

void init_arr_rand(int *arr, int count_nums) 
{
	int i;
	for (i = 0; i < count_nums; i++) {
		arr[i] = rand() % MAX_NUM;
	}
}

void print_arr_to_buf(char *buf, int *arr, int count_nums)
{
	int offset = 0, i;
	char *buf_p;

	offset += sprintf(buf, "%d,", count_nums); /* Записываем кол-во чисел */

	for (i = 0; i < count_nums; i++) { /* Записываем сами числа */
		offset = sprintf(buf + offset, "%d,", arr[i]);
	}
}

/* Return count of nums */
int scan_arr_from_buf(char *buf, int *arr)
{
	int offset = 0, 
		count_nums, i;

	/* Получаем кол-во чисел */
	sscanf(buf, "%d,", &count_nums);

	for (i = 0; i < count_nums; i++) {
		offset = sscanf(buf + offset, "%d,", arr[i]);
	}

	return count_nums;
}

void parent_process(char *shm_buf, int sem_par_w, int sem_child_w)
{
	int count_nums, i;
	int arr[MAX_INT_ARR_SIZE];
	int offset = 0; /* pointer to buffer and offset is using to write into sharead memory */
	char *buf_p;

	while (1) {
		sem_op(sem_par_w, -1); /* Блокируем семафор для работы родительского процесса */

		/* count_nums э [MIN_INT_ARR_SIZE, MAX_INT_ARR_SIZE] */
		count_nums = rand() % MAX_INT_ARR_SIZE + MIN_INT_ARR_SIZE; 
		init_arr_rand(arr, count_nums);
		/** Примечание
		 * Формат записи в буфер:
		 * "%d,%d,...,", <кол-во элементов в массиве>, <элементы массива> ...
		 */
		print_arr_to_buf(shm_buf, arr, count_nums);

		sem_op(sem_child_w, 1); /* Даём поработать дочернему процессу */
		sem_op(sem_par_w, -1); /* Ждём когда дочерний процесс разблокирует sem_par_w */

		scan_arr_from_buf(shm_buf, arr); /* Извлекаем максимальное и минимальное числа из общей памяти */
		clean_buf(shm_buf, BUF_SIZE);

		printf("Max num: %d\nMin num: %d\n", arr[0], arr[1]); /* Выводим результаты */

		count_handles++; /* Увеличиваем счетчик циклов обработки */
		sem_op(sem_par_w, 1); /* Разблокируем семафор */
	}
}

void clean_buf(char* buf, size_t size)
{
	memset(buf, 0, size);
}

int find_max(int *arr, int count_nums) {
	int max = INT_MIN, i;

	for (i = 0; i < count_nums; i++) {
		if (arr[i] > max) max = arr[i];
	}

	return max;
}

int find_min(int *arr, int count_nums) {
	int min = INT_MAX, i;

	for (i = 0; i < count_nums; i++) {
		if (arr[i] > min) min = arr[i];
	}

	return min;
}

void child_process(char *shm_buf, int sem_par_w, int sem_child_w)
{
	int count_nums, i;
	int arr[MAX_INT_ARR_SIZE];
	int offset = 0;
	char *buf_p;

	while (1) {
		sem_op(sem_child_w, -1);

		count_nums = scan_arr_from_buf(shm_buf, arr);
		clean_buf(shm_buf, BUF_SIZE);

		arr[0] = find_max(arr, count_nums);
		arr[1] = find_min(arr, count_nums);

		print_arr_to_buf(shm_buf, arr, 2); /* Нужно записать только первые два элемента */

		sem_op(sem_par_w, 1); /* Дальше работает родительский процесс */
	}
}