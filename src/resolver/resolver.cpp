#include <resolver/resolver.h>



Resolver::Resolver(Interpreter *ipreter)
{
    this->ipreter = ipreter;
    this->scopes = new std::vector<std::unordered_map<std::string, bool>*>();
    this->currentFuncType = FUNCTION_TYPE::F_NONE;
    this->currentClassType = CLASS_TYPE::C_NONE;
}

void Resolver::resolve(Expr *expr)
{
    switch (expr->nodeType()){
        case AST_NODE_TYPES::BINARY:
            ((Binary*)expr)->accept(this); break;
        case AST_NODE_TYPES::GROUPING:
            ((Grouping*)expr)->accept(this); break;
        case AST_NODE_TYPES::LITERAL:
            ((Literal*)expr)->accept(this); break;
        case AST_NODE_TYPES::EXPR_LOGICAL:
            ((Logical*)expr)->accept(this); break;
        case AST_NODE_TYPES::UNARY:
            ((Unary*)expr)->accept(this); break;
        case AST_NODE_TYPES::VARIABLE:
            ((Variable*)expr)->accept(this); break;
        case AST_NODE_TYPES::ASSIGN:
            ((Assign*)expr)->accept(this); break;
        case AST_NODE_TYPES::EXPR_FUNC_CALL:
            ((FuncCall*)expr)->accept(this); break;
        case AST_NODE_TYPES::EXPR_INST_GET:
            ((Get*)expr)->accept(this); break;
        case AST_NODE_TYPES::EXPR_INST_SET:
            ((Set*)expr)->accept(this); break;
        case AST_NODE_TYPES::EXPR_THIS:
            ((This*)expr)->accept(this); break;
        case AST_NODE_TYPES::EXPR_SUPER:
            ((Super*)expr)->accept(this); break;
    }
}

void Resolver::resolve(Stmt *stmt)
{
    switch (stmt->nodeType()){
        case AST_NODE_TYPES::STMT_PRINT:
            ((Print*)stmt)->accept(this); break;
        case AST_NODE_TYPES::STMT_EXPRESSION:
            ((Expression*)stmt)->accept(this); break;
        case AST_NODE_TYPES::STMT_VAR:
            ((Var*)stmt)->accept(this); break;
        case AST_NODE_TYPES::STMT_BLOCK:
            ((Block*)stmt)->accept(this); break;
        case AST_NODE_TYPES::STMT_IF:
            ((If*)stmt)->accept(this); break;
        case AST_NODE_TYPES::STMT_WHILE:
            ((While*)stmt)->accept(this); break;
        case AST_NODE_TYPES::DEFN_FUNC:
            ((Function*)stmt)->accept(this); break;
        case AST_NODE_TYPES::DEFN_CLASS:
            ((Class*)stmt)->accept(this); break;
        case AST_NODE_TYPES::STMT_RET:
            ((Return*)stmt)->accept(this); break;
    }
}

void Resolver::resolve(std::vector<Stmt *>* stmtList)
{
    for(Stmt* stmt: (*stmtList))
        resolve(stmt);
}

void Resolver::resolveLocal(Expr *expr, Token *name)
{
    for(int i = scopes->size()-1; i >= 0; i--){
        if((*scopes)[i]->find(name->lexeme) != (*scopes)[i]->end()){
            ipreter->resolve(expr, scopes->size()-1-i);
            return;
        }
    }
}

void Resolver::resolveFunction(Function *funcDecl, FUNCTION_TYPE ftype)
{
    beginScope();
    FUNCTION_TYPE enclosingFunction = this->currentFuncType;
    this->currentFuncType = ftype;
    for(Token* param: *(funcDecl->params)){
        declare(param);
        define(param);
    }
    resolve(funcDecl->body);
    this->currentFuncType = enclosingFunction;
    endScope();
}

void Resolver::beginScope()
{
    this->scopes->push_back(new std::unordered_map<std::string, bool>());
}
void Resolver::endScope()
{
    auto mp = scopes->back();
    scopes->pop_back();
    delete mp;
}
void Resolver::declare(Token *tk)
{
    if(scopes->empty())
        return;
    if(scopes->back()->find(tk->lexeme) != scopes->back()->end())
        myLang::communicateError(tk, "Redeclaring '"+tk->lexeme+"' in the same scope.");
    scopes->back()->insert(std::make_pair(tk->lexeme, false));
}
void Resolver::define(Token *tk)
{
    if(scopes->empty())
        return;
    (*(scopes->back()))[tk->lexeme] = true;
}
MyLang_Object *Resolver::visit(Literal *literal)
{
    return NULL;
}

MyLang_Object *Resolver::visit(Logical *logical)
{
    resolve(logical->left);
    resolve(logical->right);
    return NULL;
}

MyLang_Object *Resolver::visit(Grouping *grouping)
{
    resolve(grouping->expr);
    return NULL;
}

