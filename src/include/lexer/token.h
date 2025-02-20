#ifndef __TOKEN_H__
#define __TOKEN_H__
#include <string>

enum TokenType {
    LEFT_PAREN = 258, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, SEMICOLON, SLASH, STAR, PLUS, MINUS,

    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER,
    GREATER_EQUAL, LESS, LESS_EQUAL,

    IDENTIFIER, STRING, NUMBER,

    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    EOF_
};

class Token{
    public:
    enum TokenType ttype;
    std::string lexeme;
    int lineno;
    public:
    Token(enum TokenType ttype, std::string lexeme, int lineno){
        this->ttype = ttype;
        this->lexeme = lexeme;
        this->lineno = lineno;
    }
};
#endif