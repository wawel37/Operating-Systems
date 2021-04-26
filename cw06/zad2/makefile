CC = gcc
CC_FLAGS = -Wall -std=c11

static:
	$(CC) $(CC_FLAGS) -c -ltr lib.c -o lib.o
	ar rcs lib.a lib.o

compile-client:
	$(CC) $(CC_FLAGS) client.c -lrt -lpthread -static -L. lib.a  -o client

compile-server:
	$(CC) $(CC_FLAGS) server.c -lrt -static -L. lib.a -o server 

clean: 
	rm -f *.o
	rm -f *.a
	rm -f server client