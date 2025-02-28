#include <callable/userdefined.h>


UserDefinedFunc::UserDefinedFunc(Function *func)
{
    funcDefn = func;
}
int UserDefinedFunc::arity()
{
    return funcDefn->params->size();
}

MyLang_Object *UserDefinedFunc::call(Interpreter *ipreter, std::vector<MyLang_Object *> *args)
{
    Environment* funcEnv = new Environment(ipreter->globals);
    for(int i = 0; i < funcDefn->params->size(); i++){
        funcEnv->define((*(funcDefn->params))[i]->lexeme, (*args)[i]);
    }
    ipreter->executeBlock(funcDefn->body, funcEnv);
    return NULL;
}

MyLang_object_type UserDefinedFunc::getType()
{
    return MyLang_object_type::MYLANG_CALLABLE;
}
