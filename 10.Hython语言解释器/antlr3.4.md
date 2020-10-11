# 使用Antlr的C接口(3.4版)

[1 Reply](https://www.coder4.com/archives/4016#comments)

Antlr 4，它是一个非常强大的词法、语法分析器辅助生成工具，比之前用Flex + Bison强太多倍。

遗憾的是，当前的v4只支持Java，暂不支持C、C++，于是降级了一把，尝试了一下在3.4版上使用Antlr C。

要说明的是，这不是一篇原创文章，我参考了两篇文章，如下：

《用ANTLR3实现规则解析—-1-安装》 http://blog.csdn.net/wfp458113181wfp/article/details/9148577

《ANTLR Example in C》 http://contrapunctus.net/blog/2012/antlr-c

并针对实际情况，对一些步骤做出了修改、补充，于是有了本文。

一、编译安装 antlr c library

```bash
wget http://www.antlr3.org/download/C/libantlr3c-3.4.tar.gz
tar -xzvf ./libantlr3c-3.4.tar.gz
cd libantlr3c-3.4
./configure --enable-64bit
make
sudo make install
#将所有的静态库放入到libs中
mkdir libs
cp ./libantlr3c-3.4/.lib/* libs
#将所有的头文件放入到include中
mkdir include
cp ./libantlr3c-3.4/include/* include
```

二、下载 antlr 3.4 jar包

尽管我们安装了c library，但是从.g文件，到各种.h .c文件的过程，还是要依赖antlr java的。

特别注意：必须要用3.4的jar包，我试了3.5.1，果断不行……

```bash
wget http://www.antlr3.org/download/antlr-3.4-complete.jar
```

三、编写语法文件(.g)

后续的语法，驱动程序，都是直接照搬开头提到的两篇参考文献，仅做了必要的修改。

```c
grammar test;

options {
    language = C;
    output = AST;
    ASTLabelType=pANTLR3_BASE_TREE;
}

@header {
    #include <assert.h>
}

// The suffix '^' means make it a root.
// The suffix '!' means ignore it.

expr: multExpr ((PLUS^ | MINUS^) multExpr)*
    ;

PLUS: '+';
MINUS: '-';

multExpr
    : atom (TIMES^ atom)*
    ;

TIMES: '*';

atom: INT
    | ID
    | '('! expr ')'!
    ;

stmt: expr NEWLINE -> expr  // tree rewrite syntax
    | ID ASSIGN expr NEWLINE -> ^(ASSIGN ID expr) // tree notation
    | NEWLINE ->   // ignore
    ;

ASSIGN: '=';

prog
    : (stmt {pANTLR3_STRING s = $stmt.tree->toStringTree($stmt.tree);
             assert(s->chars);
             printf(" tree \%s\n", s->chars);
            }
        )+
    ;

ID: ('a'..'z'|'A'..'Z')+ ;
INT: '~'? '0'..'9'+ ;
NEWLINE: '\r'? '\n' ;
WS : (' '|'\t')+ {$channel = HIDDEN;};
```

四、生成c中间文件 (Antlr Target C)

https://www.oracle.com/java/technologies/javase-downloads.html

```bash
java -jar ./antlr-3.4-complete.jar ./test.g
# 看一下文件，应该有这些
test.g  testLexer.c  testLexer.h  testParser.c  testParser.h  test.tokens
```

五、编写驱动文件

这里同样照搬的，main.cpp

```c
#include "testLexer.h"
#include "testParser.h"
#include <cassert>
#include <map>
#include <string>
#include <iostream>

using std::map;
using std::string;
using std::cout;

class ExprTreeEvaluator {
    map<string, int> memory;
public:
    ExprTreeEvaluator(ExprTreeEvaluator *next) {
        this->next = next;
    }
    int find(string var) {
        if (this->memory.find(var) != this->memory.end()) {
            return this->memory[var];
        }
        if (this->next) return this->next->find(var);
        return 0;
    }
    int run(pANTLR3_BASE_TREE);
    ExprTreeEvaluator *next;
};

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
const char* getText(pANTLR3_BASE_TREE tree);

int main(int argc, char* argv[]) {
  pANTLR3_INPUT_STREAM input;
  ptestLexer lex;
  pANTLR3_COMMON_TOKEN_STREAM tokens;
  ptestParser parser;

  assert(argc > 1);
  input = antlr3FileStreamNew((pANTLR3_UINT8)argv[1],ANTLR3_ENC_8BIT);
  lex = testLexerNew(input);
  tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
                                            TOKENSOURCE(lex));
  parser = testParserNew(tokens);
  testParser_prog_return r = parser->prog(parser);

  pANTLR3_BASE_TREE tree = r.tree;

  ExprTreeEvaluator eval(NULL);
  int rr = eval.run(tree);
  cout << "Evaluator result: " << rr << '\n';

  parser->free(parser);
  tokens->free(tokens);
  lex->free(lex);
  input->close(input);

  return 0;
}

int ExprTreeEvaluator::run(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if (tok) {
        switch (tok->type) {
        case INT: {
            const char* s = getText(tree);
            if (s[0] == '~') {
                return -atoi(s + 1);
            } else {
                return atoi(s);
            }
        }
        case ID: {
            string var(getText(tree));
            return this->find(var);
        }
        case PLUS:
            return run(getChild(tree, 0)) + run(getChild(tree, 1));
        case MINUS:
            return run(getChild(tree, 0)) - run(getChild(tree, 1));
        case TIMES:
            return run(getChild(tree, 0)) * run(getChild(tree, 1));
        case ASSIGN: {
            string var(getText(getChild(tree, 0)));
            int val = run(getChild(tree, 1));
            memory[var] = val;
            return val;
        }
        default:
            cout << "Unhandled token: #" << tok->type << '\n';
            return -1;
        }
    } else {
        cout << "in" << std::endl;
        int k = tree->getChildCount(tree);
        int r = 0;
        for (int i = 0; i < k; i++) {
            r = run(getChild(tree, i));
            printf("value : %d\n", r);
        }
        return r;
    }
}

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i) {
    assert(i < tree->getChildCount(tree));
    return (pANTLR3_BASE_TREE) tree->getChild(tree, i);
}

const char* getText(pANTLR3_BASE_TREE tree) {
    return (const char*) tree->getText(tree)->chars;
}

```

六、编译，测试

生成的可执行文件是test

```bash
# 此处，我直接链接的静态.a库
g++ -g -Wall *.cpp *.c ../lantlr3c/lib/libantlr3c.a -o test -I. -I ../lantlr3c/include/
```

测试数据为：

```bash
cat ./input
a = 4
b = 5
a + b
```

测试结果：

```bash
./test input
 tree (= b 5)
 tree (= a 4)
 tree (+ a b)
in
value : 5
value : 4
value : 9
Evaluator result: 9
```