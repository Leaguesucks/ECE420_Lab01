all: 

lab1_IO.o: lab1_IO.c lab1_IO.h
	gcc -Wall -g -c lab1_IO.c

matrixgen:
	gcc matrixgen.c -o matrixgen

clean:
	rm *.o matrixgen