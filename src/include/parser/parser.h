#ifndef __PARSER_H__
#define __PARSER_H__
#include <vector>
#include <lexer/token.h>
#include <parser/expr.h>
#include <parser/stmt.h>
#include <errors/errors.h>

class Parser{
    std::vector<Token*>* tokens;
    int currentPos;
public:
    Parser(std::vector<Token*>* tokens);
    Token* getTokenAt(int pos);
    myLang::ParseError* error(Token* tk, std::string msg);
    void synchronize();
    void advance();
    Token* previous();
    Token* peek();
    Token* match(enum TokenType ntype);
    Token* consume(enum TokenType ntype, std::string msg);
    Expr* getExpr();
    Expr* getAssign();
    Expr* getEquality();
    Expr* getComparison();
    Expr* getTerm();
    Expr* getFactor();
    Expr* getUnary();
    Expr* getPrimary();
    Expr* getOr();
    Expr* getAnd();
    Expr* getFuncCall();

    Stmt* parseStatement();
    Stmt* parsePrintStatement();
    Stmt* parseExpressionStatement();
    std::vector<Stmt*>* parseBlock();
    Stmt* parseIfStatement();
    Stmt* parseWhileStatement(); 
    Stmt* parseForStatement();
    Stmt* parseReturnStatement();

    Stmt* parseDeclaration();
    Stmt* parseVarDeclaration();
    Stmt* parseFuncDeclaration(std::string kind);
    Stmt* parseClassDeclaration();

    std::vector<Stmt*>* getAST();
};
#endif