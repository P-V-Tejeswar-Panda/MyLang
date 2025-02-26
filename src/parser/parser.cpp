/*
This expression parser implements A recursive descent parser for
the following grammar.
--------------------------------------------------------------
program        → declaration* EOF ;

declaration    → varDecl
               | statement ;

statement      → exprStmt
               | printStmt
               | ifStmt
               | block ;
ifStmt         → "if" "(" expression ")" statement
               ( "else" statement )?;
block          → "{" declaration* "}" ;
expression     → assignment ;
assignment     → IDENTIFIER "=" assignment
               | equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")"
               | IDENTIFIER ;
--------------------------------------------------------------
*/
#include <parser/parser.h>
#include <lexer/token.h>


Parser::Parser(std::vector<Token *> *tokens)
{
    this->tokens = tokens;
    this->currentPos = 0;
}

Token *Parser::getTokenAt(int pos)
{
    return this->tokens->at(pos);
}

myLang::ParseError *Parser::error(Token *tk, std::string msg)
{
    myLang::communicateError(tk, msg);
    return new myLang::ParseError(tk);
}
void Parser::synchronize()
{
    // We will have the offending / unexpected token in the queue.
    while(peek()->ttype != TokenType::EOF_){
        if(peek()->ttype == SEMICOLON){
            advance();
            return;
        }

        switch(peek()->ttype){
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
        }

        advance();
    }
}
void Parser::advance()
{
    this->currentPos++;
}

Token *Parser::previous()
{
    if(this->currentPos == 0)
        return NULL;
    return this->getTokenAt(this->currentPos-1);
}

Token *Parser::peek()
{
    if(this->currentPos >= this->tokens->size())
        return NULL;
    return this->getTokenAt(this->currentPos);
}

Expr *Parser::getExpr()
{
    return getAssign();
}

Expr *Parser::getAssign()
{
    Expr* exp = getEquality();
    if(peek()->ttype == TokenType::EQUAL){
        Token* eq = peek();
        advance();
        Expr* val = getAssign();
        if(exp->nodeType() == AST_NODE_TYPES::VARIABLE){
            Token* name = ((Variable*)exp)->name;
            return new Assign(name, val);
        }
        error(eq, "Invalid assignment target.");
    }
    return exp;
}

Expr *Parser::getEquality()
{
    Expr* expr = getComparison();
    Token* t = peek();
    while (t && (t->ttype == TokenType::BANG_EQUAL || t->ttype == TokenType::EQUAL_EQUAL))
    {
        advance();
        Token* op = this->previous();
        Expr* right = getComparison();
        expr = new Binary(expr, op, right);
        t = peek();
    }
    return expr;
}
Expr *Parser::getComparison()
{
    Expr* expr = getTerm();
    Token *t = peek();
    while (t && 
        (t->ttype == TokenType::GREATER ||
         t->ttype == TokenType::GREATER_EQUAL ||
         t->ttype == TokenType::LESS ||
         t->ttype == TokenType::LESS_EQUAL)
    )
    {
        advance();
        Token* op = this->previous();
        Expr* right = getTerm();
        expr = new Binary(expr, op, right);
        t = peek();
    }
    return expr;
}

Expr *Parser::getTerm()
{
    Expr* expr = getFactor();
    Token *t = peek();
    while (t && 
        (t->ttype == TokenType::MINUS ||
         t->ttype == TokenType::PLUS)
    )
    {
        advance();
        Token* op = this->previous();
        Expr* right = getFactor();
        expr = new Binary(expr, op, right);
        t = peek();
    }
    return expr;
}

Expr *Parser::getFactor()
{
    Expr* expr = getUnary();
    Token *t = peek();
    while (t && 
        (t->ttype == TokenType::STAR ||
         t->ttype == TokenType::SLASH)
    )
    {
        advance();
        Token* op = this->previous();
        Expr* right = getUnary();
        expr = new Binary(expr, op, right);
        t = peek();
    }
    return expr;
}

Expr *Parser::getUnary()
{
    Expr* exp = NULL;
    Token* t = peek();
    if(t && (t->ttype == TokenType::BANG ||
             t->ttype == TokenType::MINUS))
    {
        advance();
        exp = getUnary();
        exp = new Unary(t, exp);
    }
    else{
        exp = getPrimary();
    }
    return exp;
}

