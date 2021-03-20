CC = gcc
CC_FLAGS = -Wall -O0 -std=c11

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *merged*
	rm -f *test*
	rm -f *output*

compile5: clean
	$(CC) $(CC_FLAGS) zad_5_1.c -o output

run5:
	./output input.txt output.txt