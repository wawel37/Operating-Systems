CC = gcc
CCFLAGS = -Wall -std=c11

static:
	$(CC) $(CC_FLAGS) -c lib.c -o lib.o
	ar rcs lib.a lib.o

clean:
	rm -f *test*

compile2: clean static
	$(CC) $(CCFLAGS) zad2.c -static -L. lib.a -o output