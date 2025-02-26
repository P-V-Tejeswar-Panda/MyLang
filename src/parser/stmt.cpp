/*
 * This file is generated by './tools/generate_ast_node.py'.
 * DO NOT EDIT MANUALLY!!, Add your spec to './tools/expr_spec.json' and it will be auto generated.
*/
#include <parser/stmt.h>


Stmt::Stmt(){}


Expression::Expression(Expr* expression){
    this->expression = expression;
}
enum AST_NODE_TYPES Expression::nodeType(){
    return AST_NODE_TYPES::STMT_EXPRESSION;
}
void Expression::accept(StmtVisitor* visitor){
    return visitor->visit(this);
}


Print::Print(Expr* expression){
    this->expression = expression;
}
enum AST_NODE_TYPES Print::nodeType(){
    return AST_NODE_TYPES::STMT_PRINT;
}
void Print::accept(StmtVisitor* visitor){
    return visitor->visit(this);
}


Var::Var(Token* name,Expr* initializer){
    this->name = name;
    this->initializer = initializer;
}
enum AST_NODE_TYPES Var::nodeType(){
    return AST_NODE_TYPES::STMT_VAR;
}
void Var::accept(StmtVisitor* visitor){
    return visitor->visit(this);
}


Block::Block(std::vector<Stmt*>* stmts){
    this->stmts = stmts;
}
enum AST_NODE_TYPES Block::nodeType(){
    return AST_NODE_TYPES::STMT_BLOCK;
}
void Block::accept(StmtVisitor* visitor){
    return visitor->visit(this);
}


If::If(Expr* contition,Stmt* thenBranch,Stmt* elseBranch){
    this->contition = contition;
    this->thenBranch = thenBranch;
    this->elseBranch = elseBranch;
}
enum AST_NODE_TYPES If::nodeType(){
    return AST_NODE_TYPES::STMT_IF;
}
void If::accept(StmtVisitor* visitor){
    return visitor->visit(this);
}


StmtVisitor::StmtVisitor(){}



