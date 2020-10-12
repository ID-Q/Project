/*************************************************************************
	> File Name: main.cpp
	> Author: 
	> Mail: 
	> Created Time: 日 10/11 19:44:03 2020
 ************************************************************************/

#include "hlLexer.h"
#include "hlParser.h"
#include <cassert>
#include <map>
#include <string>
#include <iostream>
#include <stdexcept>
#include "program_master.h"

using std::map;
using std::string;
using std::cout;
using std::runtime_error;

int main(int argc, char* argv[]) {
    pANTLR3_INPUT_STREAM input;
    phlLexer lex;
    pANTLR3_COMMON_TOKEN_STREAM tokens;
    phlParser parser;

    assert(argc > 1);
    input = antlr3FileStreamNew((pANTLR3_UINT8)argv[1],ANTLR3_ENC_8BIT);
    lex = hlLexerNew(input);

    tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
    parser = hlParserNew(tokens);
    hlParser_prog_return r = parser->prog(parser);
    pANTLR3_BASE_TREE tree = r.tree;

    ProgramMaster eval;
    eval.run(tree);

    parser->free(parser);
    tokens->free(tokens);
    lex->free(lex);
    input->close(input);
    return 0;
}
