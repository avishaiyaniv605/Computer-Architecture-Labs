#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: encoder

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
encoder: encoder.o
	gcc -m32 -g -Wall -o encoder encoder.o

encoder.o: encoder.c
	gcc -g -Wall -m32  -c -o encoder.o encoder.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hello
