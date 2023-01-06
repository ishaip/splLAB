all: exec

exec: task0.o
	gcc -m32 -g -Wall -o task0 task0.o

task0.o: task0.c
	gcc -m32 -Wall -c task0.c -o task0.o

.PHONY: clean

clean:
	rm -f *.o task0