Expr *Parser::getPrimary()
{
    Expr* exp = NULL;
    Token* t = peek();
    if(t && t->ttype == TokenType::LEFT_PAREN){
        advance();
        exp = getExpr();
        Token* op = t;
        t = peek();
        if(t->ttype == TokenType::RIGHT_PAREN){
            exp = new Grouping(op, exp, t);
            advance();
        }
        else
            throw error(t, "Expect ')' after expression.");
    }
    else if (t && (t->ttype == TokenType::NUMBER ||
                   t->ttype == TokenType::STRING ||
                   t->ttype == TokenType::TRUE   ||
                   t->ttype == TokenType::FALSE  ||
                   t->ttype == TokenType::NIL))
    {
        exp = new Literal(t);
        advance();
    }
    else if(t && (t->ttype == TokenType::IDENTIFIER)){
        exp = new Variable(t);
        advance();
    }
    else
        throw error(t, "Expect expression.");
    return exp;
}

Stmt *Parser::parseStatement()
{
    if(peek()->ttype == TokenType::PRINT)
        return parsePrintStatement();
    if(peek()->ttype == TokenType::LEFT_BRACE)
        return new Block(parseBlock());
    if(peek()->ttype == TokenType::IF)
        return parseIfStatement();
    return parseExpressionStatement();
}
Stmt *Parser::parsePrintStatement()
{
    advance(); // consume the innital 'print' token.
    Expr* val = getExpr();
    if(peek()->ttype != TokenType::SEMICOLON)
        throw error(peek(), "Expected ';' after value.");
    advance();
    return new Print(val);
}
Stmt *Parser::parseExpressionStatement()
{
    Expr* val = getExpr();
    if(peek()->ttype != TokenType::SEMICOLON)
        throw error(peek(), "Expected ';' after expression.");
    advance();
    return new Expression(val);
}

std::vector<Stmt *> *Parser::parseBlock()
{
    std::vector<Stmt*>* ret = new std::vector<Stmt*>();
    advance();

    while(peek()->ttype != TokenType::RIGHT_BRACE && peek()->ttype != TokenType::EOF_)
    {
        ret->push_back(parseDeclaration());
    }

    if(peek()->ttype != TokenType::RIGHT_BRACE)
        throw error(peek(), "Expect '}' after block.");
    advance();
    return ret;
}
Stmt *Parser::parseIfStatement()
{
    if(peek()->ttype != TokenType::IF)
        throw error(peek(), "Expect 'if' at the beginning.");    // Not really necessary
    advance();
    if(peek()->ttype != TokenType::LEFT_PAREN)
        throw error(peek(), "Expect '(' after if.");
    advance();
    Expr* if_cond = getExpr();
    if(peek()->ttype != TokenType::RIGHT_PAREN)
        throw error(peek(), "Expect ')' at the end of if condition.");
    advance();
    Stmt* if_body = parseStatement();
    if(peek()->ttype == TokenType::ELSE){
        advance();
        Stmt* else_body = parseStatement();
        return new If(if_cond, if_body, else_body);
    }
    return new If(if_cond, if_body, NULL);
}
Stmt *Parser::parseDeclaration()
{
    try{
        if(peek()->ttype == TokenType::VAR){
            advance();
            return parseVarDeclaration();
        }
        return parseStatement();
    } catch (myLang::ParseError* pe){
        synchronize();
        return NULL;
    }
}

Stmt *Parser::parseVarDeclaration()
{
    Token* name = peek();
    if(name->ttype != TokenType::IDENTIFIER){
        throw error(name, "Expect variable name.");
    }
    advance();
    Expr* initializer = NULL;
    if(peek()->ttype == TokenType::EQUAL){
        advance();
        initializer = getExpr();
    }
    if(peek()->ttype != TokenType::SEMICOLON){
        throw error(peek(), "Expect ';' at the end of variable declaration.");
    }
    advance();
    return new Var(name, initializer);
}

std::vector<Stmt *> *Parser::getAST()
{
    try
    {
        std::vector<Stmt*>* stmts = new std::vector<Stmt*>();
        while(peek()->ttype != TokenType::EOF_){
            stmts->push_back(parseDeclaration());
        }
        return stmts;
    }
    catch(const myLang::ParseError* e)
    {
        return NULL;
    }
}



