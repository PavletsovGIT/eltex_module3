#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	if (argc == 1) {
		perror("strcompare needs 1 or more arguments");
		exit(EXIT_FAILURE);
	}
	
	int i, arg_size, full_size = 0;
	char* cmb_str;
	char* ptr; //= cmb_str;
	
	arg_size = strlen(argv[1]);
	full_size += arg_size;
	cmb_str = (char*)malloc(sizeof(char) * arg_size);
	ptr = cmb_str;
	strncpy(cmb_str, argv[1], arg_size);
	ptr += arg_size;
	
	for (i = 2; i < argc; i++) {
		arg_size = strlen(argv[i]);
		full_size += arg_size;
		cmb_str = (char*)realloc(cmb_str, sizeof(char) * full_size);
		strncpy(ptr, argv[i], arg_size);
		ptr += arg_size;
	}
	
	full_size++;
	cmb_str = (char*)realloc(cmb_str, sizeof(char) * full_size);
	cmb_str[full_size - 1] = '\0';	
	
	printf("%s\n", cmb_str);
	
	exit(EXIT_SUCCESS);
}
