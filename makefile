CC = gcc

.PHONY:all
all: thread test01 test01x

linked_list.o: linked_list.c linked_list.h
	$(CC) -g -c linked_list.c

thread.o: thread.c thread.h linked_list.o
	$(CC) -g -c thread.c

test01: test01.c thread.o
	$(CC) -g -c test01.c -o test01 thread.o linked_list.o

test01x: test01x.c thread.o
	$(CC) -g -c test01x.c -o test01x thread.o linked_list.o

clean: 
	rm -rf thread.o test01.o test01x.o
