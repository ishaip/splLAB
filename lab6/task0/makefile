# Readme:
# gcc : gnu c/c++ compiler
# gcc flags:
# 	-m32 : use 32bit libraries
# 	-g : add debug info - no flag no debug
# 	-Wall : warning level - all
# 	-c : no linking (just compile)
# 	-o : optimize
# nasm : assembly compiler
# taken form an online form

# macros:
compC = gcc -m32 -g -Wall -c -o
complC = gcc -m32 -g -Wall -o
complS = nasm -g -f elf -w+all -o

.PHONY : clean

all: myshell

# (target) : (dependencies)
myshell : task0.o LineParser.o 
#	@echo "making exe"
	$(complC) myshell $^

LineParser.o : LineParser.c
#	@echo "making mainC.o"
	$(compC) LineParser.o $^

task0.o : task0.c 
#	@echo "making mainC.o"
	$(compC) task0.o $^

clean:
	rm -f *.o myshell