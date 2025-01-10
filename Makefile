all: matrixgen main

main: main.o lab1_IO.o
	gcc -Wall -pthread -g main.o lab1_IO.o -o main

main.o: lab1_IO.h timer.h
	gcc -Wall -pthread -g -c main.c -o main.o

lab1_IO.o: lab1_IO.c lab1_IO.h
	gcc -Wall -pthread -g -c lab1_IO.c -o lab1_IO.o

matrixgen:
	gcc matrixgen.c -o matrixgen

clean:
	rm *.o matrixgen main data_input data_output