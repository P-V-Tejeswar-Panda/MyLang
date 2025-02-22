/*
This expression parser implements A recursive descent parser for
the following grammar.
--------------------------------------------------------------
expression     → equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" ;
--------------------------------------------------------------
*/
#include <parser/parser.h>
#include <lexer/token.h>
#include "parser.h"


Token *Parser::getTokenAt(int pos)
{
    return this->tokens->at(pos);
}

void Parser::advance()
{
    this->currentPos++;
}

Token *Parser::previous()
{
    if(this->currentPos == 0)
        return NULL;
    return this->getTokenAt(this->currentPos-1);
}

Token *Parser::peek()
{
    if(this->currentPos >= this->tokens->size())
        return NULL;
    if(peek()->ttype == TokenType::EOF_)
        return NULL;
    return this->getTokenAt(this->currentPos);
}

Expr *Parser::getExpr()
{
    return getEquality();
}
Expr *Parser::getEquality()
{
    Expr* expr = getComparison();
    Token* t = peek();
    while (t && (t->ttype == TokenType::BANG_EQUAL || t->ttype == TokenType::EQUAL_EQUAL))
    {
        advance();
        Token* op = this->previous();
        Expr* right = getComparison();
        expr = new Binary(expr, op, right);
        t = peek();
    }
    return expr;
}
Expr *Parser::getComparison()
{
    Expr* expr = getTerm();
    Token *t = peek();
    while (t && 
        (t->ttype == TokenType::GREATER ||
         t->ttype == TokenType::GREATER_EQUAL ||
         t->ttype == TokenType::LESS ||
         t->ttype == TokenType::LESS_EQUAL)
    )
    {
        advance();
        Token* op = this->previous();
        Expr* right = getTerm();
        expr = new Binary(expr, op, right);
        t = peek();
    }
    return expr;
}

Expr *Parser::getTerm()
{
    Expr* expr = getFactor();
    Token *t = peek();
    while (t && 
        (t->ttype == TokenType::MINUS ||
         t->ttype == TokenType::PLUS)
    )
    {
        advance();
        Token* op = this->previous();
        Expr* right = getFactor();
        expr = new Binary(expr, op, right);
        t = peek();
    }
    return expr;
}

Expr *Parser::getFactor()
{
    Expr* expr = getUnary();
    Token *t = peek();
    while (t && 
        (t->ttype == TokenType::STAR ||
         t->ttype == TokenType::SLASH)
    )
    {
        advance();
        Token* op = this->previous();
        Expr* right = getUnary();
        expr = new Binary(expr, op, right);
        t = peek();
    }
    return expr;
}

Expr *Parser::getUnary()
{
    Expr* exp = NULL;
    Token* t = peek();
    if(t && (t->ttype == TokenType::BANG ||
             t->ttype == TokenType::MINUS))
    {
        exp = getUnary();
        exp = new Unary(t, exp);
    }
    else{
        exp = getPrimary();
    }
    return exp;
}

Expr *Parser::getPrimary()
{
    Expr* exp = NULL;
    Token* t = peek();
    if(t && t->ttype == TokenType::LEFT_BRACE){
        advance();
        exp = getExpr();
        Token* op = t;
        t = peek();
        if(t->ttype == TokenType::RIGHT_BRACE){
            exp = new Grouping(op, exp, t);
            advance();
        }
        else
            exp = NULL;
    }
    else if (t && (t->ttype == TokenType::NUMBER ||
                   t->ttype == TokenType::STRING ||
                   t->ttype == TokenType::TRUE   ||
                   t->ttype == TokenType::FALSE  ||
                   t->ttype == TokenType::NIL))
    {
        exp = new Literal(t);
    }
    return exp;
}

Expr *Parser::getAST()
{
    return getExpr();
}
