CC = gcc
CC_FLAGS = -Wall -O0 -std=c11

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *merged*
	rm -f *test*
	rm -f *output*

compile1: clean
	$(CC) $(CC_FLAGS) zad_1_1.c -o output

run1:
	./output text1.txt text2.txt