#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <interpreter/myLang_types.h>
#include <parser/expr.h>
#include <parser/stmt.h>
#include <lexer/token.h>
#include <errors/errors.h>
#include <string>
#include <vector>

class Interpreter: public ExprVisitor, public StmtVisitor{
public:
    //virtual MyLang_Object* visit(Expr* expr);
    virtual MyLang_Object* visit(Literal* literal);
    virtual MyLang_Object* visit(Grouping* grouping);
    virtual MyLang_Object* visit(Unary* unary);
    virtual MyLang_Object* visit(Binary* binary);
    virtual void visit(Print* printStmt);
    virtual void visit(Expression* exprStmt);
    virtual void interpret(std::vector<Stmt*>* stmts);
    virtual void execute(Stmt* stmt);
    virtual MyLang_Object* evaluate(Expr* expr);
    virtual bool  isTruthy(MyLang_Object* obj);
    virtual bool  isEqual(MyLang_Object* l, MyLang_Object* r);
    virtual void checkNumberOperand(Token* op, MyLang_Object* operand);
    virtual void checkNumberOperands(Token* op, MyLang_Object* l_operand, MyLang_Object* r_operand);
    virtual std::string stringify(MyLang_Object* obj);
};
#endif