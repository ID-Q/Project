/*************************************************************************
	> File Name: expression.cpp
	> Author: 
	> Mail: 
	> Created Time: 一 10/ 5 19:20:25 2020
 ************************************************************************/

#include "expression.h"

/* 各类访问者接口 */
void IdExpr::Accept(IVisitor* visitor) {
	visitor->Visit(this);
}

void NumberExpr::Accept(IVisitor* visitor) {
	visitor->Visit(this);
}

void BinaryExpr::Accept(IVisitor* visitor) {
	visitor->Visit(this);
}

void InvokeExpr::Accept(IVisitor* visitor) {
	visitor->Visit(this);
}
