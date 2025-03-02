/*
This expression parser implements A recursive descent parser for
the following grammar.
--------------------------------------------------------------
program        → declaration* EOF ;

declaration    → varDecl
               | statement
               | funcDecl ;
funcDecl       → "func" function;
function       → IDENTIFIER "(" parameters ")" block ;
parameters     → IDENTIFIER ("," IDENTIFIER)* ;
statement      → exprStmt
               | printStmt
               | ifStmt
               | whileStmt
               | forStmt
               | block ;

forStmt        → "for" "(" (declaration|stmt)?;
                        (expression)?; (expression)? ")" statement;
whileStmt      → "while" "(" expression ")" statement ;
ifStmt         → "if" "(" expression ")" statement
               ( "else" statement )?;
block          → "{" declaration* "}" ;
expression     → assignment ;
assignment     → IDENTIFIER "=" assignment
               | equality ;
logical_or     → logical_and ("or" logical_and)*;
logical_and    → equality ("and" equality)*;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | call ;
call           → primary ("(" arguments? ")")* ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")"
               | IDENTIFIER ;
arguments      → expression ("," expression)* ;
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

Token *Parser::match(enum TokenType ntype)
{
    Token* ret = NULL;
    if(peek()->ttype == ntype){
        ret = peek();
        advance();
    }
    return ret;
}

Token *Parser::consume(TokenType ntype, std::string msg)
{
    Token* ret = NULL;
    if(peek()->ttype == ntype){
        ret = peek();
        advance();
    }
    else{
        throw error(peek(), msg);
    }
    return ret;
}

Expr *Parser::getExpr()
{
    return getOr();
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
        exp = getFuncCall();
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

Expr *Parser::getOr()
{
    Expr* left = getAnd();
    while(peek()->ttype == TokenType::OR){
        Token* tk = peek(); advance();
        Expr* right = getAnd();
        return new Logical(left, tk, right);
    }
    return left;
}
Stmt *Parser::parseStatement()
{
    if(peek()->ttype == TokenType::PRINT)
        return parsePrintStatement();
    if(peek()->ttype == TokenType::LEFT_BRACE)
        return new Block(parseBlock());
    if(peek()->ttype == TokenType::IF)
        return parseIfStatement();
    if(peek()->ttype == TokenType::WHILE)
        return parseWhileStatement();
    if(peek()->ttype == TokenType::FOR)
        return parseForStatement();
    if(peek()->ttype == TokenType::RETURN)
        return parseReturnStatement();
    return parseExpressionStatement();
}
Expr *Parser::getAnd()
{
    Expr* left = getAssign();
    while(peek()->ttype == TokenType::AND){
        Token* tk = peek(); advance();
        Expr* right = getAssign();
        return new Logical(left, tk, right);
    }
    return left;
}
Expr *Parser::getFuncCall()
{
    Expr* exp = getPrimary();
    while(match(TokenType::LEFT_PAREN)){
        std::vector<Expr*>* args = new std::vector<Expr*>();
        if(peek()->ttype == TokenType::RIGHT_PAREN){
            exp = new FuncCall(exp, peek(), args);
            advance();
            continue;
        }
        do{
            if(args->size() >= 255)
                error(peek(), "Not more than 255 arguments is allowed!");
            args->push_back(getExpr());
        }
        while(match(TokenType::COMMA));
        if(peek()->ttype != TokenType::RIGHT_PAREN)
            throw error(peek(), "Expect ')' after arguments in function call.");
        exp = new FuncCall(exp, peek(), args);
        advance();
    }
    return exp;
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
Stmt *Parser::parseWhileStatement()
{
    if(peek()->ttype != TokenType::WHILE)
        throw error(peek(), "Expect 'while' at the beginning.");    // Not really necessary
    advance();
    if(peek()->ttype != TokenType::LEFT_PAREN)
        throw error(peek(), "Expect '(' after 'while'.");
    advance();
    Expr* while_cond = getExpr();
    if(peek()->ttype != TokenType::RIGHT_PAREN)
        throw error(peek(), "Expect ')' at the end of 'while' condition block.");
    advance();
    Stmt* while_body = parseStatement();
    return new While(while_cond, while_body);
}
Stmt *Parser::parseForStatement()
{
    Stmt* forInit = NULL;
    Expr* forCond = NULL;
    Expr* forInc  = NULL;
    Stmt* forBody = NULL;
    if(peek()->ttype != TokenType::FOR)
        throw error(peek(), "Expect 'for' at the beginning.");    // Not really necessary
    advance();
    if(peek()->ttype != TokenType::LEFT_PAREN)
        throw error(peek(), "Expect '(' after 'while'.");
    advance();
    if(peek()->ttype != TokenType::SEMICOLON)
        forInit = parseDeclaration();
    else
        advance();
    if(peek()->ttype != TokenType::SEMICOLON)
        forCond = getExpr();
    if(peek()->ttype != TokenType::SEMICOLON)
        throw error(peek(), "Expect ';' after 'for' check statement.");
    advance();
    if(peek()->ttype != TokenType::RIGHT_PAREN)
        forInc = getExpr();
    if(peek()->ttype != TokenType::RIGHT_PAREN)
        throw error(peek(), "Expect ')' after 'for' loop increment statement.");
    advance();
    forBody = parseStatement();
    std::vector<Stmt*>* stmts = new std::vector<Stmt*>();
    std::vector<Stmt*>* whileBody = new std::vector<Stmt*>();
    While* whBlk = NULL;
    if(forInc){
        whileBody->push_back(forBody);
        whileBody->push_back(new Expression(forInc));
        forBody = new Block(whileBody);
    }
    if(forCond)
        whBlk = new While(forCond, forBody);
    else
        whBlk = new While(new Literal(new Token(TokenType::TRUE, "true", -1)), forBody);
    if(forInit){
        stmts->push_back(forInit);
        stmts->push_back(whBlk);
        return new Block(stmts);
    }
    return whBlk;
}
Stmt *Parser::parseReturnStatement()
{
    Token* ret_token = consume(TokenType::RETURN, "Expect 'return' at the beginning of return statement.");
    Expr* exp = NULL;
    if(peek()->ttype != TokenType::SEMICOLON)
        exp = getExpr();
    consume(TokenType::SEMICOLON, "Expect ';' at the end of return statement.");
    return new Return(ret_token, exp);
}
Stmt *Parser::parseDeclaration()
{
    try{
        if(peek()->ttype == TokenType::VAR){
            advance();
            return parseVarDeclaration();
        }
        if(match(TokenType::FUN))
            return parseFuncDeclaration("function");
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

Stmt *Parser::parseFuncDeclaration(std::string kind)
{
    Token* name = consume(TokenType::IDENTIFIER, "Expect "+kind+" name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after "+kind+" name.");
    std::vector<Token*>* params = new std::vector<Token*>();
    if(!match(TokenType::RIGHT_PAREN)){
        do{
            if(params->size() >= 255){
                error(peek(), "Can't have more than 255 parameters");
            }
            params->push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        }
        while(match(TokenType::COMMA));   
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    }
    // parse block expects the current token to be pointing to '{'
    if(peek()->ttype != TokenType::LEFT_BRACE)
        throw error(peek(), "Expect '{' before "+kind+" body.");
    std::vector<Stmt*>* body = parseBlock();
    return new Function(name, params, body);
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



