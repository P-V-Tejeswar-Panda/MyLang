#ifndef __PARSER_H__
#define __PARSER_H__
#include <vector>
#include <lexer/token.h>
#include <parser/expr.h>
#include <myLang/error.h>
#include <errors/errors.h>

class Parser{
    std::vector<Token*>* tokens;
    int currentPos;
    Parser(std::vector<Token*>* tokens);
    Token* getTokenAt(int pos);
    myLang::ParseError* error(Token* tk, std::string msg);
    void synchronize();
    void advance();
    Token* previous();
    Token* peek();
    Expr* getExpr();
    Expr* getEquality();
    Expr* getComparison();
    Expr* getTerm();
    Expr* getFactor();
    Expr* getUnary();
    Expr* getPrimary();

    Expr* getAST();
};
#endif