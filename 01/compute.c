#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	// Вычислять то нечего
	if (argc == 1) {
		perror("No argumets...\n");
		exit(EXIT_FAILURE);
	}
	
	int i, n;
	
	// Вычисляем площади квадратов
	for (i = 1; i < argc; i++) {
		n = atoi(argv[i]);
		printf("PID: %d : N = %d : S = %d\n", getpid(), n, n*n);
	}
	exit(EXIT_SUCCESS);
}
