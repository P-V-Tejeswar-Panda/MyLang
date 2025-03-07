#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <interpreter/myLang_types.h>
#include <parser/expr.h>
#include <parser/stmt.h>
#include <lexer/token.h>
#include <errors/errors.h>
#include <environment/environment.h>
#include <callable/callable.h>
#include <callable/native.h>
#include <callable/userdefined.h>
#include <string>
#include <vector>

class Interpreter: public ExprVisitor, public StmtVisitor{
public:
    Environment* env;
    Environment* globals;
    std::unordered_map<Expr*, int>* scope_map;
    virtual MyLang_Object* visit(Literal* literal);
    virtual MyLang_Object* visit(Grouping* grouping);
    virtual MyLang_Object* visit(Unary* unary);
    virtual MyLang_Object* visit(Binary* binary);
    virtual MyLang_Object* visit(Variable* variable);
    virtual MyLang_Object* visit(Assign* assign);
    virtual MyLang_Object* visit(Logical* logical);
    virtual MyLang_Object* visit(FuncCall* funcCall);
    virtual MyLang_Object* visit(Get* instGet);
    virtual MyLang_Object* visit(Set* instSet);
    virtual MyLang_Object* visit(This* keyword);
    virtual MyLang_Object* visit(Super* superkey);
    virtual void visit(Print* printStmt);
    virtual void visit(Expression* exprStmt);
    virtual void visit(Var* varStmt);
    virtual void visit(Block* blkStmt);
    virtual void visit(If* ifStmt);
    virtual void visit(While* whileStmt);
    virtual void visit(Function* funcDecl);
    virtual void visit(Class* classDecl);
    virtual void visit(Return* retStmt);
    virtual void interpret(std::vector<Stmt*>* stmts);
    virtual void execute(Stmt* stmt);
    virtual void executeBlock(std::vector<Stmt*>* stmts, Environment* env);
    virtual MyLang_Object* evaluate(Expr* expr);
    virtual bool  isTruthy(MyLang_Object* obj);
    virtual bool  isEqual(MyLang_Object* l, MyLang_Object* r);
    virtual void checkNumberOperand(Token* op, MyLang_Object* operand);
    virtual void checkNumberOperands(Token* op, MyLang_Object* l_operand, MyLang_Object* r_operand);
    virtual std::string stringify(MyLang_Object* obj);
    void resolve(Expr* expr,int depth);
    MyLang_Object* lookUpVariable(Token* name, Expr* exp);
    Interpreter();
};
#endif