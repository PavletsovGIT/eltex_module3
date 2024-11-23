#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_SIZE 256

int main(void) {
	char* filename = "help.txt";
	
	char buffer[BUF_SIZE];
	
	FILE* fp = fopen(filename, "r");
	if (!fp) {
		perror("Can't open file");
		exit(EXIT_FAILURE);
	}
	
	while (fgets(buffer, 256, fp) != NULL) {
		printf("%s", buffer);
	}
	
	fclose(fp);
	
	exit(EXIT_SUCCESS);
}
