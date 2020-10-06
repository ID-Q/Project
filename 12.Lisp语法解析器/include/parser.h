/*************************************************************************
	> File Name: parser.h
	> Author: 
	> Mail: 
	> Created Time: 一 10/ 5 19:16:27 2020
 ************************************************************************/

#ifndef _PARSER_H
#define _PARSER_H

#include "expression.h"
#include "handler.h"
#include "utility.h"
#include <memory>

using std::shared_ptr;

struct Exception {
	const char* Start;
	const char* Error;
};
shared_ptr<Expr> Parse(const char*& expression, ExprHandler* handler);

#endif
