# bf.bf
Self-hosted BF compiler for macOS.

This program reads BF source code from standard input and writes an
assembler script to standard output. The compiler was boostrapped with
tools/bfc and all code that prints strings was generated with
tools/str2bf.

The generated assembler script includes an ec header, which is a short
comment telling the ec program how to compile the script. ec can be
found at https://github.com/michaelfm1211/ec

To bootstrap this compiler, run the following:
```
# Build the stage1 compiler tools/bfc
cd tools
gcc -Wall -Wextra -Werror -pedantic -o bfc bfc.c
cd ..

# Build the stage2 compiler
tools/bfc < bf.bf > bf-stage2.S
as -o bf-stage2.o bf-stage2.S
gcc -nostdlib -static -o bf-stage2 bf-stage2.o

# Build the stage3 self-hosted compiler
./bf-stage2 < bf.bf > bf.S  # this might take a bit, be patient
as -o bf.o bf.S
gcc -nostdlib -static -o bf bf.o
```

You can also run some of the example programs in tests/
```
./bf < tests/hello.bf > hello.S
as -o hello.o hello.S
gcc -nostdlib -static -o hello hello.o
./hello
# Hello World!
```
