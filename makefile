CC = gcc

thread.o: thread.c thread.h
	$(CC) -g -c thread.c

test01.o: test01.c
	$(CC) -g -c test01.c

test01x.o: test01x.c
	$(CC) -g -c test01x.c

clean: 
	rm -rf thread.o test01.o test01x.o
