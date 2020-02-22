CC = gcc
CFLAGS = -c 

.PHONY: clean

all :
	java -jar ./antlr-3.4-complete.jar ./hl.g
	$(CC) $(CFLAGS) hlLexer.c hlParser.c

clean:
	rm -rf *.h *.c *.o
