# 自制编程语言

## 安装antlr运行环境

-  这里使用antlr3.4版本，[下载地址](https://github.com/antlr/website-antlr3/tree/gh-pages/download)

- 按照install文件的说明的安装步骤执行安装

    ```sh
    ./configure  --enable-64bit
    ```

    - ```sh
        make install
        #将会生成所有的.o（对象文件）
        ```

- 安装antlr Tool

    - 先安装java JDK ,[下载地址](https://www.oracle.com/java/technologies/javase-jdk13-downloads.html)

    - 下载antlr-3.4-complete.jar，[下载地址](https://github.com/antlr/website-antlr3/tree/gh-pages/download)

    - ```sh
        cd  /usr/local/lib
        #将antlr-3.4-complete.jar放在目录下
        #然后到用户目录下修改.bash_profile
        vim ./bash_profile
        export CLASSPATH=/usr/local/lib/antlr-3.4-complete.jar:$CLASSPATH #修改如下
        ```

    - 运行：java org.antlr.Tool 

    - ```bash
        #显示如下——安装成功
        zhengdongqideMacBook:~ zhengdongqi$ java org.antlr.Tool
        ANTLR Parser Generator  Version 3.4
        usage: java org.antlr.Tool [args] file.g [file2.g file3.g ...]
          -o outputDir          specify output directory where all output is generated
          -fo outputDir         same as -o but force even files with relative paths to dir
          -lib dir              specify location of token files
          -depend               generate file dependencies
          -report               print out a report about the grammar(s) processed
          -print                print out the grammar without actions
          -debug                generate a parser that emits debugging events
          -profile              generate a parser that computes profiling information
          -trace                generate a recognizer that traces rule entry/exit
          -nfa                  generate an NFA for each rule
          -dfa                  generate a DFA for each decision point
          -message-format name  specify output style for messages
          -verbose              generate ANTLR version and other information
          -make                 only build if generated files older than grammar
          -version              print the version of ANTLR and exit.
          -language L           override language grammar option; generate L
          -X                    display extended argument list
        zhengdongqideMacBook:~ zhengdongqi$
        ```

    - /usr/local/lib路径下会生成许多对象文件

        ```c
        //我们需要一下文件
        libantlr3c.a//静态库
        libantlr3c.dylib
        libantlr3c.dylib.dSYM
        ```

        

## 了解antlr3

- antlr是指可以根据输入自动生成语法树并可视化的显示出来的开源语法分析器。ANTLR—Another Tool for Language Recognition，其前身是PCCTS，它为包括Java，C++，C#在内的语言提供了一个通过语法描述来自动构造自定义语言的识别器（recognizer），[编译器](https://baike.baidu.com/item/编译器/8853067)（parser）和[解释器](https://baike.baidu.com/item/解释器/10418965)（translator）的框架。

- 词法分析器

> [词法分析器](https://baike.baidu.com/item/词法分析器)又称为Scanner，Lexical analyser和Tokenizer。[程序设计语言](https://baike.baidu.com/item/程序设计语言)通常由[关键字](https://baike.baidu.com/item/关键字)和严格定义的语法结构组成。编译的最终目的是将程序设计语言的高层指令翻译成物理机器或[虚拟机](https://baike.baidu.com/item/虚拟机)可以执行的指令。词法分析器的工作是分析量化那些本来毫无意义的字符流，将他们翻译成离散的字符组（也就是一个一个的Token），包括关键字，标识符，符号（symbols）和操作符供[语法分析器](https://baike.baidu.com/item/语法分析器)使用。
>
> **词法分析器**
>
> 将输入的字符序列分解成一系列词法符号。一个词法分析器负责分析词法

- 语法分析器

> [编译器](https://baike.baidu.com/item/编译器)又称为Syntactical analyser。在分析字符流的时候，Lexer不关心所生成的单个Token的语法意义及其与上下文之间的关系，而这就是Parser的工作。语法分析器将收到的Tokens组织起来，并转换成为目标语言语法定义所允许的序列。
>
> 无论是Lexer还是Parser都是一种识别器，Lexer是字符序列识别器而Parser是Token序列识别器。他们在本质上是类似的东西，而只是在分工上有所不同而已。
>
> **语法树**
>
> 代表了语句的结构，其中的每个子树的根节点都使用一个抽象的名字给其包含的元素命名。即子树的根节点对应了语法规则的名字。树的叶子节点是语句中的符号或者词法符号。

- 编写语法文件(hl.g)

    > ```c
    > grammar hl;
    > 
    > options {
    >     language = C;
    >     output = AST;
    >     ASTLabelType = pANTLR3_BASE_TREE;
    > }
    > 
    > @header {
    >     #include <assert.h>
    > }
    > 
    > // The suffix '^' means make it a root.
    > // The suffix '!' means ignore it.
    > 
    > defid
    >     : DEF! ID (','! ID)* ';'!
    >     ;
    > 
    > expr: multExpr ((PLUS^ | MINUS^) multExpr)*
    >     ;
    > 
    > multExpr
    >     : atom (TIMES^ atom)*
    >     ;
    > 
    > atom: INT
    >     | ID
    >     | '('! expr ')'!
    >     ;
    > 
    > or_expr: and_expr (OR^ and_expr)*;
    > and_expr: cond_expr (AND^ cond_expr)*;
    > cond_expr: expr ((EQ^ | NE^ | LITTLE^ | GT^ | LE^ | EL^ | GE^ | EG^) expr)*;
    > 
    > if_expr: IF^ '('! or_expr ')'! expr_stmt;
    > print_expr: PRINT^ or_expr ';'! ;
    > 
    > stmt: expr_stmt
    >     | NEWLINE ->   // ignore
    >     ;
    > 
    > expr_stmt:
    >       or_expr ';' NEWLINE -> or_expr // tree rewrite syntax
    >     | defid NEWLINE -> ^(DEF defid)
    >     | ID ASSIGN expr ';' NEWLINE -> ^(ASSIGN ID expr) // tree notation
    >     | if_expr
    >     | block
    >     | print_expr
    >     ;
    > 
    > code_block:
    >     '{'! (stmt)* '}'!
    >     ;
    > 
    > block:
    >     code_block -> ^(BLOCK code_block)
    >     ;
    > 
    > prog
    >     : (block {
    >             pANTLR3_STRING s = $block.tree->toStringTree($block.tree);
    >             assert(s->chars);
    >             printf("tree \%s\n", s->chars);
    >     })+
    >     ;
    > 
    > PRINT: 'print';
    > EQ : '==';
    > NE : '!=';
    > LITTLE : '<';
    > GT : '>';
    > LE : '<=';
    > EL : '=<';
    > GE : '>=';
    > EG : '=>';
    > OR : '||';
    > AND: '&&';
    > IF: 'if';
    > PLUS: '+';
    > MINUS: '-';
    > TIMES: '*';
    > DOT : ',';
    > ASSIGN: '=';
    > BLOCK: '{}';
    > DEF: 'd' 'e' 'f';
    > ID: ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'0'..'9'|'A'..'Z'|'_')*;
    > INT: '~'? '0'..'9'+ ;
    > NEWLINE: '\r'? '\n' ;
    > WS : (' '|'\t')+ {$channel = HIDDEN;};
    > ```

- 使用语法文件生成语法解析器(hlLexer.c hlLexer.h)和和词法解析器(hlParser.c hlParser.h)

> ```bash
> java -jar ../antlr-3.4-complete.jar ./hl.g
> #生成文件如下
> hl.g  hlLexer.c  hlLexer.h  hlParser.c  hlParser.h  hl.tokens
> ```

## 编写驱动文件

- Main.cpp

    > ```cpp
    > #include "./build/hlLexer.h"
    > #include "./build/hlParser.h"
    > #include <cassert>
    > #include <map>
    > #include <string>
    > #include <iostream>
    > #include <stdexcept>
    > 
    > using std::map;
    > using std::string;
    > using std::cout;
    > using std::runtime_error;
    > 
    > class ExprTreeEvaluator {
    > public:
    >     ExprTreeEvaluator() : father(nullptr) {}
    >     ExprTreeEvaluator(ExprTreeEvaluator *father) : father(father) {}
    >     int run(pANTLR3_BASE_TREE);
    >     int getVal(string name);
    >     int setVal(string name, int val);
    > private :
    >     map<string, int> memory;
    >     ExprTreeEvaluator *father;
    > };
    > 
    > pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
    > const char* getText(pANTLR3_BASE_TREE tree);
    > 
    > int main(int argc, char* argv[]) {
    >     pANTLR3_INPUT_STREAM input;
    >     phlLexer lex;
    >     pANTLR3_COMMON_TOKEN_STREAM tokens;
    >     phlParser parser;
    > 
    >     assert(argc > 1);
    >     input = antlr3FileStreamNew((pANTLR3_UINT8)argv[1],ANTLR3_ENC_8BIT);
    >     lex = hlLexerNew(input);
    > 
    >     tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,TOKENSOURCE(lex));
    >     parser = hlParserNew(tokens);
    >     printf("get parser done!\n");
    >     fflush(stdout);
    >     hlParser_prog_return r = parser->prog(parser);
    >     printf("parser tree is done!\n");
    >     fflush(stdout);
    >     pANTLR3_BASE_TREE tree = r.tree;
    > 
    >     ExprTreeEvaluator eval;
    >     int rr = eval.run(tree);
    > 
    >     parser->free(parser);
    >     tokens->free(tokens);
    >     lex->free(lex);
    >     input->close(input);
    > 
    >     return 0;
    > }
    > 
    > int ExprTreeEvaluator::getVal(string name) {
    >     printf("get memory : %s\n", name.c_str());
    >     ExprTreeEvaluator *p = this;
    >     while (p != nullptr) {
    >         if (p->memory.find(name) != p->memory.end()) {
    >             return p->memory[name];
    >         }
    >         p = p->father;
    >     }
    >     throw runtime_error(name + " is Undeclared");
    >     return -1;
    > }
    > 
    > int ExprTreeEvaluator::setVal(string name, int val) {
    >     printf("set %s = %d\n", name.c_str(), val);
    >     this->memory[name] = val;
    >     return 1;
    > }
    > 
    > int ExprTreeEvaluator::run(pANTLR3_BASE_TREE tree) {
    >     pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    >     if(tok) {
    >         switch(tok->type) {
    >         case INT: {
    >             const char* s = getText(tree);
    >             if(s[0] == '~') {
    >                 return -atoi(s+1);
    >             }
    >             else {
    >                 return atoi(s);
    >             }
    >         }
    >         case ID: {
    >             string var(getText(tree));
    >             return this->getVal(var);
    >         }
    >         case BLOCK: {
    >             ExprTreeEvaluator newExpr(this);
    >             int k = tree->getChildCount(tree);
    >             int r = 0;
    >             for (int i = 0; i < k; i++) {
    >                 r = newExpr.run(getChild(tree, i));
    >             }
    >             cout << "This Block result: " << r << '\n';
    >             return r;
    >         }
    >         case PRINT: {
    >             int ret = run(getChild(tree, 0));
    >             printf("%d\n", ret);
    >         } break;
    >         case EQ: {
    >             int a = run(getChild(tree, 0));
    >             int b = run(getChild(tree, 1));
    >             return a == b;
    >         } break;
    >         case IF: {
    >             int ret = run(getChild(tree, 0));
    >             if (ret) {
    >                 return run(getChild(tree, 1));
    >             }
    >             return 0;
    >         } break;
    >         case PLUS:
    >             return run(getChild(tree,0)) + run(getChild(tree,1));
    >         case MINUS:
    >             return run(getChild(tree,0)) - run(getChild(tree,1));
    >         case TIMES:
    >             return run(getChild(tree,0)) * run(getChild(tree,1));
    >         case DEF: {
    >             int k = tree->getChildCount(tree);
    >             for (int i = 0; i < k; i++) {
    >                 string var(getText(getChild(tree, i)));
    >                 this->setVal(var, 0);
    >             }
    >             return 0;
    >         }
    >         case ASSIGN: {
    >             string var(getText(getChild(tree,0)));
    >             int val = run(getChild(tree,1));
    >             this->setVal(var, val);
    >             return val;
    >         }
    >         default:
    >             cout << "Unhandled token: #" << tok->type << '\n';
    >             return -1;
    >         }
    >     } else {
    >         int k = tree->getChildCount(tree);
    >         int r = 0;
    >         for(int i = 0; i < k; i++) {
    >             r = run(getChild(tree, i));
    >         }
    >         return r;
    >     }
    >     return 0;
    > }
    > 
    > pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i) {
    >     assert(i < tree->getChildCount(tree));
    >     return (pANTLR3_BASE_TREE) tree->getChild(tree, i);
    > }
    > 
    > const char* getText(pANTLR3_BASE_TREE tree) {
    >     return (const char*) tree->getText(tree)->chars;
    > }
    > ```

## C++模式

- 责任链设计模式

> 使用责任链模式对语法树中的字符类型进行处理，确认类型，调用抽象工厂去处理语法表达

- 抽象工厂设计模式

> 对确认的语法进行加工

