CC = gcc
CC_FLAGS = -Wall

compile: clear cook supplier
	touch Table.txt
	touch Furnance.txt
	$(CC) $(CC_FLAGS) main.c -lrt -lpthread -o main

supplier:
	$(CC) $(CC_FLAGS) supplier.c -lrt -lpthread -o supplier

cook: 
	$(CC) $(CC_FLAGS) cook.c -lrt -lpthread -o cook

clear:
	rm -f Table.txt
	rm -f Furnance.txt
	rm -f cook
	rm -f supplier