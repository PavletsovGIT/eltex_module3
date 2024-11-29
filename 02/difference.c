#include <stdio.h>
#include <stdlib.h>

double difference(int n, int* arr) ;

int main(int argc, char* argv[]) {
	if (argc == 1) {
		perror("defference needs 1 or more arguments");
		exit(EXIT_FAILURE);
	}

	int* arr;
	int i, j;
	
	arr = (int*)malloc(sizeof(int) * argc - 1);
	
	for (i = 1, j = 0; i < argc; i++, j++) {
		arr[j] = atoi(argv[i]);
	}
	
	printf("%lf\n", difference(argc - 1, arr));
	
    exit(EXIT_SUCCESS);
}

double difference(int n, int* arr) {
	double result = arr[0];
	int i;
	
	for (i = 1; i < n; i++) {
		result -= arr[i];
	}
	
	return result;
}
