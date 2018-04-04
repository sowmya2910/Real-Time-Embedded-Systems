INCLUDE_DIRS =  -I/usr/local/opencv/include
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lrt
CPPLIBS= -L/usr/local/opencv/lib -lopencv_core -lopencv_flann -lopencv_video

HFILES= 
CFILES= 

SRCS= ${HFILES} ${CFILES}

all: Q5

clean:
	-rm -f *.o *.d
	
	-rm -f JA Q5


distclean:
	-rm -f *.o *.d

JA: JA.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $@.o `pkg-config --libs opencv` $(CPPLIBS) -lpthread
	
Q5: Q5.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $@.o `pkg-config --libs opencv` $(CPPLIBS) -lpthread


depend:

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<
