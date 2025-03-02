#ifndef __USERDEFINED_H__
#define __USERDEFINED_H__
#include <callable/callable.h>
#include <interpreter/interpreter.h>
#include <parser/stmt.h>
#include <environment/environment.h>
#include <errors/errors.h>


class UserDefinedFunc: public MyLangCallable{
    Function* funcDefn;
    Environment* closure;
public:
    UserDefinedFunc(Function* func, Environment* closure);
    virtual int arity() override;
    virtual MyLang_Object* call(Interpreter* ipreter, std::vector<MyLang_Object*>* args) override;
    virtual MyLang_object_type getType() override;
};
#endif