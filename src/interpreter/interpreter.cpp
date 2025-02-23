#include <interpreter/interpreter.h>



MyLang_Object *Interpreter::visit(Expr *expr)
{
    return nullptr;
}

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
            checkNumberOperand(unary->token, obj);
            MyLang_Double* d = (MyLang_Double*)obj;
            return new MyLang_Double(-1*d->value);
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

void Interpreter::interpret(Expr *expression)
{
    try{
        MyLang_Object* val = evaluate(expression);
        std::cout << stringify(val) << std::endl;
    } catch (myLang::RuntimeError* err){
        myLang::communicateRuntimeError(err);
    }
}

MyLang_Object *Interpreter::evaluate(Expr* expr)
{
    return expr->accept(this);
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
    return "Error";
}
