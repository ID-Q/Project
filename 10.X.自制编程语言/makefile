C+ = g++
CFLAGS = -Wall -g
TARGET='./bin/test'
OBJS = ./build/*.o 
LINK1 = ./build/ 
LINk2 = ./libantlr3c-3.4/include
ANTLR = ./build/libs/libantlr3c.a
.PHONY: clean


all: 
	cd ./build && make
	$(C+) $(CFLAGS) *.cpp $(OBJS) $(ANTLR) -o $(TARGET) -I. -I $(LINK1) -I $(LINk2)
	$(TARGET) ./input

clean:
	cd ./build && make clean
	rm -rf $(TARGET)
