all: exec

exec: Bubblesort.o
	gcc -m32 -g -Wall -o Bubblesort Bubblesort.o

Bubblesort.o: Bubblesort.c
	gcc -m32 -Wall -c Bubblesort.c -o Bubblesort.o

.PHONY: clean

clean:
	rm -f *.o Bubblesort
