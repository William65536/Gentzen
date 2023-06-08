#ifndef AST_NODE_H
#define AST_NODE_H

#include <stdbool.h>
#include <stddef.h>

#include "..\Lexer\Token.h"

typedef struct ASTNode ASTNode;
struct ASTNode {
    Position pos;
    enum ASTType {
        AST_TYPE_IDENTIFIER,
        AST_TYPE_BOOLEAN,
        AST_TYPE_NEGATION,
        AST_TYPE_CONJUNCTION,
        AST_TYPE_DISJUNCTION,
        AST_TYPE_IMPLICATION,
        AST_TYPE_CONV_IMPLICATION,
        AST_TYPE_BI_IMPLICATION,
        AST_TYPE_PAIR,
        AST_TYPE_EMPTY,
        AST_TYPE_ASSERTION,
        AST_TYPE_QUERY,
        AST_TYPE_REMARK
    } type;
    union {
        size_t identifier;
        bool boolean;
        struct { size_t arg; };
        struct { size_t left, right; };
    };
};

#endif
