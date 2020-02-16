# 自制计算器

## 功能介绍

- 实现+、-、*、/、（ ）、负数运算

## 涉及功能

- 自制词法分析器
- 自制语法分析器

## 实现情况

- ![](http://zhengdongqi.oss-cn-beijing.aliyuncs.com/2020-02-12/63A2275B54F546AF8D4C04E16AC27FB9.jpg)

## 程序框架

- ![](http://zhengdongqi.oss-cn-beijing.aliyuncs.com/2020-02-12/9F4C1C7C259246F6BC76428344543C14.jpg)

## 代码实现

词法分析器

```c
/*************************************************************************
	> Description:自制词法分析器 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "token.h"

static char *st_line;
static int st_line_pos;

/*存储数值特征*/
typedef enum {
    INITIAL_STATUS,//初始化状态
    IN_INT_PART_STATUS,//遇到整数
    DOT_STATUS,//遇到小数点
    IN_FRAC_PART_STATUS//再遇到整数
} LexerStatus;

void get_token(Token *token) {
    int out_pos = 0;
    LexerStatus status = INITIAL_STATUS;
    char current_char;

    token->kind = BAD_TOKEN;
    while (st_line[st_line_pos] != '\0') {
        current_char = st_line[st_line_pos];
        if ((status == IN_INT_PART_STATUS || status == IN_FRAC_PART_STATUS) && !isdigit(current_char) && current_char != '.') {
            token->kind = NUMBER_TOKEN;
            sscanf(token->str, "%lf", &token->value);
            return;
        }
        if (isspace(current_char)) {
            if (current_char == '\n') {
                token->kind = END_OF_LINE_TOKEN;
                return;
            }
            st_line_pos++;
            continue;
        }

        if (out_pos >= MAX_TOKEN_SIZE - 1) {
            fprintf(stderr, "token too long.\n");
            exit(1);
        }
        token->str[out_pos] = st_line[st_line_pos];
        st_line_pos++;
        out_pos++;
        token->str[out_pos] = '\0';

        if (current_char == '+') {
            token->kind = ADD_OPERATOR_TOKEN;
            return;
        } else if (current_char == '-') {
            token->kind = SUB_OPERATOR_TOKEN;
            return;
        } else if (current_char == '*') {
            token->kind = MUL_OPERATOR_TOKEN;
            return;
        } else if (current_char == '/') {
            token->kind = DIV_OPERATOR_TOKEN;
            return;
        } else if (current_char == '(') {
            token->kind = LEFT_PAREN_TOKEN;
            return;
        } else if (current_char == ')') {
            token->kind = RIGHT_PAREN_TOKEN;
            return;
        } else if (isdigit(current_char)) {
            if (status == INITIAL_STATUS) {
                status = IN_INT_PART_STATUS;
            } else if (status == DOT_STATUS) {
                status = IN_FRAC_PART_STATUS;
            }
        } else if (current_char == '.') {
            if (status == IN_INT_PART_STATUS) {
                status = DOT_STATUS;
            } else {
                fprintf(stderr, "syntax error.\n");
                exit(1);
            }
        }
    }
}

void set_line(char *line) {
    st_line = line;
    st_line_pos = 0;
}

#if 0
void
parse_line(void) {
    Token token;
    st_line_pos = 0;

    for (;;) {
        get_token(&token);
        if (token.kind == END_OF_LINE_TOKEN) {
          break;
        } else {
            printf("kind..%d, str..%s\n", token.kind, token.str);
        }
    }
}

int main(int argc, char **argv) {
    while (fgets(st_line, LINE_BUF_SIZE, stdin) != NULL) {
        parse_line();
    }

    return 0;
}
#endif

```

语法分析器

```c
/*************************************************************************
    > Description:自制语法分析器
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

#define LINE_BUF_SIZE (1024)

static Token st_look_ahead_token;
static int st_look_ahead_token_exists;

static void my_get_token(Token *token) {
    if (st_look_ahead_token_exists) {
        *token = st_look_ahead_token;
        st_look_ahead_token_exists = 0;
    } else {
        get_token(token);
    }
}

static void unget_token(Token *token) {
    st_look_ahead_token = *token;
    st_look_ahead_token_exists = 1;
}

double parse_expression(void);

static double parse_primary_expression() {
    Token token;
    double value = 0.0;
    int minus_flag = 0;

    my_get_token(&token);
    if (token.kind == SUB_OPERATOR_TOKEN) {
        minus_flag = 1;
    } else {
        unget_token(&token);
    }

    my_get_token(&token);
    if (token.kind == NUMBER_TOKEN) {
        value = token.value;
    } else if (token.kind == LEFT_PAREN_TOKEN) {
        value = parse_expression();
        my_get_token(&token);
        if (token.kind != RIGHT_PAREN_TOKEN) {
            fprintf(stderr, "missing ')' error.\n");
            exit(1);
        }
    } else {
        unget_token(&token);
    }
    if (minus_flag) {
        value = -value;
    }
    return value;
}

static double parse_term() {
    double v1;
    double v2;
    Token token;

    v1 = parse_primary_expression();
    for (;;) {
        my_get_token(&token);
        if (token.kind != MUL_OPERATOR_TOKEN && token.kind != DIV_OPERATOR_TOKEN) {
            unget_token(&token);
            break;
        }
        v2 = parse_primary_expression();
        if (token.kind == MUL_OPERATOR_TOKEN) {
            v1 *= v2;
        } else if (token.kind == DIV_OPERATOR_TOKEN) {
            v1 /= v2;
        }
    }
    return v1;
}

double parse_expression() {
    double v1;
    double v2;
    Token token;

    v1 = parse_term();
    for (;;) {
        my_get_token(&token);
        if (token.kind != ADD_OPERATOR_TOKEN && token.kind != SUB_OPERATOR_TOKEN) {
            unget_token(&token);
            break;
        }
        v2 = parse_term();
        if (token.kind == ADD_OPERATOR_TOKEN) {
            v1 += v2;
        } else if (token.kind == SUB_OPERATOR_TOKEN) {
            v1 -= v2;
        } else {
            unget_token(&token);
        }
    }
    return v1;
}

double parse_line(void) {
    double value;

    st_look_ahead_token_exists = 0;
    value = parse_expression();

    return value;
}

int main(int argc, char **argv) {
    char line[LINE_BUF_SIZE];
    double value;

    while (fgets(line, LINE_BUF_SIZE, stdin) != NULL) {
        set_line(line);
        value = parse_line();
        printf(">>%f\n", value);
    }

    return 0;
}

```

头文件

```c
#ifndef INCLUDE_H_INCLUDED
#define INCLUDE_H_INCLUDED

//枚举记号种类状态
typedef enum {
    BAD_TOKEN, //错误记号
    NUMBER_TOKEN, //数字记号
    ADD_OPERATOR_TOKEN, //‘+’记号
    SUB_OPERATOR_TOKEN, //‘-’记号
    MUL_OPERATOR_TOKEN, //‘*’记号
    DIV_OPERATOR_TOKEN, //‘/’记号
    LEFT_PAREN_TOKEN,   //‘(’记号
    RIGHT_PAREN_TOKEN,  //‘)’记号
    END_OF_LINE_TOKEN   //结束记号
} TokenKind;

#define MAX_TOKEN_SIZE (100)

typedef struct {
    TokenKind kind;//记录记号
    double value;//记录数值
    char str[MAX_TOKEN_SIZE];//记录运算符
} Token;

/* 将接下来要解析的行置入词法分析器中 */
void set_line(char *line);

/* 从被置入的行中，分割记号并返回
* 在行尾会返回 END_OF_LINE_TOKEN 这种特殊的记号
* 接受的入口参数为一个 Token 结构体指针
* 函数中会分割出 记号的信息装入 Token 结构体并返回*/
void get_token(Token *token);

#endif /* INCLUDE_H_INCLUDED */

```

makefile

```bash
CC = gcc
CFLAGS = -Wall -g -D _DEBUG 
OBJS = parser.o lexer.o

.PHONY: clean

./a.out : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o mycalc 

parser.o : parser.c token.h
	$(CC) -c $(CFLAGS) parser.c

lexer.o : lexer.c token.h 
	$(CC) -c $(CFLAGS) lexer.c 

clean:
	rm -rf *.o mycalc
```

