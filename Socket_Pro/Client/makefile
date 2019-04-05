CC = gcc
CFLAGS = -Wall -g -lpthread -lz -D _DEBUG 
OBJS = Client.o common.o 

.PHONY: clean

./a.out : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) 

Client.o : Client.c
	$(CC) -c $(CFLAGS) Client.c

common.o : ./common/common.c ./common/common.h
	$(CC) -c $(CFLAGS) ./common/common.c

clean:
	rm -rf *.o ./a.out
