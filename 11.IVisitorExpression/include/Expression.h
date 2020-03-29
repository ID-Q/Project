/*************************************************************************
	> File Name: Expression.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 日  3/29 21:50:40 2020
 ************************************************************************/

#ifndef _EXPRESSION_H
#define _EXPRESSION_H
#pragma once
#include <memory>
using std::shared_ptr;
enum class BinaryOperator {
	Plus,
	Minus,
	Multiply,
	Divide,
};

class Expression;
class NumberExpression;
class BinaryExpression;

class Expression {
public:
	bool IsLeft;
	Expression();

	class IVisior {
	public:
		virtual void Visit(NumberExpression* node) = 0;
		virtual void Visit(BinaryExpression* node) = 0;
		virtual ~IVisior() = default;
	};

	virtual void Accept(IVisior* visitor) = 0;
	virtual ~Expression() = default;
};

class NumberExpression :public Expression {
public:
	int Value;
	void Accept(IVisior* visitor);
	NumberExpression(int number);

};

class BinaryExpression :public Expression {
public:
	shared_ptr<Expression> First;
	shared_ptr<Expression> Second;
	BinaryOperator Op;
	BinaryExpression(BinaryOperator theOp, shared_ptr<Expression> theLeft, shared_ptr<Expression> theRight) :
		Op(theOp), First(theLeft), Second(theRight) {

	}
	void Accept(IVisior* visitor);
};

#endif
