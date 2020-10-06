CC = g++
CXX = g++
CPPFLAGS = -I../include -std=c++14 -Wextra -g
LDFLAGS = -std=c++14

EXENAME = main
.PHONY : clean TAGS all
all : $(EXENAME)
	mv $(EXENAME) ../
main : parser.o utility.o visitor.o expression.o handler.o

main.o : main.cpp

parser.o : parser.cpp ../include/parser.h

utility.o : utility.cpp ../include/utility.h

visitor.o : visitor.cpp ../include/visitor.h 

expression.o : expression.cpp ../include/expression.h 

clean :
	-rm -rf *.o *.dSYM
TAGS : clean all

