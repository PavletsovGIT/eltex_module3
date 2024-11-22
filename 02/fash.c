#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_STR_SIZE 100
#define MAX_ARGS 100

void get_name_args(char* input, char** name, char*** arguments);

int main(void) {

	int rv, i;
	pid_t pid;
	char inputStr[MAX_STR_SIZE * 2];
	char* appName;
	char** arguments;
	
	printf("Welcome to FASH!!!\n\n");
	
	while (1) {
		printf("#: ");
		
		// Считываем строку до '\n' или до указанного размера
   		if (fgets(inputStr, sizeof(inputStr), stdin)) {
        // Убираем символ новой строки, если он есть
        	inputStr[strcspn(inputStr, "\n")] = '\0';
        
        	printf("Ваша строка: %s\n", inputStr);
    	} else {
        	printf("Ошибка при считывании строки\n");
    	}
		
		get_name_args(inputStr, &appName, &arguments);
		
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

void get_name_args(char* input, char** name, char*** arguments) {
    char sep[] = " \0";
    char* ptr = input;
    int size, args_size = 0;
    
    // Выделяем память для массива указателей на аргументы
    *arguments = (char**)malloc(sizeof(char*) * 10); // Начальный размер массива, можно увеличить при необходимости
    
    // Инициализация, выделение памяти для первого аргумента
    ptr = strtok(input, sep);
    
    // Выделяем память для name
    size = strcspn(ptr, sep) + 1;
    *name = (char*)malloc(sizeof(char) * size); 
    strncpy(*name, ptr, size - 1);
    (*name)[size - 1] = '\0';
    
    // Переходим к остальным аргументам
    ptr = strtok(NULL, sep);
    
    while (ptr != NULL) {
        // Увеличиваем размер массива аргументов
        args_size++;
        *arguments = (char**)realloc(*arguments, sizeof(char*) * (args_size + 1)); // Увеличиваем размер массива
        
        size = strcspn(ptr, sep) + 1;
        (*arguments)[args_size - 1] = (char*)malloc(sizeof(char) * size); // Выделяем память для нового аргумента
        
        // Копируем строку в новый элемент массива
        strncpy((*arguments)[args_size - 1], ptr, size - 1);
        (*arguments)[args_size - 1][size - 1] = '\0';
        
        // Переходим к следующему аргументу
        ptr = strtok(NULL, sep);
    }
}
