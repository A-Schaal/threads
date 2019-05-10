CC = gcc

TESTS = test01@ test01x@
BINS = $(subst @,,$(TESTS))

.PHONY:all
all: test01 test01x semaphore.o test03

linked_list.o: linked_list.c linked_list.h
	$(CC) -g -c linked_list.c

thread.o: thread.c thread.h linked_list.o
	$(CC) -g -c thread.c

semaphore.o: semaphore.c semaphore.h 
	$(CC) -g -c semaphore.c

test01: test01.c thread.o
	$(CC) -g test01.c -o test01 thread.o linked_list.o

test01x: test01x.c thread.o
	$(CC) -g test01x.c -o test01x thread.o linked_list.o

test03: test03.c semaphore.o
	$(CC) -g test03.c -o test03 semaphore.o thread.o linked_list.o
clean: 
	rm -rf *.o $(BINS)
