#include "lib12.h"

static int count_handles 	= 0; /* Количество пройденных циклов обработки данных */

void sigint_handler(int sign)
{
	LI("func start work")
	printf("PID: %d\tCount of handle: %d", getpid(), count_handles);
	LI("func end work")
	exit(EXIT_SUCCESS);
}

void error(char* msg) 
{
	LI("func start work")
	perror(msg);
	LI("func end work")
	exit(EXIT_FAILURE);

}

key_t init_key(char *pathname, int perms, int proj_id) 
{
	LI("func start work")
	int fd;
	key_t key;
	if ((fd = open(pathname, perms)) == -1) {
		error("open");
	}
	if ((key = ftok(pathname, proj_id)) == -1) {
		error("ftok");
	}
	close(fd);
	LI("func end work")
	return key;
}

void sem_op(int semid, int op) 
{
	LI("func start work")
    struct sembuf sb = {0, op, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
    LI("func end work")
}

int sem_get(key_t key, int nsems, int semflg) 
{
	LI("func start work")
	int sem;
	if ((sem = semget(key, nsems, semflg)) == -1) error("semget");
	return sem;
	LI("func end work")
}

int shm_get(key_t key, int size, int shmflg)
{
	LI("func start work")
	int shm_id;
	if ((shm_id = shmget(key, size, shmflg)) == -1) error("shmget");
	return shm_id;
	LI("func end work")
}

void init_arr_rand(int *arr, int count_nums) 
{
	LI("func start work")
	int i;
	for (i = 0; i < count_nums; i++) {
		arr[i] = rand() % MAX_NUM;
	}
	LI("func end work") 
}

void print_arr_to_buf(char *buf, int *arr, int count_nums)
{
	LI("func start work")
	int offset = 0, i;

	offset += sprintf(buf, "%d,", count_nums); /* Записываем кол-во чисел */

	for (i = 0; i < count_nums; i++) { /* Записываем сами числа */
		offset = sprintf(buf + offset, "%d,", arr[i]);
	}
	LI("func end work")
}

/* Return count of nums */
int scan_arr_from_buf(char *buf, int *arr)
{
	LI("func start work")
	int offset = 0, 
		count_nums, i;

	/* Получаем кол-во чисел */
	sscanf(buf, "%d,", &count_nums);

	for (i = 0; i < count_nums; i++) {
		offset =+ sscanf(buf + offset, "%d,", &arr[i]);
	}
	LI("func end work")
	return count_nums;
}

void clean_buf(char* buf, size_t size)
{
	LI("func start work")
	memset(buf, 0, size);
	LI("func end work")
}

int find_max(int *arr, int count_nums) 
{
	LI("func start work")
	int max = INT_MIN, i;

	for (i = 0; i < count_nums; i++) {
		if (arr[i] > max) max = arr[i];
	}
	LI("func end work")
	return max;
}

int find_min(int *arr, int count_nums) 
{
	LI("func start work")
	int min = INT_MAX, i;

	for (i = 0; i < count_nums; i++) {
		if (arr[i] > min) min = arr[i];
	}
	LI("func end work")
	return min;
}

void parent_process(char *shm_buf, int sem_par_w, int sem_child_w)
{
	LI("func start work")
	int count_nums;
	int arr[MAX_INT_ARR_SIZE];
	
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
	LI("func end work")
}

void child_process(char *shm_buf, int sem_par_w, int sem_child_w)
{
	LI("func start work")
	int count_nums;
	int arr[MAX_INT_ARR_SIZE];

	while (1) {
		sem_op(sem_child_w, -1);

		count_nums = scan_arr_from_buf(shm_buf, arr);
		clean_buf(shm_buf, BUF_SIZE);

		arr[0] = find_max(arr, count_nums);
		arr[1] = find_min(arr, count_nums);

		print_arr_to_buf(shm_buf, arr, 2); /* Нужно записать только первые два элемента */

		sem_op(sem_par_w, 1); /* Дальше работает родительский процесс */
	}
	LI("func end work")
}