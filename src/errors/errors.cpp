#include <errors/errors.h>

namespace myLang{
ParseError::ParseError(Token *tk)
{
    this->tk = tk;
}

RuntimeError::RuntimeError(Token *tk, std::string msg)
{
    this->tk = tk;
    this->msg = msg;
}
void communicateError(Token *tk, std::string msg){
    std::ostringstream err_msg;
    if(tk->ttype == TokenType::EOF_){
        err_msg << tk->lineno << ": at end " << msg;
        report(err_msg.str());
    }
    else{
        err_msg << tk->lineno << ": at '" << tk->lexeme << "' : " << msg;
        report(err_msg.str());
    }
    MyLangGlobals::getInstance().hadError = true;
}
void communicateRuntimeError(RuntimeError *err)
{
    std::ostringstream err_msg;
    err_msg << err->msg << "\n[line " << err->tk->lineno << "]";
    report(err_msg.str());
    MyLangGlobals::getInstance().hadRuntimeError = true;
}
void report(std::string msg){
    std::cout << msg << std::endl;
}
ReturnExp::ReturnExp(MyLang_Object *obj)
{
    this->val = obj;
}

}
