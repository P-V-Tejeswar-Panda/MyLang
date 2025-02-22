#ifndef __ERROR_H__
#define __ERROR_H__
#include <lexer/token.h>
#include <string>
#include <iostream>
#include <sstream>
namespace myLang{
void report(std::string msg);
void communicateError(Token* tk, std::string msg);
}
#endif