CC = gcc
CC_FLAGS = -Wall -std=c11

compile: clear cook supplier
	touch Table.txt
	touch Furnance.txt
	$(CC) $(CC_FLAGS) main.c -o main

supplier:
	$(CC) $(CC_FLAGS) supplier.c -o supplier

cook: 
	$(CC) $(CC_FLAGS) cook.c -o cook

clear:
	rm -f Table.txt
	rm -f Furnance.txt
	rm -f cook
	rm -f supplier