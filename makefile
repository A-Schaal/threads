CC = gcc

TESTS = test01@ test01x@ test03@ test06@ test05@ test08@
BINS = $(subst @,,$(TESTS))

LIBRARY_OBJS = linked_list.o thread.o semaphore.o mailbox.o

.PHONY:all
all: $(BINS)

linked_list.o: linked_list.c linked_list.h
	$(CC) -g -c linked_list.c

thread.o: thread.c thread.h linked_list.h
	$(CC) -g -c thread.c

semaphore.o: semaphore.c semaphore.h 
	$(CC) -g -c semaphore.c

mailbox.o: mailbox.c mailbox.h
	$(CC) -g -c mailbox.c

#generic rule for making tests
test%: test%.c $(LIBRARY_OBJS)
	$(CC) -g $< -o $@ $(LIBRARY_OBJS)

clean: 
	rm -rf *.o $(BINS)
