CC = gcc
CC_FLAGS = -Wall -O0 -std=c11
RAPORT2 = raport2.txt
RAPORT3 = raport3.txt

static:
	$(CC) $(CC_FLAGS) -c lib.c -o lib.o
	ar rcs lib.a lib.o

shared:
	$(CC) $(CC_FLAGS) -fPIC -shared lib.c -o lib.so

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *merged*
	rm -f *test*
	rm -f *output*

main1: clean static
	$(CC) $(CC_FLAGS) zad1.c -static -L. lib.a -o output
	make run1

main2Static: clean static
	$(CC) $(CC_FLAGS) zad2.c -static -L. lib.a -o output

main2Shared: clean shared
	$(CC) $(CC_FLAGS) zad2.c lib.so -L. -Wl,-rpath=. -o output

main3Dynamic: clean shared
	$(CC) $(CC_FLAGS) zad3.c -ldl -o output

makeAndRun1:
	./output
	make clean

run2:
	./output create_table 2 merge_files file_1.txt:file_2.txt file_3.txt:file_4.txt remove_row 0 2 remove_block 0 test 5 10 test 5 100 test 200 100 test 200 400 test 1000 100 test 1000 600 >> $(RAPORT2)
	make clean

run3:
	./output create_table 2 merge_files file_1.txt:file_2.txt file_3.txt:file_4.txt remove_row 0 2 remove_block 0 test 5 10 test 5 100 test 200 100 test 200 400 test 1000 100 test 1000 600 >> $(RAPORT3)
	make clean

raport2:
	make main2Static
	@echo "\n------STATIC TESTS------\n\n" >> $(RAPORT2)
	make run2
	make main2Shared
	@echo "\n------SHARED TESTS------\n\n" >> $(RAPORT2)
	make run2

raport3:
	@echo "\n-------------- O0 tests ----------\n\n\n" >> $(RAPORT3)



	@echo "\n------STATIC TESTS------\n\n" >> $(RAPORT3)
	make static
	$(CC) -Wall -O0 -std=c11 zad2.c -static -L. lib.a -o output
	make run3
	make clean
	
	@echo "\n------SHARED TESTS------\n\n" >> $(RAPORT3)
	make shared
	$(CC) -Wall -O0 -std=c11 zad2.c lib.so -L. -Wl,-rpath=. -o output
	make run3
	make clean

	@echo "\n------DYNAMIC TESTS------\n\n" >> $(RAPORT3)
	make shared
	$(CC) -Wall -O0 -std=c11 zad3.c -ldl -o output
	make run3
	make clean

	
	@echo "\n-------------- O1 tests ----------\n\n\n" >> $(RAPORT3)


	@echo "\n------STATIC TESTS------\n\n" >> $(RAPORT3)
	make static
	$(CC) -Wall -O1 -std=c11 zad2.c -static -L. lib.a -o output
	make run3
	make clean
	
	@echo "\n------SHARED TESTS------\n\n" >> $(RAPORT3)
	make shared
	$(CC) -Wall -O1 -std=c11 zad2.c lib.so -L. -Wl,-rpath=. -o output
	make run3
	make clean

	@echo "\n------DYNAMIC TESTS------\n\n" >> $(RAPORT3)
	make shared
	$(CC) -Wall -O1 -std=c11 zad3.c -ldl -o output
	make run3
	make clean


	@echo "\n-------------- O2 tests ----------\n\n\n" >> $(RAPORT3)


	@echo "\n------STATIC TESTS------\n\n" >> $(RAPORT3)
	make static
	$(CC) -Wall -O2 -std=c11 zad2.c -static -L. lib.a -o output
	make run3
	make clean
	
	@echo "\n------SHARED TESTS------\n\n" >> $(RAPORT3)
	make shared
	$(CC) -Wall -O2 -std=c11 zad2.c lib.so -L. -Wl,-rpath=. -o output
	make run3
	make clean

	@echo "\n------DYNAMIC TESTS------\n\n" >> $(RAPORT3)
	make shared
	$(CC) -Wall -O2 -std=c11 zad3.c -ldl -o output
	make run3
	make clean





