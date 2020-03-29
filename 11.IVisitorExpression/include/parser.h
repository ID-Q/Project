/*************************************************************************
	> File Name: parser.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 日  3/29 21:51:05 2020
 ************************************************************************/

#ifndef _PARSER_H
#define _PARSER_H
#pragma once
#include <memory>
#include "Expression.h"
using std::shared_ptr;
shared_ptr<Expression> GetNumber(const char*& Stream);
shared_ptr<Expression> GetTerm(const char*& Stream);
shared_ptr<Expression> GetFactor(const char*& Stream);
shared_ptr<Expression> GetExp(const char*& Stream);

#endif
