CC = gcc
CFLAGS = -Wall -g -lpthread -D _DEBUG 
OBJS = Master.o node.o epoll.o common.o condition.o threadpool.o

.PHONY: clean

./a.out : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) 

Master.o : Master.c
	$(CC) -c $(CFLAGS) Master.c

node.o : ./node/node.c ./node/node.h 
	$(CC) -c $(CFLAGS) ./node/node.c 

epoll.o : ./epoll/epoll.c ./epoll/epoll.h 
	$(CC) -c $(CFLAGS) ./epoll/epoll.c

common.o : ./common/common.c ./common/common.h
	$(CC) -c $(CFLAGS) ./common/common.c

condition.o : ./condition/condition.c ./condition/condition.h 
	$(CC) -c $(CFLAGS) ./condition/condition.c 

threadpool.o : ./threadpool/threadpool.c ./threadpool/threadpool.h 
	$(CC) -c $(CFLAGS) ./threadpool/threadpool.c

clean:
	rm -rf *.o ./a.out

