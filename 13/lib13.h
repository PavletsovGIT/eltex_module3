#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include "logger.h"

#define SHM_SIZE 			1024
#define PATHNAME 			"shared-memory-13"
#define SEM_PARENT_WORK_KEY "/tmp/sem-par-work-key"
#define SEM_CHILD_WORK_KEY 	"/tmp/sem-child-work-key"
#define PROJECT_ID			'a'
#define MAX_INT_ARR_SIZE	10
#define MIN_INT_ARR_SIZE	2
#define MAX_DIGITS_IN_NUM	3
#define MAX_NUM 			999

void error(char * str);
void child_process(int sem_par, int sem_chi, void *shm_ptr);
void parent_process(int sem_par, int sem_chi, void *shm_ptr);
void sigint_handler(int); /* Обработчик сигнала SIGINT */

void sem_op(int semid, int op);
void init_arr_rand(int *arr, int count_nums);
void print_arr_to_buf(void *buf, int *arr, int count_nums);
int scan_arr_from_buf(void *buf, int *arr);
void clean_buf(char* buf, size_t size);
int find_max(int *arr, int count_nums);
int find_min(int *arr, int count_nums);
