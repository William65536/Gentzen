#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "Lexer\Token.h"
#include "Lexer\Lexer.h"

#include <stdio.h>
#include <stdlib.h>

#include "Lexer\Lexer.h"

static charList *string_arena;

static void Token_print_arena(Token token) { Token_print(token, string_arena); }

void print_error(const char *type, const char *format, ...)
{
    assert(type != NULL);
    assert(type != NULL);

    va_list args;

    va_start(args, format);

    fprintf(stderr, "%s: ", type);
    vfprintf(stderr, format, args);
}

int main(void)
{
    const char *const input = "a <->\n@b";

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
