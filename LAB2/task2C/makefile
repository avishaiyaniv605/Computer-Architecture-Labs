#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: 2

# Tool invocations
2: 2.o 
	gcc -m32 -g -Wall -o 2 2.o

2.o: 2.c
	gcc -g -Wall -m32  -c -o 2.o 2.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hello
