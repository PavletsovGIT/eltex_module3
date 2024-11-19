#include "cvslib.h"

// Инициализация переменной fd
void initfd(char* file, int flags) {
	fd = open(file, flags);
	
	if (fd == -1) {
		perror("can't open file");
		exit(EXIT_FAILURE);
	}
}

void closefd() {
	close(fd);
}

// Write Record - делает запись в файле, не использует fsync!!!
void writercd(char* buff) {
	write(fd, buff, strlen(buff));
}

// Read Record - читает из файла запись с заданным id и возвращает её в виде строки
char* readrcd(int id) {
	char* result, ptr;
	char* buff;
	ssize_t bytes_read;
	
	ptr = result;
	
	while ((bytes_read = read(fd, buff, sizeof(buff))) > 0) {
		// Выделяем память в result
		result = (char*)realloc(result, sizeof(char) * bytes_read);
		// Копируем содержимое из buff в result через ptr
		strncpy(ptr, buff, bytes_read);
		// Перемещаем ptr
		ptr += bytes_read;
    }
    
    return result;
}
