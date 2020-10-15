/**
 * @file lexer-test.c
 * @authors Peter Rucek ...
 * @date 15 Oct 2020
 * @brief Lexer tests
 */

#include "lexer.h"

void print(Token t)
{
    printf("Token type: %d\n",t.type);
    printf("Token data: %p\n",t.data);
}

int main ()
{
    Token t;
    while(t.type != EoF)
    {
        t = get_next_token(stdin);
        print(t);
    }

}