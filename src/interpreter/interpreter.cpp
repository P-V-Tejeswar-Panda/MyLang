#include <interpreter/interpreter.h>



// MyLang_Object *Interpreter::visit(Expr *expr)
// {
//     return nullptr;
// }

MyLang_Object *Interpreter::visit(Literal *literal)
{
    switch (literal->op->ttype){
        case TokenType::NUMBER:
            return new MyLang_Double(std::stod(literal->op->lexeme));
        case TokenType::STRING:
            return new MyLang_String(literal->op->lexeme);
        case TokenType::TRUE:
            return new MyLang_Boolean(true);
        case TokenType::FALSE:
            return new MyLang_Boolean(false);
        case TokenType::NIL:
            return NULL;
        default:
            break;
    }
    return nullptr;
}

MyLang_Object *Interpreter::visit(Grouping *grouping)
{
    return evaluate(grouping->expr);
}

MyLang_Object *Interpreter::visit(Unary *unary)
{
    MyLang_Object* obj = evaluate(unary->expr);
    switch(unary->token->ttype){
        case TokenType::MINUS:
            {
                checkNumberOperand(unary->token, obj);
                MyLang_Double* d = (MyLang_Double*)obj;
                return new MyLang_Double(-1*d->value);
            }
        case TokenType::BANG:
            return new MyLang_Boolean(!isTruthy(obj));
    }
    return NULL;
}

MyLang_Object *Interpreter::visit(Binary *binary)
{
    MyLang_Object* left  = evaluate(binary->left);
    MyLang_Object* right = evaluate(binary->right);
    MyLang_Double* l = (MyLang_Double*)left;
    MyLang_Double* r = (MyLang_Double*)right;
    switch(binary->op->ttype){
        case TokenType::MINUS:
            checkNumberOperands(binary->op, left, right);
            return new MyLang_Double(l->value-r->value);
        case TokenType::PLUS:
            if(left->getType() == MyLang_object_type::MYLANG_DOUBLE &&
               right->getType() == MyLang_object_type:: MYLANG_DOUBLE)
            {
                return new MyLang_Double(l->value+r->value);
            }
            if(left->getType() == MyLang_object_type::MYLANG_STRING &&
            right->getType() == MyLang_object_type:: MYLANG_STRING)
            {
                MyLang_String* l_str = (MyLang_String*)left;
                MyLang_String* r_str = (MyLang_String*)right;
                return new MyLang_String(l_str->value+r_str->value);
            }
            throw new myLang::RuntimeError(binary->op, "Operands must be two numbers or two strings.");
        case TokenType::STAR:
            checkNumberOperands(binary->op, left, right);
            return new MyLang_Double(l->value*r->value);
        case TokenType::SLASH:
            checkNumberOperands(binary->op, left, right);
            return new MyLang_Double(l->value/r->value);
        case TokenType::GREATER:
            checkNumberOperands(binary->op, left, right);
            return new MyLang_Boolean(l->value > r->value);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(binary->op, left, right);
            return new MyLang_Boolean(l->value >= r->value);
        case TokenType::LESS:
            checkNumberOperands(binary->op, left, right);
            return new MyLang_Boolean(l->value < r->value);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(binary->op, left, right);
            return new MyLang_Boolean(l->value <= r->value);
        case TokenType::EQUAL_EQUAL:
            return new MyLang_Boolean(isEqual(left, right));
        case TokenType::BANG_EQUAL:
            return new MyLang_Boolean(!isEqual(left, right));
    }

    return NULL;
}

MyLang_Object *Interpreter::visit(Variable *variable)
{
    return lookUpVariable(variable->name, variable);
}

