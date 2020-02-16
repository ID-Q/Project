CC = gcc
CFLAGS = -Wall -g -D _DEBUG 
OBJS = parser.o lexer.o

.PHONY: clean

./a.out : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o mycalc 

parser.o : parser.c token.h
	$(CC) -c $(CFLAGS) parser.c

lexer.o : lexer.c token.h 
	$(CC) -c $(CFLAGS) lexer.c 

clean:
	rm -rf *.o mycalc

