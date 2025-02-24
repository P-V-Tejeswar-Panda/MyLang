/*
 * This file is generated by './tools/generate_ast_node.py'.
 * DO NOT EDIT MANUALLY!!, Add your spec to './tools/expr_spec.json' and it will be auto generated.
*/
#ifndef __EXPR_H__
#define __EXPR_H__
#include <lexer/token.h>
#include <string>
#include <parser/ast_node_types.h>
#include <interpreter/myLang_types.h>

class Expr;

class ExprVisitor{
public:
    ExprVisitor();
    virtual MyLang_Object* visit(Expr* expr) = 0;
};

class Expr{
public:
    Expr();
    MyLang_Object* accept(ExprVisitor* visitor);
    virtual enum AST_NODE_TYPES nodeType() = 0;
};

class Literal: public Expr{
public:
    Token* op;
    Literal(Token* op);
    enum AST_NODE_TYPES nodeType();
};

class Grouping: public Expr{
public:
    Token* op;
    Expr* expr;
    Token* cp;
    Grouping(Token* op,Expr* expr,Token* cp);
    enum AST_NODE_TYPES nodeType();
};

class Unary: public Expr{
public:
    Token* token;
    Expr* expr;
    Unary(Token* token,Expr* expr);
    enum AST_NODE_TYPES nodeType();
};

class Binary: public Expr{
public:
    Expr* left;
    Token* op;
    Expr* right;
    Binary(Expr* left,Token* op,Expr* right);
    enum AST_NODE_TYPES nodeType();
};

#endif
