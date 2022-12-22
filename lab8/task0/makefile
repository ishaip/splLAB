all: exec

exec: hexeditplus.o
	gcc -m32 -g -Wall -o hexeditplus hexeditplus.o

hexeditplus.o: hexeditplus.c
	gcc -m32 -Wall -c hexeditplus.c -o hexeditplus.o

.PHONY: clean

clean:
	rm -f *.o hexeditplus
