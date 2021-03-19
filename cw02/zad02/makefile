CC = gcc
CC_FLAGS = -Wall -O0 -std=c11

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *merged*
	rm -f *test*
	rm -f *output*

compile2: clean
	$(CC) $(CC_FLAGS) zad_2_1.c -o output

run2:
	./output a text.txt