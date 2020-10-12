CC = g++
CFLAGS = -g -Wall
OBJS = 
TARGET='./bin/test'

all : 
	cd ./antlr3.4 && make
	$(CC) $(CFLAGS) *.cpp ./src/*.cpp ./antlr3.4/*.c ./antlr3.4/libs/libantlr3c.a -o $(TARGET) -I./include -I ./antlr3.4/include -I ./antlr3.4/

.PHONY: run clean 
run:
	$(TARGET) ./input

clean:
	rm -rf $(TARGET)
