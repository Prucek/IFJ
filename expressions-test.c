/**
 * @file expressions-test.c
 * @authors Peter Rucek ...
 * @date 16 Nov 2020
 * @brief Imlemenatation of expressions
 */

#include "expressions.h"

int main()
{
    if (expr(false,0))
        printf("Correct\n");
    else
        printf("False\n");
    return 0;
}