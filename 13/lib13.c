#include "lib13.h"

static int count_handles = 0;

void sigint_handler(int sign)
{
	printf("PID: %d\tCount of handle: %d", getpid(), count_handles);
	exit(EXIT_SUCCESS);
}

void error(char *str)
{
	LI("func start work")
	perror(str);
	exit(EXIT_FAILURE);
	LI("func end work")
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

void init_arr_rand(int *arr, int count_nums) 
{
	LI("func start work")
	int i;
	for (i = 0; i < count_nums; i++) {
		arr[i] = rand() % MAX_NUM;
	}
	LI("func end work")
}

void print_arr_to_buf(void *buf, int *arr, int count_nums)
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
int scan_arr_from_buf(void *buf, int *arr)
{
	LI("func start work")
	int offset = 0, 
		count_nums, i;

	/* Получаем кол-во чисел */
	sscanf(buf, "%d,", &count_nums);

	LI("getted count_nums")

	for (i = 0; i < count_nums; i++) {
		LI("getted num")
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

void parent_process(int sem_par_w, int sem_child_w, void *shm_ptr)
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
		print_arr_to_buf(shm_ptr, arr, count_nums);

		sem_op(sem_child_w, 1); /* Даём поработать дочернему процессу */
		sem_op(sem_par_w, -1); /* Ждём когда дочерний процесс разблокирует sem_par_w */

		scan_arr_from_buf(shm_ptr, arr); /* Извлекаем максимальное и минимальное числа из общей памяти */
		clean_buf(shm_ptr, SHM_SIZE);

		printf("i = %d\t{Min num: %d;\tMin num: %d}\n", count_handles, arr[0], arr[1]); /* Выводим результаты */

		count_handles++; /* Увеличиваем счетчик циклов обработки */
		sem_op(sem_par_w, 1); /* Разблокируем семафор */
	}
	LI("func end work")
}

void child_process(int sem_par_w, int sem_child_w, void *shm_ptr)
{
	LI("func start work")

	int count_nums;
	int arr[MAX_INT_ARR_SIZE];

	while (1) {
		sem_op(sem_child_w, -1);

		count_nums = scan_arr_from_buf(shm_ptr, arr);
		clean_buf(shm_ptr, SHM_SIZE);

		arr[0] = find_max(arr, count_nums);
		arr[1] = find_min(arr, count_nums);

		print_arr_to_buf(shm_ptr, arr, 2); /* Нужно записать только первые два элемента */

		sem_op(sem_par_w, 1); /* Дальше работает родительский процесс */
	}
	LI("func end work")
}