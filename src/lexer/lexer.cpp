#include <iostream>
#include <lexer/token.h>

extern int yylex();
extern char* yytext;
extern int yylineno;

Token nextToken(){
    int token;
    token = yylex();
    Token tk((enum TokenType)token, std::string(yytext), yylineno);
    return tk;
}

int main(int argc, char** argv){
    Token tk = nextToken();
    while(tk.ttype != EOF_){
        std::cout << tk.ttype;
        std::cout << ": " << tk.lexeme << "\tLn: " 
                    << tk.lineno << std::endl;
        tk = nextToken();
    }
}