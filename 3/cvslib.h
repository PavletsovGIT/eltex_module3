#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFSIZE 4096

// Глобальная (внутри файла) переменная, используемая для работы обёрток функций open, write, read, close
static int fd = -1;

// Инициализация переменной fd
void initfd(char* file, int flags);
void closefd();
// Write Record - делает запись в файле, не использует fsync!!!
void writercd(char* buff);
// Read Record - читает из файла запись с заданным id и возвращает её в виде строки
char* readrcd(int id);

