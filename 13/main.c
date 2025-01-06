#include <stdio.h>
#include "lib13.h"

int main()
{
	if (freopen("file.log", "w", stderr) == NULL) {
      	error("freopen");
    }
	LI("func start work")

	int shm_fd,
		fd,
		flag 	= O_CREAT | O_RDWR,
		pid,
		status;
	mode_t mode = S_IRWXU;
	void *shm_ptr;
	int sem_par_w, sem_child_w; /* Semaphores parent work and child work */
	key_t key;
	//const char *name = "shared-memory-13";

	signal(SIGINT, sigint_handler);

	if ((shm_fd = shm_open(PATHNAME, flag, mode)) == -1) error("shm_open");
	ftruncate(shm_fd, SHM_SIZE);
	shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

	if ((fd = open(SEM_PARENT_WORK_KEY, O_CREAT, 0660)) == -1)  {
		error("open");
	}
	close(fd);

	if ((fd = open(SEM_CHILD_WORK_KEY, O_CREAT, 0660)) == -1)  {
		error("open");
	}
	close(fd);

	if ((key = ftok(SEM_PARENT_WORK_KEY, PROJECT_ID)) == -1) {
		error("ftok");
	}
	if ((sem_par_w = semget(key, 1, 0660 | IPC_CREAT)) == -1) {
		error("semget");
	}
	semctl(sem_par_w, 0, SETVAL, 1);

	if ((key = ftok(SEM_CHILD_WORK_KEY, PROJECT_ID)) == -1) {
		error("ftok");
	}
	if ((sem_child_w = semget(key, 1, 0660 | IPC_CREAT)) == -1) {
		error("semget");
	}
	semctl(sem_child_w, 0, SETVAL, 0);

	switch (pid = fork()) { /* Создаём дочерний процесс */
		case -1:
			error("ftok");
		case 0:
			child_process(sem_par_w, sem_child_w, shm_ptr);
			exit(EXIT_SUCCESS);
		default:
			break;
	}

	parent_process(sem_par_w, sem_child_w, shm_ptr);

	/* Ожидаем окончание доченрнего процесса */
	waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Child process exited with status %d\n", WEXITSTATUS(status));
    } else {
        printf("Child process did not exit normally\n");
    }

    /* Убираем за собой */
    remove(PATHNAME);
    remove(SEM_PARENT_WORK_KEY);
    remove(SEM_CHILD_WORK_KEY);

    LI("func end work")
    return 0;
}