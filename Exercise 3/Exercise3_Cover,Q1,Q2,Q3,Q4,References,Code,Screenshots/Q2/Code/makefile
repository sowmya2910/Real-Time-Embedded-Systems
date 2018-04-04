CC = gcc

# Setting the compiler flags
CFLAGS = -c -O0 -w

all: question2

question2 :
	$(CC) threadsafe.c -o threadsafe -pthread -lrt
	

build : threadsafe.c
	$(CC) question_2.c $(CFLAGS) -pthread -lrt


# Phony targets
.PHONY : clean
clean:
	rm -rf *.o *.map *.i *.s threadsafe
