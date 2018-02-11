#Sample Makefile for Malloc
CC=gcc
CFLAGS=-g -O0 -fPIC -Wall

TESTS=t-test1 test1
HEADERS= mallutl.h malloc.h free.h calloc.h realloc.h reallocarray.h memalign.h

all:	${TESTS} libmalloc.so

clean:
	rm -rf *.o *.so ${TESTS}

libmalloc.so: malloc.o free.o mallutl.o calloc.o realloc.o reallocarray.o memalign.o #add other sources here
	$(CC) $(CFLAGS) -shared -Wl,--unresolved-symbols=ignore-all $^ -o $@ -ldl -lm

%: %.c
	$(CC) $(CFLAGS) $< -o $@ -lpthread -lm

# For every XYZ.c file, generate XYZ.o.
%.o: %.c ${HEADERS}
	$(CC) $(CFLAGS) $< -c -o $@ -lm

check1:	libmalloc.so test1 t-test1
	LD_PRELOAD=`pwd`/libmalloc.so ./t-test1

check:	libmalloc.so ${TESTS}
	LD_PRELOAD=`pwd`/libmalloc.so ./test1

dist: clean
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
