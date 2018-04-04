#Makefile for question4 to generate heapmq ,posixmq

# Defining the compiler as c compiler(gcc)
CC=gcc

#Defining various flags which attaches various attributes required for makefile execution
CFLAGS= -Wall \
-O0


# Receipe to generate executable for posixmq.c
posixmq:posixmq.c
	$(CC) $(CFLAGS)  -o posixmq   posixmq.c -lpthread -lrt

# Receipe to generate executable for heapmq.c
heapmq:heapmq.c
	$(CC) $(CFLAGS)  -o heapmq   heapmq.c -lpthread -lrt

#PHONY target to remove the executables
.PHONY:clean
clean:
	rm heapmq posixmq
