#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "1 argument max required\n");
		return 1;
	}
	printf("Expression : %s\n", argv[1]);
	return 0;
}
