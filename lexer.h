/**
 * @file lexer.h
 * @authors Peter Rucek ...
 * @date 15 Oct 2020
 * @brief Interface for the lexer
 */

#ifndef _LEXER_H_
#define _LEXER_H_

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

extern const char *keywords[];

/**
 * @brief keywords[index] == Keyword state
 */
typedef enum
{
    K_ERROR = -1,
    K_INT,
    K_STRING,
    K_FLOAT64,
    K_IF,
    K_ELSE,
    K_FOR,
    K_FUNC,
    K_PACKAGE,
    K_RETURN,

} Keyword;

typedef enum
{
    ID,
    INT,
    STRING,
    FLOAT64,
    KEYWORD,

    PARENTHESIS_LEFT,   // (
    PARENTHESIS_RIGHT,  // )
    BRACKET_LEFT,       // {
    BRACKET_RIGHT,      // }

    SUB,
    DIV,
    ADD,
    MUL,

    COMMA,      // ,
    DEF_OF_VAR, // :=
    VAR_ASSIGN, // =

    GT,        // >
    LT,        // <
    NE,        // !=
    LE,        // <=
    GE,        // >=
    EQ,        // ==

    EOL,
    EoF,       // ??
    //TODO

} Token_type;

typedef enum
{
    S_START,
    S_COMMENT,
    S_INT,
    S_STRING,
    S_DOUBLE,
    S_ERROR,
    S_ID_OR_KEY,
    // TODO
} State;


typedef union 
{
    Keyword k;
    char *s;
    int i;
    double d;
} Data;


typedef struct token
{
    Token_type type;
    Data data;     // data reperesented as string, if int token.data = "55"...., if no data needed token.data = NULL

} Token;


//prototypes
Token get_next_token(FILE *f);
Keyword is_keyword(char *str);
//TODO


#endif //_LEXER_H_