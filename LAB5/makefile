#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: 1B 

# Tool invocations
1B: 1B.o LineParser.o
	gcc -m32 -g -Wall -o 1B 1B.o LineParser.o

1B.o: 1B.c 
	gcc -g -Wall -m32  -c -o 1B.o 1B.c 

LineParser.o: LineParser.c
	gcc -g -Wall -m32  -c -o LineParser.o LineParser.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o 1B
