//build:gcc -Wall -Wextra -Werror -pedantic -o str2bf $SRC
#include <stdio.h>

int main(int argc, char **argv) {
	char ch;

	if (argc != 1) {
		fprintf(stderr, "%s", argv[0]);
		return 1;
	}

	while ((ch = getchar()) != EOF) {
		char ch2;

		// Build up the character and print it
		ch2 = ch;
		while (ch2) {
			printf("+");
			ch2--;
		}
		printf(".");

		// Set cell back to zero for next character
		ch2 = ch;
		while (ch2) {
			printf("-");
			ch2--;
		}

		argv[1]++;
	}
	return 1;
}
