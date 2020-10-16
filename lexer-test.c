/**
 * @file lexer-test.c
 * @authors Peter Rucek ...
 * @date 15 Oct 2020
 * @brief Lexer tests
 */

#include "lexer.h"
#include "error.h"

void print(Token t)
{
    printf("Token type: %d\n",t.type);
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