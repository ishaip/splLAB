all: exec

exec: virusDetector.o
	gcc -m32 -g -Wall -o virusDetector virusDetector.o

virusDetector.o: virusDetector.c
	gcc -m32 -Wall -c virusDetector.c -o virusDetector.o

.PHONY: clean

clean:
	rm -f *.o virusDetector
