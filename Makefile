all: matrixgen main test

test: test.o lab1_IO.o
	gcc -Wall -pthread -g test.o lab1_IO.o -o test

test.o: lab1_IO.h timer.h
	gcc -Wall -pthread -g -c test.c -o test.o

main: main.o lab1_IO.o
	gcc -Wall -pthread -g main.o lab1_IO.o -o main

main.o: lab1_IO.h timer.h
	gcc -Wall -pthread -g -c main.c -o main.o

lab1_IO.o: lab1_IO.c lab1_IO.h
	gcc -Wall -pthread -g -c lab1_IO.c -o lab1_IO.o

matrixgen:
	gcc matrixgen.c -o matrixgen

clean:
	rm *.o matrixgen main test data_input data_output

# Only remove data_input and data_output. Might be useful for debug
clean1:
	rm data_input data_output