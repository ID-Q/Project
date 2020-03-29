/*************************************************************************
	> File Name: main.cpp
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 日  3/29 21:52:56 2020
 ************************************************************************/

#include "visitor.h"
#include "parser.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;
int pass_count = 0;

void output() {
    pass_count++;
    cout << "Test " << pass_count << " passed." << endl;
}

void Assert(const char* exp, string regular)
{
    ExpressionPointer visitor;
    GetExp(exp)->Accept(&visitor);
    assert(visitor.result == regular);
    output();
}
int main() {
    //1
    Assert("(1+2)+3", "1+2+3");
    //2
    Assert("1+(2+3)", "1+2+3");
    //3
    Assert("(1+2+3)", "1+2+3");
    //4
    Assert("1-(2+3)", "1-(2+3)");
    //5
    Assert("1-(2-3)", "1-(2-3)");
    //6
    Assert("(1-2)+3", "1-2+3");
    //7
    Assert("(1+2)-3", "1+2-3");
    //8
    Assert("1+2*3", "1+2*3");
    //9
    Assert("1*2+3", "1*2+3");
    //10
    Assert("(1*2)+3", "1*2+3");
    //11
    Assert("1+(2*3)", "1+2*3");
    //12
    Assert("1*(2+3)", "1*(2+3)");
    //13
    Assert("(1+2)*3", "(1+2)*3");
    //14
    Assert("1+2/3", "1+2/3");
    //15
    Assert("1/2+3", "1/2+3");
    //16
    Assert("(1/2)+3", "1/2+3");
    //17
    Assert("1+(2/3)", "1+2/3");
    //18
    Assert("1/(2+3)", "1/(2+3)");
    //19
    Assert("(1+2)/3", "(1+2)/3");
    //20
    Assert("(1 * 2) / 3", "1*2/3");
    //21
    Assert("1/(2*3)", "1/(2*3)");

    cout << "You have passed all tests!" << endl;
    return 0;
}



