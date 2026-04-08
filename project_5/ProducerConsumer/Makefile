# makefile for producer consumer
#

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: main.o buffer.o
	$(CC) $(CFLAGS) -o producer_consumer main.o buffer.o $(PTHREADS)

main.o: main.c buffer.h
	$(CC) $(CFLAGS) -c main.c $(PTHREADS)

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c $(PTHREADS)

clean:
	rm -rf *.o
	rm -rf producer_consumer
