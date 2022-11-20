# Readme:
# gcc : gnu c/c++ compiler
# gcc flags:
# 	-m32 : use 32bit libraries
# 	-g : add debug info - no flag no debug
# 	-Wall : warning level - all
# 	-c : no linking (just compile)
# 	-o : optimize
# nasm : assembly compiler

# macros:
compC = gcc -m32 -g -Wall -c -o
complC = gcc -m32 -g -Wall -o
complS = nasm -g -f elf -w+all -o

.PHONY : clean

all: task4

# (target) : (dependencies)
task4 :  task4.o
#	@echo "making exe"
	$(complC) task4 task4.o

task4.o : task4.s
#	@echo "making task4.o"
	$(complS) task4.o task4.s

clean:
	rm -f *.o task4