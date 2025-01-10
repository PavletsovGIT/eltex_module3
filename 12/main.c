#include "lib12.h"

int main(void) 
{
	freopen("file.log", "w+", stderr);
	LI("func start work")
	int pid, 
		status, 
		fd_shm, 
		shm_id,
		shm_flg = 0660 | IPC_CREAT;
	key_t s_key;
	char shared_buf[BUF_SIZE];
	char *shared_buf_ptr;
	int sem_par_w, sem_child_w; /* Semaphores parent work and child work */

	srand(time(NULL));
	signal(SIGINT, sigint_handler); /* Назначение обработчика сигнала SIGINT */

	/* Создвём и инициализируем файлы для работы программмы */
	/* Shared memory */
	s_key = init_key(SHARED_MEMORY_KEY, 0660 | O_CREAT, PROJECT_ID);
	shm_id = shm_get(s_key, BUF_SIZE, shm_flg);

	/* Semaphore Parent Work */
	s_key = init_key(SEM_PARENT_WORK_KEY, 0660 | O_CREAT, PROJECT_ID);
	sem_par_w = sem_get(s_key, 1, 0660);
	/**
	 * Изначально разблокирован, 
	 * потому что первый работает родительский процесс
	 */
	semctl(sem_par_w, 0, SETVAL, 1); 

	/* Semaphore Child Work */
	s_key = init_key(SEM_CHILD_WORK_KEY, 0660 | O_CREAT, PROJECT_ID);
	sem_child_w = sem_get(s_key, 1, 0660);
	semctl(sem_child_w, 0, SETVAL, 0);

	switch (pid = fork()) { /* Создаём дочерний процесс */
		case -1:
			error("fork");
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

    LI("func end work")
    return 0;
}