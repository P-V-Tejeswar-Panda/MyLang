#include <lexer/token.h>
#include <vector>


extern int yylex();
extern char* yytext;
extern int yylineno;

Token* nextToken(){
    int token;
    token = yylex();
    return new Token((enum TokenType)token, std::string(yytext), yylineno);
}

std::vector<Token*>* scanAllTokens(){
    std::vector<Token*>* vec = new std::vector<Token*>();
    while(true){
        Token* tk = nextToken();
        vec->push_back(tk);
        if(tk->ttype == TokenType::EOF_)
            break;
    }
    return vec;
}