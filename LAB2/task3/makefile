#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: 3

# Tool invocations
3: 3.o 
	gcc -m32 -g -Wall -o 3 3.o

3.o: 3.c
	gcc -g -Wall -m32  -c -o 3.o 3.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hello
