#include <iostream>
#include <lexer/token.h>

extern int yylex();
extern char* yytext;

int main(int argc, char** argv){
    int token;
    while(token = yylex()){
        std::cout << token;
        if(token == NUMBER)
            std::cout << ": " << yytext << std::endl;
        else if(token == STRING)
            std::cout << ": " << yytext << std::endl;
        else if(token == EOF_){
            std::cout << ": " << yytext << std::endl;
            break;
        }
        else
            std::cout << ": " << yytext << std::endl;
    }
}