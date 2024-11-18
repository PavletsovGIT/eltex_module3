#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]) {
	if (argc == 1) {
		perror("Not enought arguments...\n");
		exit(EXIT_FAILURE);
	}
	
	pid_t pid;
	int rv, i, j;
	char** par_arr;
	char** child_arr;
	
	size_t par_size = argc / 2 + 1;
	size_t child_arr_size = argc - par_size + 1;
	par_arr = (char**)malloc(sizeof(char*) * par_size);
	child_arr = (char**)malloc(sizeof(char*) * child_arr_size);
	
	// Распределение аргументов между массивами
	// Для par_arr
	for (i = 1, j = 0; i < par_size; i++, j++) {
		// Копируем значение
		par_arr[j] = argv[i];
		//strncpy(par_arr[i], argv[i], strlen(argv[i]));
	}
	par_arr[par_size - 1] = NULL;
	
	// Для child_arr
	for (i = child_arr_size + 1, j = 0; i < argc; i++, j++) {
		// Копируем значение
		child_arr[j] = argv[i];
		//strncpy(child_arr[i - child_arr_size], argv[i], strlen(argv[i]));
	}
	child_arr[child_arr_size - 1] = NULL;
	
	switch(pid = fork()) {
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);
	case 0:
		printf("CHILD: PID - %d\n", getpid());
		printf("CHILD: PPID - %d\n", getppid());
		execv("compute", child_arr);
		exit(EXIT_SUCCESS);

	default:
		printf("PARENT: PID - %d\n", getpid());
		printf("PARENT: CHILD PID - %d\n", pid);
		execv("compute", par_arr);
		wait(&rv);
		printf("PARENT: RETURN STATUS FOR CHILD - %d\n", WEXITSTATUS(rv));

	}
	exit(EXIT_SUCCESS);
}
