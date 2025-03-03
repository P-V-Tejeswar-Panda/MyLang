#include <environment/environment.h>

Environment::Environment()
{
    this->values = new std::unordered_map<std::string, MyLang_Object*>();
    this->enclosingEnv = NULL;
}

Environment::Environment(Environment *enclosing)
{
    this->values = new std::unordered_map<std::string, MyLang_Object*>();
    this->enclosingEnv = enclosing;
}
void Environment::define(std::string name, MyLang_Object *obj)
{
    (*(this->values))[name] = obj;
}

MyLang_Object *Environment::get(Token* tk)
{
    if(this->values->find(tk->lexeme) != this->values->end())
        return (*(this->values))[tk->lexeme];
    if(this->enclosingEnv)
        return this->enclosingEnv->get(tk);
    throw myLang::RuntimeError(tk, "Undefined variable '"+tk->lexeme+"'.");
}

MyLang_Object *Environment::getAt(int distance, std::string name)
{
    return (*(ancestor(distance)->values))[name];
}
Environment *Environment::ancestor(int distance)
{
    Environment* currEnv = this;
    for(int i = 0; i < distance; i++){
        currEnv = currEnv->enclosingEnv;
    }
    return currEnv;
}
void Environment::assign(Token *tk, MyLang_Object *obj)
{
    if(this->values->find(tk->lexeme) != this->values->end()){
        (*(this->values))[tk->lexeme] = obj;
        return;
    }
    if(this->enclosingEnv)
        return this->enclosingEnv->assign(tk, obj);
    throw myLang::RuntimeError(tk, "Undefined variable '"+tk->lexeme+"'.");
}
