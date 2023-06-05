#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct Position { size_t ln, col; } Position;

typedef struct Token {
    Position pos;
    enum TokenType {
        TOKEN_TYPE_UNRECOGNIZED,
        TOKEN_TYPE_UNEXPECTED,
        TOKEN_TYPE_EOF,

        TOKEN_TYPE_IDENTIFIER,
        TOKEN_TYPE_BOOLEAN,
        TOKEN_TYPE_LROUND,
        TOKEN_TYPE_RROUND,
        TOKEN_TYPE_LSQUARE,
        TOKEN_TYPE_RSQUARE,
        TOKEN_TYPE_TILDE,
        TOKEN_TYPE_AMPERSAND,
        TOKEN_TYPE_PIPE,
        TOKEN_TYPE_LARROW,
        TOKEN_TYPE_RARROW,
        TOKEN_TYPE_BIARROW,
        TOKEN_TYPE_COMMA,
        TOKEN_TYPE_DOT,
        TOKEN_TYPE_INTERROGATIVE,
        TOKEN_TYPE_BANG
    } type;
    union {
        size_t identifier;
        bool boolean;
        struct { char erroneous, expected; };
    };
} Token;

#endif
