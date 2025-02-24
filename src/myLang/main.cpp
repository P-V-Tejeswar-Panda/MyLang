#include <lexer/lexer.h>
#include <lexer/token.h>
#include <parser/parser.h>
#include <interpreter/interpreter.h>
#include <myLang/globals.h>
#include <errors/errors.h>
#include <vector>
#include <iostream>

int main(void){
    std::vector<Token*>* tokens = scanAllTokens();
    if(MyLangGlobals::getInstance().hadError == true)
        exit(1);
    Parser* parser = new Parser(tokens);
    std::vector<Stmt*>* stmts = parser->getAST();
    if(MyLangGlobals::getInstance().hadError == true)
        exit(2);
    Interpreter* ipreter = new Interpreter();
    ipreter->interpret(stmts);
}