MyLang_Object *Resolver::visit(Unary *unary)
{
    resolve(unary->expr);
    return NULL;
}

MyLang_Object *Resolver::visit(Variable *variable)
{
    if(!scopes->empty() && scopes->back()->find(variable->name->lexeme) != scopes->back()->end() &&
        (*scopes->back())[variable->name->lexeme] == false)
    {
        myLang::communicateError(variable->name,
            "Cannot read local variable in its own initializer.");
    }
    resolveLocal(variable, variable->name);
    return NULL;
}

MyLang_Object *Resolver::visit(FuncCall *funcCall)
{
    resolve(funcCall->callee);
    for(Expr* arg: *(funcCall->args))
        resolve(arg);
    return NULL;
}

MyLang_Object *Resolver::visit(Assign *assign)
{
    resolve(assign->value);
    resolveLocal(assign, assign->name);
    return nullptr;
}

MyLang_Object *Resolver::visit(Binary *binary)
{
    resolve(binary->left);
    resolve(binary->right);
    return NULL;
}

MyLang_Object *Resolver::visit(Get *instGet)
{
    resolve(instGet->instObject);
    return nullptr;
}
MyLang_Object *Resolver::visit(Set *instSet)
{
    resolve(instSet->value);
    resolve(instSet->instObject);
    return NULL;
}
MyLang_Object *Resolver::visit(This *keyword)
{
    if(currentClassType == CLASS_TYPE::C_NONE){
        myLang::communicateError(keyword->keyword, "Can't use 'this' outside of a class.");
        return NULL;
    }
    resolveLocal(keyword, keyword->keyword);
    return nullptr;
}
MyLang_Object *Resolver::visit(Super *superkey)
{
    if(this->currentClassType == CLASS_TYPE::C_NONE){
        myLang::communicateError(superkey->keyword, "Can't use 'super' outside of a class.");
    }
    else if(this->currentClassType == CLASS_TYPE::BASE_CLASS){
        myLang::communicateError(superkey->keyword, "Can't use 'super' in a base class.");
    }
    resolveLocal(superkey, superkey->keyword);
    return nullptr;
}
void Resolver::visit(Print *printStmt)
{
    resolve(printStmt->expression);
}

void Resolver::visit(Expression *exprStmt)
{
    resolve(exprStmt->expression);
}

void Resolver::visit(Block *blockStmt)
{
    beginScope();
    resolve(blockStmt->stmts);
    endScope();
}

void Resolver::visit(If *ifStmt)
{
    resolve(ifStmt->contition);
    resolve(ifStmt->thenBranch);
    if(ifStmt->elseBranch) resolve(ifStmt->elseBranch);
}

void Resolver::visit(Function *funcDecl)
{
    declare(funcDecl->name);
    define(funcDecl->name);

    resolveFunction(funcDecl, FUNCTION_TYPE::FUNCTION);
}

void Resolver::visit(Class *classDecl)
{
    CLASS_TYPE bkpClassType = this->currentClassType;
    this->currentClassType = CLASS_TYPE::BASE_CLASS;
    declare(classDecl->name);
    define(classDecl->name);

    if(classDecl->superclass &&
        classDecl->name->lexeme == classDecl->superclass->name->lexeme)
    {
        myLang::communicateError(classDecl->superclass->name,
                "A class cannot inherit from itself.");
    }

    if(classDecl->superclass){
        this->currentClassType = CLASS_TYPE::SUB_CLASS;
        resolve(classDecl->superclass);
    }
    if(classDecl->superclass){
        beginScope();
        (*(scopes->back()))["super"] = true;
    }
    beginScope();
    scopes->back()->insert(std::make_pair("this", true));

    for(Function* meth: *classDecl->methods){
        FUNCTION_TYPE ftype = FUNCTION_TYPE::METHOD;
        if(meth->name->lexeme == "init")
            ftype = FUNCTION_TYPE::INITIALIZER;
        resolveFunction(meth, ftype);
    }

    endScope();
    if(classDecl->superclass)
        endScope();
    this->currentClassType = bkpClassType;
}
void Resolver::visit(While *whileStmt)
{
    resolve(whileStmt->contition);
    resolve(whileStmt->whileBody);
}

void Resolver::visit(Return *returnStmt)
{
    if(this->currentFuncType == FUNCTION_TYPE::F_NONE)
        myLang::communicateError(returnStmt->ret_token,
                            "Cannot return from top-level code");
    if(returnStmt->exp){
        if(this->currentFuncType == FUNCTION_TYPE::INITIALIZER)
            myLang::communicateError(returnStmt->ret_token,
                                            "can't return value from initializer.");
        resolve(returnStmt->exp);
    }
}

void Resolver::visit(Var *varStmt)
{
    declare(varStmt->name);
    if(varStmt->initializer)
        resolve(varStmt->initializer);
    define(varStmt->name);
}
