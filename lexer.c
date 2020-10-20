/**
 * @file lexer.c
 * @authors Peter Rucek, Marek Micek ...
 * @date 15 Oct 2020
 * @brief Lexer implementation
 */

#include "lexer.h"

const char *keywords[] = {"int", "string", "float64", "if", "else",
"for", "func", "package", "return"};

dynamic_string buffer, error_buffer;    //< buffers for correct an incorrect lexems

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
    int line = 1;       //< counter of actual line

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
                else if (isdigit(c)) 
                {
                    dynamic_string_init(&buffer);
                    add_char(&buffer, c); 
                    state = S_NUM;
                }
                else if (c == '\n')
                {
                    line++;
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

            case S_NUM:
                if (c == '0' && buffer.buff[0] == '0')  //< useless zero at the beginning is forbidden
                {
                    state = S_ERROR;
                }
                else if (isdigit(c))
                {
                    add_char(&buffer, c);
                }
                else if (c == '.')
                {
                    add_char(&buffer, c);
                    state = S_DOUBLE;
                }
                else if (c == 'e' || c == 'E')
                {
                    add_char(&buffer, c);
                    state = S_EXPO_1;
                }
                else if (isalpha(c))
                {
                    state = S_ERROR;
                }
                else
                {
                    state = S_INT;
                    ungetc(c,stdin);
                }
                break;

            case S_INT:
                t.type = INT;
                t.data.i = atoi(buffer.buff);
                dyn_string_free(&buffer);
                return t;
                break;

            case S_DOUBLE:
                if (c == '.')   //< only one decimal point is possible
                {
                    state = S_ERROR;
                }
                else if (isdigit(c))
                {
                    add_char(&buffer, c);
                }
                else if (c == 'e' || c == 'E')
                {
                    state = S_EXPO_1;
                }
                else if (isalpha(c))
                {
                    state = S_ERROR;
                }
                else
                {
                    ungetc(c, stdin);
                    t.type = FLOAT64;
                    t.data.d = atof(buffer.buff);
                    dyn_string_free(&buffer);
                    return t;
                }
                break;

            case S_EXPO_1:
                if (isdigit(c))
                {
                    add_char(&buffer, c);
                    state = S_EXPO_3;
                }
                else if (c == '+' || c == '-')
                {
                    add_char(&buffer, c);
                    state = S_EXPO_2;
                }
                else
                {
                    state = S_ERROR;
                }
                break;

            case S_EXPO_2:
                if (isdigit(c))
                {
                    add_char(&buffer, c);
                    state = S_EXPO_3;
                }
                else 
                {
                    state = S_ERROR;
                }
                break;

            case S_EXPO_3:
                if (isdigit(c))
                {
                    add_char(&buffer, c);
                }
                else 
                {
                    ungetc(c, stdin);
                    t.type = FLOAT64;
                    t.data.d = atof(buffer.buff);
                    dyn_string_free(&buffer);
                    return t;
                }
                break;

            case S_COMMENT:
                break;

            case S_ERROR:
                dynamic_string_init(&error_buffer);             //< init the buffer for incorrect lexem
                if (!(add_string(&error_buffer, buffer.buff)))  //< in order to print full lexem, not only incorrect part
                    while (!(isspace(c)))
                    {
                        add_char(&error_buffer, c);
                    }
                    
                lexical_error(error_buffer.buff, line);
                dyn_string_free(&buffer);
                dyn_string_free(&error_buffer);
                t.type = ERROR;
                t.data.s = NULL;
                return t;   //< mozno lepsie vraciat NULL, no treba zmenit implementaciu
                break;

            default:
                break;
        }
    }
}