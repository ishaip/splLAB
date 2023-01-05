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

all: mypipe

# (target) : (dependencies)
mypipe : mypipe.o   # looper.o
#	@echo "making exe"
	$(complC) mypipe $^

mypipe.o : mypipe.c 
#	@echo "making mainC.o"
	$(compC) mypipe.o $^

#LineParser.o : LineParser.c
#	@echo "making mainC.o"
#	$(compC) LineParser.o $^

#looper.o : looper.c 
#	@echo "making mainC.o"
#	$(compC) looper.o $^

clean:
	rm -f *.o mypipe