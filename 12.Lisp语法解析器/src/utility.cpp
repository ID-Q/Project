/*************************************************************************
	> File Name: utility.cpp
	> Author: 
	> Mail: 
	> Created Time: 一 10/ 5 19:26:05 2020
 ************************************************************************/

#include "utility.h"

/* 获取选项值 */
char GetOp(BinaryOperator B) {
	switch (B) {
	case BinaryOperator::Plus:
		return '+';
	case BinaryOperator::Minus:
		return '-';
	case BinaryOperator::Multiply:
		return '*';
	case BinaryOperator::Divide:
		return '/';
	}
}

/* 判断字符重合 */
bool Is(const char*& Stream, const char* Text) {
	size_t len = strlen(Text);
	const char* Read = Stream;
	while (*Read == ' ') Read++;
	if (strncmp(Read, Text, len) == 0) {
		Stream = Read + len;
		return true;
	} else {
		return false;
	}
}

