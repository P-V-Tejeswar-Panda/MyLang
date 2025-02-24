#ifndef __LEXER_H__
#define __LEXER_H__
#include <lexer/token.h>
#include <vector>

Token* nextToken();
std::vector<Token*>* scanAllTokens();
#endif