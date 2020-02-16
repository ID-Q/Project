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
