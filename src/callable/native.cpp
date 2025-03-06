#include <callable/native.h>

int Clock::arity()
{
    return 0;
}

MyLang_Object *Clock::call(Interpreter *ipreter, std::vector<MyLang_Object *> *args)
{
    return new MyLang_Double(time(NULL));
}

MyLang_object_type Clock::getType()
{
    return MyLang_object_type::MYLANG_FUNCTION;
}
