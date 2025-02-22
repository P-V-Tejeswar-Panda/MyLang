#include <myLang/error.h>

namespace myLang{
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
}
void report(std::string msg){
    std::cout << msg << std::endl;
}
}