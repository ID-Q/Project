/*************************************************************************
	> File Name: Expression.cpp
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 日  3/29 21:52:20 2020
 ************************************************************************/

#include "Expression.h"

Expression::Expression() {
	IsLeft = false;
}

void NumberExpression::Accept(IVisior* visitor) {
	visitor->Visit(this);
}

NumberExpression::NumberExpression(int number) {
	Value = number;
}

void BinaryExpression::Accept(IVisior* visitor) {
	visitor->Visit(this);
}

