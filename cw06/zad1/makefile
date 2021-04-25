CC = gcc
CC_FLAGS = -Wall -std=c11 -g

static:
	$(CC) $(CC_FLAGS) -c lib.c -o lib.o
	ar rcs lib.a lib.o

compile-client:
	$(CC) $(CC_FLAGS) client.c -static -L. lib.a -o client

compile-server:
	$(CC) $(CC_FLAGS) server.c -static -L. lib.a -o server