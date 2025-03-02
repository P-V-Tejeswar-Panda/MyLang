#ifndef __AST_NODE_TYPES_H__
#define __AST_NODE_TYPES_H__
enum AST_NODE_TYPES{
    EXPR, LITERAL, GROUPING, UNARY, BINARY, VARIABLE, ASSIGN, EXPR_LOGICAL, EXPR_FUNC_CALL,
    STMT, STMT_EXPRESSION, STMT_PRINT, STMT_VAR, STMT_BLOCK,
    STMT_IF, STMT_WHILE, STMT_FOR, STMT_RET,
    DEFN_FUNC
};
#endif