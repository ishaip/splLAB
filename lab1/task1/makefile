all: exec

exec: task1a.o
	gcc -m32 -g -Wall -o encoder task1a.o

task1a.o: task1a.c
	gcc -m32 -Wall -c task1a.c -o task1a.o

.PHONY: clean

clean:
	rm -f *.o encoder
