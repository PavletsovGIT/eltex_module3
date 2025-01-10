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

#include "logger.h"

#define SHARED_MEMORY_KEY 	"shared-memory-key"
#define SEM_PARENT_WORK_KEY "/tmp/sem-par-work-key"
#define SEM_CHILD_WORK_KEY 	"/tmp/sem-child-work-key"
#define PROJECT_ID			'a'
#define BUF_SIZE			256
#define MAX_INT_ARR_SIZE	10
#define MIN_INT_ARR_SIZE	2
#define MAX_DIGITS_IN_NUM	3
#define MAX_NUM 			999

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
key_t init_key(char *, int, int); /* Инициализация key_t */
void sem_op(int, int);
int sem_get(key_t key, int nsems, int semflg);
int shm_get(key_t key, int size, int shmflg);
void parent_process(char *, int, int);
void child_process(char *, int, int);
