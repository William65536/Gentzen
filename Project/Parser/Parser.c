#include <stdio.h>
#include <stdbool.h>
#include <setjmp.h>
#include <assert.h>
#include "..\Lexer\Token.h"
#define T char
#include "..\includes\List.h"
#include "ASTNode.h"
#define T ASTNode
#include "..\includes\List.h"

static void ASTNode_print(size_t self, const ASTNodeList *arena, charList *string_arena)
{
    assert(arena != NULL);
    assert(string_arena != NULL);

    const ASTNode node = ASTNodeList_get(arena, self);

    switch (node.type) {
        case AST_TYPE_IDENTIFIER:
            fputs(charList_getptr(string_arena, node.identifier), stdout);
            break;
        case AST_TYPE_BOOLEAN:
            printf("%d", node.boolean);
            break;
        case AST_TYPE_NEGATION:
            putchar('~');
            ASTNode_print(node.arg, arena, string_arena);
            break;
        case AST_TYPE_CONJUNCTION:
            ASTNode_print(node.left, arena, string_arena);
            fputs(" & ", stdout);
            ASTNode_print(node.right, arena, string_arena);
            break;
        case AST_TYPE_DISJUNCTION:
            ASTNode_print(node.left, arena, string_arena);
            fputs(" | ", stdout);
            ASTNode_print(node.right, arena, string_arena);
            break;
        case AST_TYPE_IMPLICATION:
            ASTNode_print(node.left, arena, string_arena);
            fputs(" -> ", stdout);
            ASTNode_print(node.right, arena, string_arena);
            break;
        case AST_TYPE_CONV_IMPLICATION:
            ASTNode_print(node.left, arena, string_arena);
            fputs(" <- ", stdout);
            ASTNode_print(node.right, arena, string_arena);
            break;
        case AST_TYPE_BI_IMPLICATION:
            ASTNode_print(node.left, arena, string_arena);
            fputs(" <-> ", stdout);
            ASTNode_print(node.right, arena, string_arena);
            break;
        case AST_TYPE_PAIR:
            ASTNode_print(node.left, arena, string_arena);
            fputs(", ", stdout);
            ASTNode_print(node.right, arena, string_arena);
            break;
        case AST_TYPE_EMPTY:
            break;
        case AST_TYPE_ASSERTION:
            ASTNode_print(node.arg, arena, string_arena);
            putchar('.');
            break;
        case AST_TYPE_QUERY:
            ASTNode_print(node.arg, arena, string_arena);
            putchar('?');
            break;
        case AST_TYPE_REMARK:
            ASTNode_print(node.arg, arena, string_arena);
            putchar('!');
            break;
        default:
            assert(0 && "Unreachable");
    }
}

void ASTNode_println(size_t self, const ASTNodeList *arena, charList *string_arena)
{
    assert(arena != NULL);
    assert(string_arena != NULL);

    ASTNode_print(self, arena, string_arena);
    puts("");
}

static void try(bool expression, jmp_buf envbuf, int errorcode)
{
    assert(envbuf != NULL);

    if (!expression)
        longjmp(envbuf, errorcode);
}

static bool parse_expression(const Token **itr, ASTNodeList *arena, charList *string_arena, jmp_buf envbuf);

static bool parse_group(const Token **itr, ASTNodeList *arena, charList *string_arena, jmp_buf envbuf)
{
    assert(itr != NULL && *itr != NULL);
    assert(arena != NULL);
    assert(string_arena != NULL);
    assert(envbuf != NULL);

    const Token *start = *itr;

    if ((*itr)->type != TOKEN_TYPE_LROUND && (*itr)->type != TOKEN_TYPE_LSQUARE)
        return false;

    const bool issquare = (*itr)->type == TOKEN_TYPE_LSQUARE;

    ++*itr;

    const size_t preallocation_size = ASTNodeList_size(arena);

    if (!parse_expression(itr, arena, string_arena, envbuf)) {
        *itr = start;
        return false;
    }

    if (!((*itr)->type == TOKEN_TYPE_RROUND && !issquare) && !((*itr)->type == TOKEN_TYPE_RSQUARE && issquare)) {
        ASTNodeList_shrink(arena, preallocation_size);
        *itr = start;
        return false;
    }

    ++*itr;

    return true;
}

static bool parse_atom(const Token **itr, ASTNodeList *arena, charList *string_arena, jmp_buf envbuf)
{
    assert(itr != NULL && *itr != NULL);
    assert(arena != NULL);
    assert(string_arena != NULL);
    assert(envbuf != NULL);

    if ((*itr)->type == TOKEN_TYPE_IDENTIFIER) {
        try(ASTNodeList_push(arena, (ASTNode) {
            .pos = (*itr)->pos,
            .type = AST_TYPE_IDENTIFIER,
            .identifier = (*itr)->identifier
        }), envbuf, 1);

        ++*itr;

        return true;
    }

    if ((*itr)->type == TOKEN_TYPE_BOOLEAN) {
        try(ASTNodeList_push(arena, (ASTNode) {
            .pos = (*itr)->pos,
            .type = AST_TYPE_BOOLEAN,
            .boolean = (*itr)->boolean
        }), envbuf, 1);

        ++*itr;

        return true;
    }

    if (parse_group(itr, arena, string_arena, envbuf))
        return true;

    return false;
}

static bool parse_negation(const Token **itr, ASTNodeList *arena, charList *string_arena, jmp_buf envbuf)
{
    assert(itr != NULL && *itr != NULL);
    assert(arena != NULL);
    assert(string_arena != NULL);
    assert(envbuf != NULL);

    const Token *start = *itr;

    const Position pos = (*itr)->pos;

    size_t negcount;
    for (negcount = 0; (*itr)->type == TOKEN_TYPE_TILDE; negcount++, ++*itr);

    if (!parse_atom(itr, arena, string_arena, envbuf)) {
        *itr = start;
        return false;
    }

    if (negcount % 2 == 0)
        return true;

    try(ASTNodeList_push(arena, (ASTNode) {
        .pos = pos,
        .type = AST_TYPE_NEGATION,
        .arg = ASTNodeList_size(arena) - 1
    }), envbuf, 1);

    return true;
}

// static bool parse_conjunction(const Token **itr, ASTNodeList *arena, charList *string_arena, jmp_buf envbuf)
// {
//     assert(itr != NULL && *itr != NULL);
//     assert(arena != NULL);
//     assert(string_arena != NULL);
//     assert(envbuf != NULL);
// }

static bool parse_expression(const Token **itr, ASTNodeList *arena, charList *string_arena, jmp_buf envbuf)
{
    return parse_negation(itr, arena, string_arena, envbuf);
}

ASTNodeList *Parser_parse(const Token *itr, charList *string_arena)
{
    assert(string_arena != NULL);

    ASTNodeList *ret = ASTNodeList_new(10);

    if (ret == NULL)
        return NULL;

    jmp_buf envbuf;

    if (setjmp(envbuf) != 0)
        goto failure;

    if (!parse_expression(&itr, ret, string_arena, envbuf))
        goto failure;

    return ret;

    failure:

    ASTNodeList_delete(&ret);

    return NULL;
}
