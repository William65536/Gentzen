#ifndef PARSER_H
#define PARSER_H

#include "ASTNode.h"
#define T ASTNode
#include "..\includes\List.h"

void ASTNode_println(size_t self, const ASTNodeList *arena, const charList *string_arena);

ASTNodeList *Parser_parse(const Token *itr, const charList *string_arena);

#endif
