#include <parser/parser.h>
#include <lexer/token.h>

Expr *Parser::getExpr()
{
    return getEquality();
}
Expr *Parser::getEquality()
{
    return nullptr;
}