%{
    #include <lexer/token.h>
    #include <string.h>
%}

%option noyywrap
%option yylineno

%%
"("     {return LEFT_PAREN;}
")"     {return RIGHT_PAREN;}
"{"     {return LEFT_BRACE;}
"}"     {return RIGHT_BRACE;}
","     {return COMMA;}
"."     {return DOT;}
"-"     {return MINUS;}
"+"     {return PLUS;}
";"     {return SEMICOLON;}
"/"     {return SLASH;}
"*"     {return STAR;}

"!"      {return BANG;}
"!="     {return BANG_EQUAL;}
"="      {return EQUAL;}
"=="     {return EQUAL_EQUAL;}
">"      {return GREATER;}
">="     {return GREATER_EQUAL;}
"<"      {return LESS;}
"<="     {return LESS_EQUAL;}

"and"    {return AND;}
"class"  {return CLASS;}
"else"   {return ELSE;}
"false"  {return FALSE;}
"fun"    {return FUN;}
"for"    {return FOR;}
"if"     {return IF;}
"nil"    {return NIL;}
"or"     {return OR;}
"print"  {return PRINT;}
"return" {return RETURN;}
"super"  {return SUPER;}
"this"   {return THIS;}
"true"   {return TRUE;}
"var"    {return VAR;}
"while"  {return WHILE;}

\".*\"                   {return STRING;}
\/\/.*                   {/*ignore comments*/}
<<EOF>>                  {return EOF_;}
[0-9]+(\.[0-9]+)*        {return NUMBER;}
[0-9a-zA-Z_]+            {return IDENTIFIER;}

[ \n\t]+                 {/*Ignore*/}

%%
