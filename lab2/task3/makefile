all: exec

exec: menu_map.o
	gcc -m32 -g -Wall -o menu_map menu_map.o

menu_map.o: menu_map.c
	gcc -m32 -Wall -c menu_map.c -o menu_map.o

.PHONY: clean

clean:
	rm -f *.o menu_map
