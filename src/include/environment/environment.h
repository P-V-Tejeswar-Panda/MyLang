#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__
#include <unordered_map>
#include <string>
#include <interpreter/myLang_types.h>
#include <errors/errors.h>


class Environment{
    std::unordered_map<std::string, MyLang_Object*>* values;
    Environment* enclosingEnv;
public:
    Environment();
    Environment(Environment* enclosing);
    void define(std::string, MyLang_Object* obj);
    MyLang_Object* get(Token* tk);
    MyLang_Object* getAt(int distance, std::string name);
    Environment* ancestor(int distance);
    void assign(Token* tk, MyLang_Object* obj);

};
#endif