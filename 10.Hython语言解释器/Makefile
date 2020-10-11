CC = g++
CFLAGS = -DDEBUG 
OBJS = 

all : test.g main.cpp
	java -jar ./antlr-3.4-complete.jar test.g 
	$(CC) $(CFLAGS) *.cpp *.c ./libs/libantlr3c.a -I. -o test 

.PHONY: run clean 
run:
	./test input

clean:
	rm test
