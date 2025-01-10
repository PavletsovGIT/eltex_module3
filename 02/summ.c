#include <stdio.h>
#include <stdlib.h>

double summ(int n, int* arr) ;

int main(int argc, char* argv[]) {
	int* arr;
	int i, j, size = argc - 1;
	
	arr = (int*)malloc(sizeof(int) * size);
	
	for (i = 1, j = 0; i < argc; i++, j++) {
		arr[j] = atoi(argv[i]);
	}
	
	printf("%lf\n", summ(size, arr));
	
    exit(EXIT_SUCCESS);
}

double summ(int n, int* arr) {
	double result = 0;
	int i;
	
	for (i = 0; i < n; i++) {
		result += arr[i];
	}
	
	return result;
}