MyLang_Object *Interpreter::visit(Assign *assign)
{
    MyLang_Object* val = evaluate(assign->value);
    env->assign(assign->name, val);
    return val;
}
MyLang_Object *Interpreter::visit(Logical *logical)
{
    MyLang_Object* left = evaluate(logical->left);
    if(logical->op->ttype == TokenType::OR){
        if(isTruthy(left)) return left;
    }else{
        if(!isTruthy(left)) return left;
    }
    return evaluate(logical->right);
}
MyLang_Object *Interpreter::visit(FuncCall *funcCall)
{
    MyLang_Object* callee = evaluate(funcCall->callee);
    std::vector<MyLang_Object*>* args = new std::vector<MyLang_Object*>();
    for(Expr* exp : *(funcCall->args))
        args->push_back(evaluate(exp));
    if(callee->getType() != MyLang_object_type::MYLANG_CLASS &&
       callee->getType() != MyLang_object_type::MYLANG_FUNCTION)
        throw new myLang::RuntimeError(funcCall->paren,
                                        "Can only call functions and classes.");
    MyLangCallable* func = (MyLangCallable*) callee;
    if(args->size() != func->arity())
        throw new myLang::RuntimeError(funcCall->paren,
            "Expected: "+ std::to_string(func->arity())
            +" arguments but got "+ std::to_string(args->size()));
    return func->call(this, args);
}
MyLang_Object *Interpreter::visit(Get *instGet)
{
    MyLang_Object* obj = evaluate(instGet->instObject);
    if(obj->getType() == MYLANG_INSTANCE){
        return ((UserDefinedClassInstance*)obj)->get(instGet->name);
    }
    throw new myLang::RuntimeError(instGet->name,
                                    "Only instances have properties.");
}
MyLang_Object *Interpreter::visit(Set *instSet)
{
    MyLang_Object* obj = evaluate(instSet->instObject);
    if(obj->getType() != MyLang_object_type::MYLANG_INSTANCE)
        throw new myLang::RuntimeError(instSet->name,
                    "Only instances have fields");
    MyLang_Object* val = evaluate(instSet->value);
    ((UserDefinedClassInstance*)obj)->set(instSet->name, val);
    return val;
}
MyLang_Object *Interpreter::visit(This *keyword)
{
    return lookUpVariable(keyword->keyword, keyword);
}
MyLang_Object *Interpreter::visit(Super *superkey)
{
    int dist = (*(this->scope_map))[superkey];
    UserDefinedClass* supClass = (UserDefinedClass*)this->env->getAt(dist, "super");
    UserDefinedClassInstance* instance = (UserDefinedClassInstance*) this->env->getAt(dist-1, "this");
    UserDefinedFunc* method = (UserDefinedFunc*)supClass->getMethod(superkey->method->lexeme);
    if(!method){
        throw new myLang::RuntimeError(superkey->method, 
                        "Undefined property '"+superkey->method->lexeme+"'.");
    }
    return method->bind(instance);
}
void Interpreter::visit(Print *printStmt)
{
    MyLang_Object* val = evaluate(printStmt->expression);
    std::cout << stringify(val) << std::endl;
}
void Interpreter::visit(Expression *exprStmt)
{
    evaluate(exprStmt->expression);
}
void Interpreter::visit(Var *varStmt)
{
    MyLang_Object* value = NULL;
    if(varStmt->initializer != NULL)
        value = evaluate(varStmt->initializer);
    env->define(varStmt->name->lexeme, value);
}
void Interpreter::visit(Block *blkStmt)
{
    executeBlock(blkStmt->stmts, new Environment(this->env));
}
void Interpreter::visit(If *ifStmt)
{
    if(isTruthy(evaluate(ifStmt->contition))){
        execute(ifStmt->thenBranch);
    }
    else{
        if(ifStmt->elseBranch)
            execute(ifStmt->elseBranch);
    }
}
void Interpreter::visit(While *whileStmt)
{
    while(isTruthy(evaluate(whileStmt->contition)))
        execute(whileStmt->whileBody);
}
void Interpreter::visit(Function *funcDecl)
{
    UserDefinedFunc *uFunc = new UserDefinedFunc(funcDecl, this->env, false);
    env->define(funcDecl->name->lexeme, uFunc);
}
void Interpreter::visit(Class *classDecl)
{
    MyLang_Object* supClass = NULL;
    if(classDecl->superclass != NULL){
        supClass = evaluate(classDecl->superclass);
        if(supClass->getType() != MyLang_object_type::MYLANG_CLASS){
            throw new myLang::RuntimeError(classDecl->superclass->name,
                            "Superclass must be a class.");
        }
    }
    this->env->define(classDecl->name->lexeme, NULL);
    if(classDecl->superclass){
        this->env = new Environment(this->env);
        this->env->define("super", supClass);
    }
    std::unordered_map<std::string, UserDefinedFunc*>* methods = 
                        new std::unordered_map<std::string, UserDefinedFunc*>();
    for(Function* fn: *classDecl->methods){
        UserDefinedFunc* ufunc = new UserDefinedFunc(fn, this->env, (fn->name->lexeme == "init")?true:false);
        (*methods)[fn->name->lexeme] = ufunc;
    }
    UserDefinedClass* cls = new UserDefinedClass(classDecl->name->lexeme,
                                                        (UserDefinedClass*)supClass, methods);
    if(supClass){
        this->env = this->env->enclosingEnv;
    }
    this->env->assign(classDecl->name, cls);
}
void Interpreter::visit(Return *retStmt)
{
    MyLang_Object* obj = NULL;
    if(retStmt->exp != NULL)
        obj = evaluate(retStmt->exp);
    throw new myLang::ReturnExp(obj);
}
void Interpreter::interpret(std::vector<Stmt *> *stmts)
{
    try{
        for(Stmt* stmt: (*stmts))
            execute(stmt);
    } catch (myLang::RuntimeError* err){
        myLang::communicateRuntimeError(err);
    }
}

