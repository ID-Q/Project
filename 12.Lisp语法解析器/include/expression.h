/*************************************************************************
	> File Name: expression.h
	> Author: 
	> Mail: 
	> Created Time: 一 10/ 5 19:01:22 2020
 ************************************************************************/

#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include <string>
#include <memory>
#include <vector>

using std::string;
using std::shared_ptr;
using std::vector;

struct Expr;
struct IdExpr;
struct NumberExpr;
struct BinaryExpr;
struct InvokeExpr;
enum class BinaryOperator {
	Plus,
	Minus,
	Multiply,
	Divide,
};

struct Expr {
	//  virtual string print() = 0;
	struct IVisitor {
		virtual void Visit(IdExpr* node) = 0;
		virtual void Visit(NumberExpr* node) = 0;
		virtual void Visit(BinaryExpr* node) = 0;
		virtual void Visit(InvokeExpr* node) = 0;
	};
	virtual void Accept(IVisitor* visitor) = 0;
};

struct IdExpr :Expr {
	void Accept(IVisitor* visitor);
	string name;
};

struct NumberExpr :Expr {
	int number;
	void Accept(IVisitor* visitor);
};

struct BinaryExpr :Expr {
	BinaryOperator op;
	shared_ptr<Expr> first;
	shared_ptr<Expr> second;
	void Accept(IVisitor* visitor);
};

struct InvokeExpr :Expr {
	string name;
	vector<shared_ptr<Expr>>arguments;
	void Accept(IVisitor* visitor);
};

#endif
