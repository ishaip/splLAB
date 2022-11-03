all: exec

exec: addresses.o
	gcc -m32 -g -Wall -o addresses addresses.o

addresses.o: addresses.c
	gcc -m32 -Wall -c addresses.c -o addresses.o

.PHONY: clean

clean:
	rm -f *.o addresses
