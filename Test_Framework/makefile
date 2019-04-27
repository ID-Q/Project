CC = gcc
CFLAGS =  -std=c99 -Wall -g
OBJS = main.o test.o testfunc1.o testfunc2.o testfunc3.o

.PHONY: clean

a.out : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o test
    
main.o : main.c
	$(CC) -c $(CFLAGS) main.c

test.o : test.c test.h
	$(CC) -c $(CFLAGS) test.c

testfunc1.o : testfunc1.c testfunc1.h
	$(CC) -c $(CFLAGS) testfunc1.c

testfunc2.o : testfunc2.c testfunc2.h
	$(CC) -c $(CFLAGS) testfunc2.c

testfunc3.o : testfunc3.c testfunc3.h
	$(CC) -c $(CFLAGS) testfunc3.c

clean:
	rm -rf *.o test
