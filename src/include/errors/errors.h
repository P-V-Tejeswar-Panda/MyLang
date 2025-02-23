#ifndef __ERRORS_H__
#define __ERRORS_H__
#include <lexer/token.h>
#include <myLang/globals.h>
#include <string>
#include <iostream>
#include <sstream>

namespace myLang{
class ParseError{
    public:
    Token* tk;
    ParseError(Token* tk);
};

class RuntimeError{
public:
    Token* tk;
    std::string msg;
    RuntimeError(Token* tk, std::string msg);
};
void report(std::string msg);
void communicateError(Token* tk, std::string msg);
void communicateRuntimeError(RuntimeError* err);
}
#endif



