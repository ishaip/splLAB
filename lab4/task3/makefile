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

all: task3

# (target) : (dependencies)
task3 :  task3.o
#	@echo "making exe"
	$(complC) task3 task3.o

task3.o : task3.s
#	@echo "making task3.o"
	$(complS) task3.o task3.s

clean:
	rm -f *.o task3