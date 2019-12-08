#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char** argv) {
	int i, j, numbers[argc];
	for(i=1; i<argc; i++) {
		for(j=0; j<strlen(argv[i]); j++) {
			numbers[i] *= 10;
			numbers[i] += argv[i][j]-48;
		}
	}
	printf("%d %d\n", fibonacci(numbers[1]), sum_of_four_int(numbers[1], numbers[2], numbers[3], numbers[4]));
	return 0;
}
