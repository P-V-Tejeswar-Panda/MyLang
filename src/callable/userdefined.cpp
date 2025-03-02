#include <callable/userdefined.h>


UserDefinedFunc::UserDefinedFunc(Function *func, Environment* closure)
{
    funcDefn = func;
    this->closure = closure;
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
        return exp->val;
    }
    return NULL;
}

MyLang_object_type UserDefinedFunc::getType()
{
    return MyLang_object_type::MYLANG_CALLABLE;
}
