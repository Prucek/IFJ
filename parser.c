/**
 * @file parser.c
 * @authors Rebeka Cernianska, Marek Micek, Matej Jurik, Peter Rucek
 * @date 25 Oct 2020
 * @brief Parser Implementation
 */

#include "parser.h"

void program(FILE *f)
{
    prolog(f); // if wrong only lex error ??????
    functions(f);
}

void functions(FILE *f)
{
    while(func(f));
    expect_token(f,EoF,K_ERROR);
}

bool func(FILE *f)
{
    func_header(f);
    //TODO...body
}

bool func_header(FILE *f)
{
    expect_token(f,KEYWORD,K_FUNC);
    //TODO
}

void prolog(FILE *f)
{
    if (!expect_token(f, KEYWORD, K_PACKAGE))
    {
        syntax_error();
    }
    
    Token t = get_next_token(f);
    if (t.type != ID || 0) // TODO access symbol table t.data.s == main
    {
        syntax_error();
    }
}

bool expect_token(FILE *f, Token_type t_type, Keyword k) //to lexer.c ????????
{
    while(true)
    {
        Token t = get_next_token(f);
        if (t.type == t_type && ((k == K_ERROR)? true : t.data.k == k))
        {
            return true;
        }
        else if (t.type == EOL)
        {
            continue;
        }
        else
        {
            return false;
        }
    }
}