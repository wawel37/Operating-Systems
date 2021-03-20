CC = gcc
CC_FLAGS = -Wall -O0 -std=c11

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *merged*
	rm -f *test*
	rm -f *output*

compile4: clean
	$(CC) $(CC_FLAGS) zad_4_1.c -o output

run4:
	./output input.txt output.txt test dupa