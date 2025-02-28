#ifndef __NATIVE_H__
#define __NATIVE_H__
#include <callable/callable.h>
#include <interpreter/interpreter.h>
#include <ctime>

class Interpreter;

class Clock: public MyLangCallable{
public:
    virtual int arity() override;
    virtual MyLang_Object* call(Interpreter* ipreter, std::vector<MyLang_Object*>* args) override;
    virtual MyLang_object_type getType() override;
};

#endif