void Interpreter::execute(Stmt *stmt)
{
    if(stmt->nodeType() == AST_NODE_TYPES::STMT_PRINT)
        ((Print*)stmt)->accept(this);
    if(stmt->nodeType() == AST_NODE_TYPES::STMT_EXPRESSION)
        ((Expression*)stmt)->accept(this);
    if(stmt->nodeType() == AST_NODE_TYPES::STMT_VAR)
        ((Var*)stmt)->accept(this);
    if(stmt->nodeType() == AST_NODE_TYPES::STMT_BLOCK)
        ((Block*)stmt)->accept(this);
    if(stmt->nodeType() == AST_NODE_TYPES::STMT_IF)
        ((If*)stmt)->accept(this);
    if(stmt->nodeType() == AST_NODE_TYPES::STMT_WHILE)
        ((While*)stmt)->accept(this);
    if(stmt->nodeType() == AST_NODE_TYPES::DEFN_FUNC)
        ((Function*)stmt)->accept(this);
    if(stmt->nodeType() == AST_NODE_TYPES::DEFN_CLASS)
        ((Class*)stmt)->accept(this);
    if(stmt->nodeType() == AST_NODE_TYPES::STMT_RET)
        ((Return*)stmt)->accept(this);
    
}
void Interpreter::executeBlock(std::vector<Stmt *> *stmts, Environment *env)
{
    Environment* prev = this->env;
    try{
        this->env = env;
        for(Stmt* stmt: (*stmts)){
            execute(stmt);
        }
    }
    catch (myLang::ReturnExp* re){
        this->env = prev;
        throw re;
    }
    catch (myLang::RuntimeError* re){
        this->env = prev;
        throw re;
    }
    this->env = prev;
}
MyLang_Object *Interpreter::evaluate(Expr *expr)
{
    switch (expr->nodeType()){
        case AST_NODE_TYPES::BINARY:
            return ((Binary*)expr)->accept(this);
        case AST_NODE_TYPES::GROUPING:
            return ((Grouping*)expr)->accept(this);
        case AST_NODE_TYPES::LITERAL:
            return ((Literal*)expr)->accept(this);
        case AST_NODE_TYPES::EXPR_LOGICAL:
            return ((Logical*)expr)->accept(this);
        case AST_NODE_TYPES::UNARY:
            return ((Unary*)expr)->accept(this);
        case AST_NODE_TYPES::VARIABLE:
            return ((Variable*)expr)->accept(this);
        case AST_NODE_TYPES::ASSIGN:
            return ((Assign*)expr)->accept(this);
        case AST_NODE_TYPES::EXPR_FUNC_CALL:
            return ((FuncCall*)expr)->accept(this);
        case AST_NODE_TYPES::EXPR_INST_GET:
            return ((Get*)expr)->accept(this);
        case AST_NODE_TYPES::EXPR_INST_SET:
            return ((Set*)expr)->accept(this);
        case AST_NODE_TYPES::EXPR_THIS:
            return ((This*)expr)->accept(this);
        case AST_NODE_TYPES::EXPR_SUPER:
            return ((Super*)expr)->accept(this);
    }
    return NULL;
}

