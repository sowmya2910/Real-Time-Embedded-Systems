#Makefile for question5 

# Defining the compiler as c compiler(gcc)
CC=gcc

#Defining various flags which attaches various attributes required for makefile execution
CFLAGS= -Wall \
-O0


# Receipe to generate executable for problem2.c
problem5:problem5.c
	$(CC) $(CFLAGS)  -o problem5   problem5.c -lpthread -lrt

#PHONY target to remove the executables
.PHONY:clean
clean:
	rm problem5
