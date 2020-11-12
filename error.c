/**
 * @file error.c
 * @authors Marek Micek, Peter Rucek ....
 * @date 16 OCT 2020
 * @brief Error functions
 *
 * @todo Doplnit parametre funckii na zaklade implementacie scanera a parsera
 */

#include "error.h"

char *array[] = {
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
    "SEMICLN",    // ;

    "GT",        // >
    "LT",        // <
    "NE",        // !=
    "LE",        // <=
    "GE",        // >=
    "EQ",        // ==

    "EOL",
    "EoF",
 };

int error_value = 0; //< in case of no error


/**
 * PLACEHOLDER - Documentation may not be necessary here!
 *
 * @brief Inform about lexical error occurrence
 * @param line Line of error causing lexeme
 * @post error_value = 1
 */
void lexical_error(int line)
{
    fprintf(stderr, "****  Lexical error on line = '%d' !  ****\n", line);
    error_value = 1;
}


void syntax_error(int token_type, int line)
{
    static int _line = 0; //print only one mistake on single line
    if (line != _line && token_type >= 0)
        fprintf(stderr, "****  Syntax error: token type %s, line %d!  ****\n",array[token_type], line);
    _line = line;
    if (error_value != 1) // only if no lex error
    {
        error_value = 2;
    }
}


void no_definition_error(char *id, int line)      //< mozno zlucit s re_definition_error, zavisi na implementacii
{
    fprintf(stderr, "****  Semantic error : Identifier: %s on line: %d was not defined yet !  ****\n", id, line);
    if (error_value == 0)
    {
        error_value = 3;
    }
}


void re_definition_error(char *id, int line)
{
    fprintf(stderr, "****  Semantic error : Identifier: %s on line: %d is already defined !  ****\n", id, line);
    if (error_value == 0)
    {
        error_value = 3;
    }
}


void type_error()
{
    fprintf(stderr, "****  Semantic error : Unmatched data type of variable : var, line !  ****\n"); 
    if (error_value == 0)
    {
        error_value = 4;
    }
}


void compatibility_error()
{
    fprintf(stderr, "****  Semantic error : compatibility error: token, line !  ****\n");
    if (error_value == 0)
    {
        error_value = 5;
    }
}


void param_error(char *id, int line)
{
    fprintf(stderr, "****  Semantic error : Unmatched number or type of paramethers in function: %s on line: %d !  ****\n", id, line);
    if (error_value == 0)
    {
        error_value = 6;
    }
}


void other_error()
{
    fprintf(stderr, "****  Another kind of semantic error !  ****\n");
    if (error_value == 0)
    {
        error_value = 8;
    }
}


void div_zero_error()
{
    fprintf(stderr, "****  Semantic error : division with zero !  ****\n");
    if (error_value == 0)
    {
        error_value = 9;
    }
}


void intern_error()
{
    fprintf(stderr, "****  Intern error of compiler !  ****\n");
    error_value = 99;
}