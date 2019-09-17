#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: task1c task2b 

# Tool invocations
task1c: task1c.o
	gcc -m32 -g -Wall -o task1c task1c.o

task1c.o: task1c.c
	gcc -g -Wall -m32  -c -o task1c.o task1c.c 

task2b: task2b.o 
	gcc -m32 -g -Wall -o task2b task2b.o

task2b.o: task2b.c
	gcc -g -Wall -m32  -c -o task2b.o task2b.c 


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hello
