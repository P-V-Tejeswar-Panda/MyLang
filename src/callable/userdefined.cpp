#include <callable/userdefined.h>


UserDefinedFunc::UserDefinedFunc(Function *func, Environment* closure, bool isInit)
{
    funcDefn = func;
    this->closure = closure;
    this->isInitializer = isInit;
}
UserDefinedFunc *UserDefinedFunc::bind(UserDefinedClassInstance *inst)
{
    Environment* env = new Environment(this->closure);
    env->define("this", inst);
    return new UserDefinedFunc(this->funcDefn, env, isInitializer);
}
int UserDefinedFunc::arity()
{
    return funcDefn->params->size();
}

MyLang_Object *UserDefinedFunc::call(Interpreter *ipreter, std::vector<MyLang_Object *> *args)
{
    Environment* funcEnv = new Environment(this->closure);
    for(int i = 0; i < funcDefn->params->size(); i++){
        funcEnv->define((*(funcDefn->params))[i]->lexeme, (*args)[i]);
    }
    try{
        ipreter->executeBlock(funcDefn->body, funcEnv);
    }catch(myLang::ReturnExp* exp){
        if(isInitializer) return closure->getAt(0, "this");
        return exp->val;
    }
    if(isInitializer) return closure->getAt(0, "this");
    return NULL;
}

MyLang_object_type UserDefinedFunc::getType()
{
    return MyLang_object_type::MYLANG_CALLABLE;
}

UserDefinedClass::UserDefinedClass(std::string clsName,
    std::unordered_map<std::string, UserDefinedFunc*>* methods)
{
    this->clsName = clsName;
    this->methods = methods;
}

int UserDefinedClass::arity()
{
    UserDefinedFunc* initFunc = getMethod("init");
    if(!initFunc) return 0;
    return initFunc->arity();
}

MyLang_Object *UserDefinedClass::call(Interpreter *ipreter, std::vector<MyLang_Object *> *args)
{
    UserDefinedClassInstance* instance = new UserDefinedClassInstance(this);
    UserDefinedFunc* initFunc = getMethod("init");
    if(initFunc)
        initFunc->bind(instance)->call(ipreter, args);
    return instance;
}

MyLang_object_type UserDefinedClass::getType()
{
    return MyLang_object_type::MYLANG_CALLABLE;
}

std::string UserDefinedClass::toString()
{
    return this->clsName;
}

UserDefinedFunc *UserDefinedClass::getMethod(std::string name)
{
    if(methods->find(name) != methods->end())
        return (*methods)[name];
    return nullptr;
}

UserDefinedClassInstance::UserDefinedClassInstance(UserDefinedClass *cls)
{
    this->cls = cls;
    this->setType(MyLang_object_type::MYLANG_INSTANCE);
}

std::string UserDefinedClassInstance::toString()
{
    return this->cls->toString()+" instance.";
}

MyLang_Object *UserDefinedClassInstance::get(Token *name)
{
    if(this->fields->find(name->lexeme) != this->fields->end())
        return (*this->fields)[name->lexeme];
    UserDefinedFunc* uf = this->cls->getMethod(name->lexeme);
    if(uf)
        return uf->bind(this);
    throw myLang::RuntimeError(name, "Undefined property '" + name->lexeme + "'.");
}

void UserDefinedClassInstance::set(Token *name, MyLang_Object *value)
{
    (*this->fields)[name->lexeme] = value;
}
