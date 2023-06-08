#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define T char
#include "..\includes\List.h"
#include "Token.h"
#define T Token
#include "..\includes\List.h"

void Token_print(Token self, const charList *string_arena)
{
    assert(string_arena != NULL);

    switch (self.type) {
        case TOKEN_TYPE_UNRECOGNIZED:
            printf("<UNRECOGNIZED (Ln %zu, Col %zu)>", self.pos.ln + 1, self.pos.col + 1);
            break;
        case TOKEN_TYPE_UNEXPECTED:
            printf("<UNEXPECTED (Ln %zu, Col %zu)>", self.pos.ln + 1, self.pos.col + 1);
            break;
        case TOKEN_TYPE_EOF:
            printf("<EOF>");
            break;
        case TOKEN_TYPE_IDENTIFIER:
            printf("<IDENTIFIER %s>", charList_getptr(string_arena, self.identifier));
            break;
        case TOKEN_TYPE_BOOLEAN:
            printf("<BOOLEAN %d>", self.boolean);
            break;
        case TOKEN_TYPE_LROUND:
            printf("<LROUND>");
            break;
        case TOKEN_TYPE_RROUND:
            printf("<RROUND>");
            break;
        case TOKEN_TYPE_LSQUARE:
            printf("<LSQUARE>");
            break;
        case TOKEN_TYPE_RSQUARE:
            printf("<RSQUARE>");
            break;
        case TOKEN_TYPE_TILDE:
            printf("<TILDE>");
            break;
        case TOKEN_TYPE_AMPERSAND:
            printf("<AMPERSAND>");
            break;
        case TOKEN_TYPE_PIPE:
            printf("<PIPE>");
            break;
        case TOKEN_TYPE_LARROW:
            printf("<LARROW>");
            break;
        case TOKEN_TYPE_RARROW:
            printf("<RARROW>");
            break;
        case TOKEN_TYPE_BIARROW:
            printf("<BIARROW>");
            break;
        case TOKEN_TYPE_COMMA:
            printf("<COMMA>");
            break;
        case TOKEN_TYPE_DOT:
            printf("<DOT>");
            break;
        case TOKEN_TYPE_INTERROGATIVE:
            printf("<INTERROGATIVE>");
            break;
        case TOKEN_TYPE_BANG:
            printf("<BANG>");
            break;
        default:
            assert(0 && "Unreachable");
    }
}

static void incr(const char **itr, Position *pos)
{
    assert(itr != NULL && *itr != NULL);
    assert(pos != NULL);

    if (**itr == '\0')
        return;

    if (**itr == '\n') {
        pos->ln++;
        pos->col = 0;
    } else if (**itr != '\r') {
        pos->col++;
    }

    ++*itr;
}

static void skipwhite(const char **itr, Position *pos)
{
    assert(itr != NULL && *itr != NULL);

    for (; **itr == ' ' || **itr == '\t' || **itr == '\n' || **itr == '\r'; incr(itr, pos));
}

static bool isalpha(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); /** NOTE: ASCII-specific */
}

static bool isdigit(char c)
{
    return '0' <= c && c <= '9';
}

TokenList *Lexer_lex(const char *input, charList *string_arena)
{
    assert(input != NULL);
    assert(string_arena != NULL);

    const char **itr = &input;

    Position pos = { .ln = 0, .col = 0 };

    TokenList *ret = TokenList_new(10);

    if (ret == NULL)
        return NULL;

    while (true) {
        skipwhite(itr, &pos);

        Token token = { .pos = pos };

        switch (**itr) {
            case '\0':
                token.type = TOKEN_TYPE_EOF;
                break;
            case '(':
                token.type = TOKEN_TYPE_LROUND;
                break;
            case ')':
                token.type = TOKEN_TYPE_RROUND;
                break;
            case '[':
                token.type = TOKEN_TYPE_LSQUARE;
                break;
            case ']':
                token.type = TOKEN_TYPE_RSQUARE;
                break;
            case '~':
                token.type = TOKEN_TYPE_TILDE;
                break;
            case '&':
                token.type = TOKEN_TYPE_AMPERSAND;
                break;
            case '|':
                token.type = TOKEN_TYPE_PIPE;
                break;
            case '<':
                incr(itr, &pos);

                if (**itr != '-') {
                    token.pos = pos;
                    token.type = TOKEN_TYPE_UNEXPECTED;
                    token.erroneous = **itr;
                    token.expected = '-';
                }

                if (*(*itr + 1) != '>') {
                    token.type = TOKEN_TYPE_LARROW;
                    break;
                }

                incr(itr, &pos);

                token.type = TOKEN_TYPE_BIARROW;

                break;
            case '-':
                incr(itr, &pos);

                if (**itr != '>') {
                    token.pos = pos;
                    token.type = TOKEN_TYPE_UNEXPECTED;
                    token.erroneous = **itr;
                    token.expected = '>';
                    break;
                }

                token.type = TOKEN_TYPE_RARROW;
                break;
            case ',':
                token.type = TOKEN_TYPE_COMMA;
                break;
                break;
            case '.':
                token.type = TOKEN_TYPE_DOT;
                break;
            case '?':
                token.type = TOKEN_TYPE_INTERROGATIVE;
                break;
            case '!':
                token.type = TOKEN_TYPE_BANG;
                break;
            case '#':
                for (; **itr != '\n' && **itr != '\0'; incr(itr, &pos));

                continue;
            case '0':
            case '1':
                token.type = TOKEN_TYPE_BOOLEAN;
                token.boolean = **itr - '0';
                break;
            default:
                if (isalpha(**itr) || **itr == '_') {
                    /** TODO: Possibly check for duplicates */

                    token.type = TOKEN_TYPE_IDENTIFIER;

                    const char *start = *itr;

                    token.identifier = charList_size(string_arena);

                    for (; isalpha(**itr) || isdigit(**itr) || **itr == '_' || **itr == '\''; ++*itr)
                        if (!charList_push(string_arena, **itr))
                            goto failure;

                    if (!charList_push(string_arena, '\0'))
                        goto failure;

                    --*itr;

                    pos.col += *itr - start;

                    break;
                }

                token.type = TOKEN_TYPE_UNRECOGNIZED;
                token.erroneous = **itr;
        }

        if (!TokenList_push(ret, token))
            goto failure;

        if (token.type == TOKEN_TYPE_EOF)
            break;

        incr(itr, &pos);
    }

    return ret;

    failure:

    TokenList_delete(&ret);

    return NULL;
}

bool Lexer_has_errors(const TokenList *tokens, void (*output_error)(const char *type, const char *format, ...))
{
    assert(tokens != NULL);

    bool ret = false;

    for (const Token *itr = TokenList_begin(tokens); itr < TokenList_end(tokens); itr++) {
        if (itr->type == TOKEN_TYPE_UNRECOGNIZED) {
            if (output_error != NULL)
                output_error("LEXER ERROR", "Unrecognized character `%c` at Ln %zu, Col %zu.\n", itr->erroneous, itr->pos.ln + 1, itr->pos.col + 1);

            ret = true;
        } else if (itr->type == TOKEN_TYPE_UNEXPECTED) {
            if (output_error != NULL)
                output_error("LEXER ERROR", "Unexpected character `%c` at Ln %zu, Col %zu. Expected `%c` instead.\n", itr->erroneous, itr->pos.ln + 1, itr->pos.col + 1, itr->expected);

            ret = true;
        }
    }

    return ret;
}
