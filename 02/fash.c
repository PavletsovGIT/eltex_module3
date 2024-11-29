#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_STR_SIZE 100
#define MAX_ARGS 100

void strsplit(char** dst, char* src, char* sep);
void w_strncpy(char* dst, char* src, int size);

int main(void) {
	int rv, i, nameSize;
	pid_t pid;
	char inputStr[MAX_STR_SIZE * 2];
	char appName[MAX_STR_SIZE];
	char* arguments[MAX_ARGS];
	
	printf("Welcome to FASH!!!\n\n");
	
	while (1) {
		printf("#: ");
		
		// Считываем строку до '\n' или до указанного размера
   		if (fgets(inputStr, sizeof(inputStr), stdin)) {
        // Убираем символ новой строки, если он есть
        	inputStr[strcspn(inputStr, "\n")] = '\0';
    	} else {
        	printf("Ошибка при считывании строки\n");
    	}
		
		nameSize = strcspn(inputStr, " ");
		strncpy(appName, inputStr, nameSize);
		
		strsplit(arguments, inputStr, " \0");
		
		switch(pid = fork()) {
			case -1:
				perror("fork");
				exit(EXIT_FAILURE);
			case 0:
				execv(appName, arguments);
				exit(EXIT_SUCCESS);
			default:
				wait(&rv);
		}
	}
	
	exit(EXIT_SUCCESS);
}

void w_strncpy(char* dst, char* src, int size) {
    strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
}

void strsplit(char** dst, char* src, char* sep) {
	if (strlen(src) == 0) {
		return;
	}

	char* ptr;
	int size_str = 0, size_input = 0, i = 0;
	
	size_input = strlen(src);
	ptr = src;
	while (*ptr) {
		size_str = strcspn(ptr, sep);
		dst[i] = (char*)malloc(sizeof(char) * (size_str));
		w_strncpy(dst[i], ptr, size_str + 1);
		ptr += size_str + 1;
		i++;
	}
	
	dst[i] = NULL;
}
