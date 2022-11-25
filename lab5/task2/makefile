.PHONY : clean

all: main

# (target) : (dependencies)
main : task2.o util.o main.o
#	@echo "making exe"
	ld -m elf_i386 task2.o main.o util.o -o main

task2.o : task2.s # util.l
#	@echo "making mainC.o"
	nasm -f elf32 task2.s -o task2.o

util.o : util.c # util.l
#	@echo "making mainC.o"
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

main.o : main.c 
#	@echo "making mainC.o"
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o

clean:
	rm -f *.o main out