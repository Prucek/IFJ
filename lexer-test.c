/**
 * @file lexer-test.c
 * @authors Peter Rucek ...
 * @date 15 Oct 2020
 * @brief Lexer tests
 */

#include "lexer.h"
#include "error.h"

 char *arr[] = {
    "ID",
    "INT",
    "STRING",
    "FLOAT64",
    "KEYWORD",

    "PARENTHESIS_LEFT",   // (
    "PARENTHESIS_RIGHT",  // )
    "BRACKET_LEFT",       // {
    "BRACKET_RIGHT",      // }

    "SUB",
    "DIV",
    "ADD",
    "MUL",

    "COMMA",      // ,
    "DEF_OF_VAR", // :=
    "VAR_ASSIGN", // =

    "GT",        // >
    "LT",        // <
    "NE",        // !=
    "LE",        // <=
    "GE",        // >=
    "EQ",        // ==

    "EOL",
    "EoF",      
 };
void print(Token t)
{
    printf("Token type: %s\n",arr[t.type]);
    printf("Token data: %p\n",t.data.s);
}

int main ()
{
    Token t;
    while(t.type != EoF)
    {
        t = get_next_token(stdin);
        print(t);
    }

    return error_value;
}