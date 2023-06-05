#ifndef LEXER_H
#define LEXER_H

#define T char
#include "..\includes\List.h"
#include "Token.h"
#define T Token
#include "..\includes\List.h"

void Token_print(Token self, const charList *string_arena);

TokenList *Lexer_lex(const char *input, charList *string_arena);

bool Lexer_has_errors(const TokenList *tokens, void (*output_error)(const char *type, const char *format, ...));

#endif
