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
	
	// передвать аргументы из argv
	
	switch(pid = fork()) {
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);
	case 0:
		printf("CHILD: PID - %d\n", getpid());
		printf("CHILD: PPID - %d\n", getppid());
		execv("compute", argv[1]);
		exit(EXIT_SUCCESS);

	default:
		printf("PARENT: PID - %d\n", getpid());
		printf("PARENT: CHILD PID - %d\n", pid);
		execv("compute", argv[par_size]);
		wait(&rv);
		printf("PARENT: RETURN STATUS FOR CHILD - %d\n", WEXITSTATUS(rv));

	}
	exit(EXIT_SUCCESS);
}
