CC = gcc
CC_FLAGS = -Wall -O0 -std=c11

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *merged*
	rm -f *test*
	rm -f *output*

compile3: clean
	$(CC) $(CC_FLAGS) zad_3_1.c -o output

run3:
	./output