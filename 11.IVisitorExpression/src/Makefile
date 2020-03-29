CC = g++
CXX = g++
CPPFLAGS = -I../include -std=c++11 -Wextra -g
LDFLAGS = -std=c++11

EXENAME = main
.PHONY : clean TAGS all
all : $(EXENAME)
	mv $(EXENAME) ../
main : parser.o utility.o visitor.o Expression.o

main.o : main.cpp

parser.o : parser.cpp ../include/parser.h

utility.o : utility.cpp ../include/utility.h

visitor.o : visitor.cpp ../include/visitor.h

Expression.o : Expression.cpp ../include/Expression.h

clean :
	-rm -rf *.o *.dSYM
TAGS : clean all
