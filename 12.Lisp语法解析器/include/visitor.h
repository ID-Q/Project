/*************************************************************************
	> File Name: visitor.h
	> Author: 
	> Mail: 
	> Created Time: 一 10/ 5 19:18:11 2020
 ************************************************************************/

#ifndef _VISITOR_H
#define _VISITOR_H

#include "expression.h"
#include <string>
#include "utility.h"

using std::string;
using std::to_string;

struct ExprPointer :Expr::IVisitor {
	string result = "";
	void Visit(NumberExpr* node);
	void Visit(IdExpr* node);
	void Visit(BinaryExpr* node);
	void Visit(InvokeExpr* node);;
};

#endif
