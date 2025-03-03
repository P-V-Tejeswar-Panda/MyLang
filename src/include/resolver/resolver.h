#ifndef __RESOLVER_H__
#define __RESOLVER_H__
#include <interpreter/interpreter.h>
#include <parser/expr.h>
#include <parser/stmt.h>
#include <lexer/token.h>
#include <errors/errors.h>
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>

enum FUNCTION_TYPE{
    NONE, FUNCTION
};

class Resolver: public ExprVisitor, public StmtVisitor{
    Interpreter* ipreter;
    std::vector<std::unordered_map<std::string, bool>*>* scopes;
    enum FUNCTION_TYPE currentFuncType;
public:
    Resolver(Interpreter* ipreter);
    void resolve(Expr* expr);
    void resolve(Stmt* stmt);
    void resolve(std::vector<Stmt*>* stmtList);
    void resolveLocal(Expr* expr, Token* name);
    void resolveFunction(Function* funcDecl, FUNCTION_TYPE ftype);
    void beginScope();
    void endScope();
    void declare(Token* tk);
    void define(Token* tk);
    virtual MyLang_Object* visit(Literal* literal) ;
    virtual MyLang_Object* visit(Logical* logical) ;
    virtual MyLang_Object* visit(Grouping* grouping) ;
    virtual MyLang_Object* visit(Unary* unary) ;
    virtual MyLang_Object* visit(Variable* variable) ;
    virtual MyLang_Object* visit(FuncCall* funcCall) ;
    virtual MyLang_Object* visit(Assign* assign) ;
    virtual MyLang_Object* visit(Binary* binary) ;

    virtual void visit(Print* printStmt) ;
    virtual void visit(Expression* exprStmt) ;
    virtual void visit(Block* blockStmt) ;
    virtual void visit(If* ifStmt) ;
    virtual void visit(Function* funcDecl) ;
    virtual void visit(While* whileStmt) ;
    virtual void visit(Return* returnStmt) ;
    virtual void visit(Var* varStmt) ;

};
#endif