bool Interpreter::isTruthy(MyLang_Object *obj)
{
    if(obj == NULL) return false;
    if(obj->getType() == MyLang_object_type::MYLANG_BOOLEAN){
        MyLang_Boolean* b = (MyLang_Boolean*)obj;
        return b->value;
    }
    return true;
}

bool Interpreter::isEqual(MyLang_Object *l, MyLang_Object *r)
{
    if(l == NULL && r == NULL)
        return true;
    if(l == NULL)
        return false;

    if(l->getType() == MyLang_object_type::MYLANG_BOOLEAN){
        bool l_val = ((MyLang_Boolean*)l)->value;
        if(r->getType() == MyLang_object_type::MYLANG_BOOLEAN)
            return l_val == ((MyLang_Boolean*)r)->value;
        if(r->getType() == MyLang_object_type::MYLANG_DOUBLE)
            return l_val == ((MyLang_Double*)r)->value;
        if(r->getType() == MyLang_object_type::MYLANG_STRING)
            return false;
    }
    if(l->getType() == MyLang_object_type::MYLANG_DOUBLE){
        double l_val = ((MyLang_Double*)l)->value;
        if(r->getType() == MyLang_object_type::MYLANG_BOOLEAN)
            return l_val == ((MyLang_Boolean*)r)->value;
        if(r->getType() == MyLang_object_type::MYLANG_DOUBLE)
            return l_val == ((MyLang_Double*)r)->value;
        if(r->getType() == MyLang_object_type::MYLANG_STRING)
            return false;
    }
    if(l->getType() == MyLang_object_type::MYLANG_STRING){
        std::string l_val = ((MyLang_String*)l)->value;
        if(r->getType() == MyLang_object_type::MYLANG_STRING)
            return l_val == ((MyLang_String*)r)->value;
        else return false;
    }
    return false;
}

void Interpreter::checkNumberOperand(Token *op, MyLang_Object *operand)
{
    if(operand->getType() == MyLang_object_type::MYLANG_DOUBLE) return;
    throw new myLang::RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(Token *op, MyLang_Object *l_operand, MyLang_Object *r_operand)
{
    if(l_operand->getType() == MyLang_object_type::MYLANG_DOUBLE &&
       r_operand->getType() == MyLang_object_type::MYLANG_DOUBLE) return;
    throw new myLang::RuntimeError(op, "Operands must be numbers.");
}

std::string Interpreter::stringify(MyLang_Object *obj)
{
    if(obj == NULL) return "nil";
    if(obj->getType() == MyLang_object_type::MYLANG_DOUBLE){
        std::string s = std::to_string(((MyLang_Double*)obj)->value);
        return s;
    }
    if(obj->getType() == MyLang_object_type::MYLANG_STRING){
        return ((MyLang_String*)obj)->value;
    }
    if(obj->getType() == MyLang_object_type::MYLANG_BOOLEAN){
        bool v = ((MyLang_Boolean*)obj)->value;
        if(v){
            return "true";
        }
        return "false";
    }
    if(obj->getType() == MyLang_object_type::MYLANG_FUNCTION)
        return ((UserDefinedFunc*)obj)->toString();
    if(obj->getType() == MyLang_object_type::MYLANG_CLASS)
        return ((UserDefinedClass*)obj)->toString();
    if(obj->getType() == MyLang_object_type::MYLANG_INSTANCE)
        return ((UserDefinedClassInstance*)obj)->toString();
    return "Error";
}

void Interpreter::resolve(Expr *expr, int depth)
{
    (*(this->scope_map))[expr] = depth;
}
MyLang_Object* Interpreter::lookUpVariable(Token *name, Expr *exp)
{
    if(this->scope_map->find(exp) != this->scope_map->end()){
        int dist = (*this->scope_map)[exp];
        return this->env->getAt(dist, name->lexeme);
    }
    else{
        return this->globals->get(name);
    }
}
Interpreter::Interpreter()
{
    this->globals = new Environment();
    this->env = globals;
    this->scope_map = new std::unordered_map<Expr*, int>();
    this->globals->define("clock", new Clock());
}