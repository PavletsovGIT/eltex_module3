#include <stdio.h>
#include <stdlib.h>

double summ(int n, int* arr) ;

int main(int argc, char* argv[]) {
	int* arr;
	int i, j;
	
	arr = (int*)malloc(sizeof(int) * argc - 1);
	
	for (i = 1, j = 0; i < argc; i++, j++) {
		arr[j] = atoi(argv[i]);
	}
	
	printf("Summ = %lf\n", summ(argc - 1, arr));
	
    exit(EXIT_SUCCESS);
}

double summ(int n, int* arr) {
	double result = 0;
	int i;
	
	for (i = 0; i < n; i++) {
		printf("Summ id.%d : %d\n", i, arr[i]);
		result += arr[i];
	}
	
	return result;
}
