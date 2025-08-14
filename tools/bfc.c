//build:gcc -Wall -Wextra -Werror -pedantic -o bfc $SRC
//dev:gcc -Wall -Wextra -Werror -pedantic -fsanitize=address -o bfc $SRC
#include <stdlib.h>
#include <stdio.h>

#define MEM_LEN 4096

/*
 * Register with special uses
 * %rsp - Pointer to a one character buffer used for printing
 * %rbx - Pointer to memory buffer of length MEM_LEN
 */

int main(int argc, char **argv) {
	char ch;
	unsigned loop_stack[1024];
	unsigned loop_stack_size = 0;
	unsigned loop_counter = 0;

	if (argc != 1) {
		fprintf(stderr, "%s < infile.bf > outfile.S\n", argv[0]);
		return 1;
	}

	// Boilerplate
	printf("//build:as $SRC -o out.o && gcc -o out out.o -nostdlib -static\n"); // ec support
	printf(".section __TEXT,__text\n");
	printf(".global start\n");
	printf("start:\n");

	// Allocate memory
	printf("pushq %%rbp\n");
	printf("movq %%rsp, %%rbp\n");
	printf("subq $%d, %%rsp\n", MEM_LEN + 1);
	printf("leaq 1(%%rsp), %%rbx\n"); // rbx is the memory pointer
	// clear the memory
	printf("movq %%rbx, %%rdi\n");
	printf("xorq %%rax, %%rax\n");
	printf("movq $%d, %%rcx\n", MEM_LEN);
	printf("rep stosb\n");

	// Interpreter loop
	while ((ch = getchar()) != EOF) {
		switch (ch) {
			case '>':
				printf("incq %%rbx\n");
				break;
			case '<':
				printf("decq %%rbx\n");
				break;
			case '+':
				printf("incq (%%rbx)\n");
				break;
			case '-':
				printf("decq (%%rbx)\n");
				break;
			case '.':
				printf("movb (%%rbx), %%cl\n"); // char tmp = *memptr
				printf("movb %%cl, (%%rsp)\n"); // *buf = tmp // note: buf is char[1]
				printf("movq $0x2000004, %%rax\n"); // write(
				printf("movq $1, %%rdi\n"); // stdout,
				printf("movq %%rsp, %%rsi\n"); // buf,
				printf("movq $1, %%rdx\n"); // length=1)
				printf("syscall\n");
				break;
			case ',':
				printf("movq $0x2000003, %%rax\n"); // read(
				printf("movq $0, %%rdi\n"); // stdin,
				printf("movq %%rbx, %%rsi\n"); // memptr
				printf("movq $1, %%rdx\n"); // length=1)
				printf("syscall\n");
				// if read returns (in %eax) 0 aka EOF, then put 0 in the cell
				printf("mulb (%%rbx)\n");
				printf("movq %%rax, (%%rbx)\n");
				break;
			case '[':
				loop_stack[loop_stack_size++] = loop_counter++;
				printf("loop%u:\n", loop_stack[loop_stack_size - 1]);
				printf("cmpb $0, (%%rbx)\n");
				printf("je loop%u_end\n", loop_stack[loop_stack_size - 1]);
				break;
			case ']':
				printf("jmp loop%u\n", loop_stack[loop_stack_size - 1]);
				printf("loop%u_end:\n", loop_stack[loop_stack_size - 1]);
				loop_stack_size--;
				break;
		}
	}

	// epilogue and exit
	printf("addq $%d, %%rsp\n", MEM_LEN + 1);
	printf("popq %%rbp\n");
	printf("movq $0x2000001, %%rax\n");
	printf("movq $0, %%rdi\n");
	printf("syscall\n");
	return 0;
}
