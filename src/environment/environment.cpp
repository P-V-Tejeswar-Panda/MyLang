#include <environment/environment.h>

Environment::Environment()
{
    this->values = new std::unordered_map<std::string, MyLang_Object*>();
}

void Environment::define(std::string name, MyLang_Object *obj)
{
    (*(this->values))[name] = obj;
}

MyLang_Object *Environment::get(Token* tk)
{
    if(this->values->find(tk->lexeme) != this->values->end())
        return (*(this->values))[tk->lexeme];
    throw myLang::RuntimeError(tk, "Undefined variable '"+tk->lexeme+"'.");
}
