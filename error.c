/**
 * @file error.c
 * @authors Marek Micek ....
 * @date 16 OCT 2020
 * @brief Error functions
 */

#include <stdio.h>
#include <stdlib.h>


#include "lexer.h"
#include "error.h"


error_value = 0;    //< in case of no error


// TODO doplnit parametre funckii na zaklade implementacie scanera a parsera

void lexical_error(char *str, int line)
{
    fprintf(stderr, "****  Lexical error: lexem which caused error = '%s', on line = '%d' !  ****\n", str, line);
    error_value = 1;
}


void syntax_error()
{
    fprintf(stderr, "****  Syntax error: token, line !  ****\n");
    error_value = 2;
}


void no_defination_error()      //< mozno zlucit s re_defination_error, zavisi na implementacii
{
    fprintf(stderr, "****  Semantic error : Identifier was not defined yet !  ****\n");
    error_value = 3;
}


void re_defination_error()
{
    fprintf(stderr, "****  Semantic error : Tried to define identifier : ID, but identifier : ID is already defined !  ****\n");
    error_value = 3;
}


void type_error()
{
    fprintf(stderr, "****  Semantic error : Unmatched data type of variable : var, line !  ****\n");
    error_value = 4;
}


void compability_error()
{
    fprintf(stderr, "****  Semantic error : Compability error: token, line !  ****\n");
    error_value = 5;
}


void param_error()
{
    fprintf(stderr, "****  Semantic error : Unmatched number or type of paramethers in function : function !  ****\n");
    error_value = 6;
}


void other_error()
{
    fprintf(stderr, "****  Another kind of semantic error !  ****\n");
    error_value = 7;
}


void div_zero_error()
{
    fprintf(stderr, "****  Semantic error : division with zero !  ****\n");
    error_value = 9;
}


void intern_error()
{
    fprintf(stderr, "****  Intern error of compiler !  ****\n");
    error_value = 99;
}