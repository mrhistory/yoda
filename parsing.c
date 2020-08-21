#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

// for Windows
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// fake readline function
char* readline(char *prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

// fake add_history function
void add_history(char *unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv) {
    // define polish notation jim-lisp language
    // create parsers
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Yoda = mpc_new("yoda");

    // define the language
    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                         \
            number      : /-?[0-9]+/ ;                            \
            operator    : '+' | '-' | '*' | '/' ;                 \
            expr        : <number> | <expr>+ <operator> | '(' <expr>+ <operator> ')' ; \
            yoda    : /^/ <expr>+ <operator> /$/ | /^/ '(' <expr>+ <operator> ')' /$/ ;            \
        ",
        Number, Operator, Expr, Yoda);

    puts("\nYoda Version 0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while(1) {
        char *input = readline("yoda> ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Yoda, &r)) {
            // On Success print the AST
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            // Otherwise print the error
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    mpc_cleanup(4, Number, Operator, Expr, Yoda);

    return 0;
}