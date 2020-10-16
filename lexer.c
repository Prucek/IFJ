/**
 * @file lexer.c
 * @authors Peter Rucek ...
 * @date 15 Oct 2020
 * @brief Lexer implementation
 */

#include "lexer.h"

const char *keywords[] = {"int", "string", "float64", "if", "else",
"for", "func", "package", "return"};

Keyword is_keyword(char *str)
{
    int size = sizeof(keywords) / sizeof(keywords[0]);
    for(int i = 0; i < size; i++)
    {
        if(strcmp(str,keywords[i]) == 0)
            return i;
    }
    return K_ERROR;
}

Token get_next_token(FILE *f)
{
    assert(f != NULL);
    Token t;
    State state = S_START;
    while(true)
    {
        int c = fgetc(f);
        switch (state)
        {
            case S_START:
                if (c == '+')
                {
                    t.type = ADD;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == '-')
                {
                    t.type = SUB;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == '*')
                {
                    t.type = MUL;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == ',')
                {
                    t.type = COMMA;
                    t.data.s = NULL;
                    return t;
                }
                // if (c == '/') can be comment
                // {
                //     t.type = ADD;
                //     t.data.s = NULL;
                //     return t;
                // }
                
                else if (isalpha(c) || c == '_')
                {
                    state = S_ID_OR_KEY;
                }
                else if (c == '\n')
                {
                    t.type = EOL;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == EOF)
                {
                    t.type = EoF;
                    t.data.s = NULL;
                    return t;
                }
                else if (isspace(c))
                {
                    state = S_START;
                }
                //TODO
                break;
            //TODO BUFFER
            case S_ID_OR_KEY:
                break;
            case S_STRING:
                break;
            case S_INT:
                break;
            case S_DOUBLE:
                break;
            case S_COMMENT:
                break;
            case S_ERROR:
                break;
        }
    }
}