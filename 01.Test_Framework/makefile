CC = gcc
CFLAGS =  -std=c99 -Wall -g
OBJS = main.o test.o testfunc1.o testfunc2.o testfunc3.o

.PHONY: clean

a.out : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o test
    
main.o : ./src/main.c 
	$(CC) -c $(CFLAGS) ./src/main.c 

test.o : ./src/test.c ./include/test.h 
	$(CC) -c $(CFLAGS) ./src/test.c 

testfunc1.o : ./src/testfunc1.c ./include/testfunc1.h 
	$(CC) -c $(CFLAGS) ./src/testfunc1.c 

testfunc2.o : ./src/testfunc2.c ./include/testfunc2.h 
	$(CC) -c $(CFLAGS) ./src/testfunc2.c 

testfunc3.o : ./src/testfunc3.c ./include/testfunc3.h 
	$(CC) -c $(CFLAGS) ./src/testfunc3.c

clean:
	rm -rf *.o test