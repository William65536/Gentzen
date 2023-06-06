#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include "Lexer\Token.h"
#include "Lexer\Lexer.h"

#include "Parser\ASTNode.h"
#include "Parser\Parser.h"

static charList *string_arena;

static void Token_print_arena(Token token) { Token_print(token, string_arena); }

void print_error(const char *type, const char *format, ...)
{
    assert(type != NULL);
    assert(format != NULL);

    va_list args;

    va_start(args, format);

    fprintf(stderr, "%s: ", type);
    vfprintf(stderr, format, args);
}

/** TODO: Get rid of `=`---only support `<->` */
/** TODO: Deprecate remarks */
/** TODO: Add in better parser errors---use an expectation system */
/** TODO: Track memory allocations or use Valgrind (or something similar) */

int main(void)
{
    const char *const input = "~~(~~~(~(~(~p))))";

    string_arena = charList_new(100);

    if (string_arena == NULL)
        goto failure;

    TokenList *tokens; {
        tokens = Lexer_lex(input, string_arena);

        if (tokens == NULL) {
            print_error("APPLICATION ERROR", "Failed allocation!\n");
            goto charList_delete_and_failure;
        }

        TokenList_println(tokens, Token_print_arena);

        if (Lexer_has_errors(tokens, print_error))
            goto TokenList_delete_and_failure;
    }

    ASTNodeList *ast; {
        ast = Parser_parse(TokenList_begin(tokens), string_arena);

        if (ast == NULL) {
            /** TODO: Change this */
            print_error("APPLICATION ERROR", "Failed parse!\n");
            goto TokenList_delete_and_failure;
        }

        ASTNode_println(ASTNodeList_size(ast) - 1, ast, string_arena);
    }

    ASTNodeList_delete(&ast);
    TokenList_delete(&tokens);
    charList_delete(&string_arena);

    return EXIT_SUCCESS;

    {
        TokenList_delete_and_failure:

        TokenList_delete(&tokens);

        charList_delete_and_failure:

        charList_delete(&string_arena);

        failure:

        return EXIT_FAILURE;
    }
}
