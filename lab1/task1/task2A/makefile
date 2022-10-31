all: exec

exec: encoder.o
	gcc -m32 -g -Wall -o encoder encoder.o

encoder.o: encoder.c
	gcc -m32 -Wall -c encoder.c -o encoder.o

.PHONY: clean

clean:
	rm -f *.o encoder
