#ifndef __USERDEFINED_H__
#define __USERDEFINED_H__
#include <callable/callable.h>
#include <interpreter/interpreter.h>
#include <parser/stmt.h>
#include <environment/environment.h>
#include <errors/errors.h>
#include <interpreter/myLang_types.h>
#include <string>


class UserDefinedClassInstance;

class UserDefinedFunc: public MyLangCallable{
    Function* funcDefn;
    Environment* closure;
    bool isInitializer;
public:
    UserDefinedFunc(Function* func, Environment* closure, bool isInit);
    UserDefinedFunc* bind(UserDefinedClassInstance* inst);
    virtual int arity() override;
    virtual MyLang_Object* call(Interpreter* ipreter, std::vector<MyLang_Object*>* args) override;
    virtual MyLang_object_type getType() override;
};

class UserDefinedClass: public MyLangCallable{
    std::string clsName;
    std::unordered_map<std::string, UserDefinedFunc*>* methods;
public:
    UserDefinedClass(std::string clsName, std::unordered_map<std::string, UserDefinedFunc*>* methods);
    virtual int arity() override;
    virtual MyLang_Object* call(Interpreter* ipreter, std::vector<MyLang_Object*>* args) override;
    virtual MyLang_object_type getType() override;
    virtual std::string toString();
    virtual UserDefinedFunc* getMethod(std::string name);
};

class UserDefinedClassInstance: public MyLang_Object{
    UserDefinedClass* cls;
    std::unordered_map<std::string, MyLang_Object*>* fields =
                                new std::unordered_map<std::string, MyLang_Object*>();
    
public:
    UserDefinedClassInstance(UserDefinedClass* cls);
    virtual std::string toString();
    virtual MyLang_Object* get(Token* name);
    virtual void set(Token* name, MyLang_Object* value);
};
#endif