#ifndef __CALLABLE_H__
#define __CALLABLE_H__
#include <interpreter/myLang_types.h>
//#include <interpreter/interpreter.h>
#include <vector>

class Interpreter;

class MyLangCallable: public MyLang_Object{
    public:
    virtual MyLang_object_type getType() = 0;
    virtual MyLang_Object* call(Interpreter* ipreter, std::vector<MyLang_Object*>* args) = 0;
    virtual int arity() = 0;
};
#endif