/**
 * @file lexer.c
 * @authors Peter Rucek, Marek Micek, Rebeka Cernianska, Matej Jurik
 * @date 15 Oct 2020
 * @brief Lexer implementation
 */

#include "lexer.h"

const char *keywords[] = {"int", "string", "float64", "if", "else",
"for", "func", "package", "return", "inputs", "inputf", "inputi", "print", "int2float", "float2int", "len", "substr", "ord", "chr"};

dynamic_string buffer, error_buffer;    //< buffers for correct and incorrect lexems

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
    static int line = 1;       //< counter of actual line

    assert(f != NULL);
    Token t;
    State state = S_START;

    while(true)
    {
        int c = fgetc(f);
        switch (state)
        {
            case S_START:
                // Operators & comments
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
                else if (c == '/')
                {
                    state = S_COM_OR_DIV;
                }

                else if (c == '<') 
                {
                    state = S_LT;
                }

                else if (c == '>')
                {
                    state = S_GT;
                }

                else if (c == '!')
                {
                    state = S_NE;
                }

                else if (c == '=')
                {
                    state = S_ASGN_OR_EQ;
                }

                else if (c == ':')
                {
                    state = S_DEF;
                }

                // Identifiers, keywords
                else if (isalpha(c) || c == '_')
                {
                    state = S_ID_OR_KEY;
                    dynamic_string_init(&buffer);
                    add_char(&buffer, c);
                }

                // Numbers
                else if (isdigit(c))
                {
                    dynamic_string_init(&buffer);
                    add_char(&buffer, c);
                    state = S_NUM;
                }

                // End of Line
                else if (c == '\n')
                {
                    line++;
                    t.type = EOL;
                    t.data.s = NULL;
                    return t;
                }

                // End of File
                else if (c == EOF)
                {
                    t.type = EoF;
                    t.data.s = NULL;
                    return t;
                }

                // Whitespace
                else if (isspace(c))
                {
                    state = S_START;
                }

                //TODO
                break;

            case S_LT: 
                if (c == '=')
                {
                    t.type = LE;
                    t.data.s = NULL;

                    return t;
                }
                else
                {
                    ungetc(c, f);
                    t.type = LT;
                    t.data.s = NULL;

                    return t;
                }

                break;

            case S_GT:
                if (c == '=')
                {
                    t.type = GE;
                    t.data.s = NULL;

                    return t;
                }
                else
                {
                    ungetc(c, f);
                    t.type = GT;
                    t.data.s = NULL;

                    return t;
                }
                break;

            case S_NE:
                if (c == '=')
                {
                    t.type = NE;
                    t.data.s = NULL;

                    return t;
                }
                else 
                {
                    state = S_ERROR;
                }

                break;

            case S_ASGN_OR_EQ:
                if (c == '=')
                {
                    t.type = EQ;
                    t.data.s = NULL;

                    return t;
                }
                else
                {
                    ungetc(c, f);
                    t.type = VAR_ASSIGN;
                    t.data.s = NULL;
                    
                    return t;
                }

                break;

            case S_DEF:
                if (c == '=')
                {
                    t.type = DEF_OF_VAR;
                    t.data.s = NULL;

                    return t;
                }
                else
                {
                    state = S_ERROR;
                }

                break;

            case S_ID_OR_KEY:
                if (isalnum(c) || c == '_')
                {
                    state = S_ID_OR_KEY;
                    add_char(&buffer, c);
                }
                else
                {
                    ungetc(c, f);
                    state = S_START;
                    Keyword kw = -1;

                    for (Keyword i = K_INT; i <= K_CHR; i++) //iterates through enum Keywords
                    {
                        if (cmp_dyn_and_const(&buffer, keywords[i]) == 0)
                        {
                            kw = i;
                        }
                    }

                    if (kw == -1)
                    {
                        t.type = ID;
                        t.data.s = buffer.buff; //need to properly add data
                    }
                    else
                    {
                        t.type = KEYWORD;
                        t.data.k = kw;
                    }

                    dyn_string_free(&buffer);
                    return t;
                }
                break;

            case S_STRING:
                break;

            // Classification for INTs, FLOATs and FLOAT Exponentials
            case S_NUM:
                if ((isdigit(c)) && buffer.buff[0] == '0')  //< useless zero at the beginning is forbidden
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
                    ungetc(c, f);
                }
                break;

            case S_INT:

                ungetc(c, f);   //< returns last scanned char cause not included in this token
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
                    add_char(&buffer, c);
                    state = S_EXPO_1;
                }
                else if (isalpha(c))
                {
                    state = S_ERROR;
                }
                else
                {
                    ungetc(c, f);
                    t.type = FLOAT64;
                    t.data.d = atof(buffer.buff);
                    dyn_string_free(&buffer);
                    return t;
                }
                break;

            // Exponential classification - first phase
            case S_EXPO_1:
                // Expect either sign (pos, neg) or exponential number
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

            // Exponential classification - second phase
            case S_EXPO_2:
                // Sign was specified - accept exponential numbers only
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

            // Exponential classification - last phase
            case S_EXPO_3:
                // Accept more numbers if sign was not specified 
                // or accept exponential as FLOAT64
                if (isdigit(c))
                {
                    add_char(&buffer, c);
                }
                else if ((isalpha(c)) || c == '.' || c == '_' || c == ':')
                {
                    state = S_ERROR;
                }
                else
                {
                    ungetc(c, f);
                    t.type = FLOAT64;
                    t.data.d = atof(buffer.buff);
                    dyn_string_free(&buffer);
                    return t;
                }
                break;

            // Distinguish div operator from comments
            case S_COM_OR_DIV:
                if (c == '/')
                {
                    state = S_L_COMMENT;
                }
                else if (c == '*')
                {
                    state = S_B_COMMENT;
                }
                else // Division operator
                {
                    ungetc(c,f);
                    t.type = DIV;
                    t.data.s = NULL; 
                    
                    return t;
                }
                break;

            case S_L_COMMENT:
                if (c == '\n' || c == EOF)
                {
                    ungetc(c,f);
                    state = S_START;
                }
                else
                {
                    state = S_L_COMMENT;
                }
                break;

            case S_B_COMMENT:
                if (c == '*')
                {
                    state = S_B_COMMENT_END;
                }
                else if (c == EOF)
                {
                    //TODO /* .....EOF
                }
                else
                {
                    state = S_B_COMMENT;
                }
                break;
            case S_B_COMMENT_END:
                if (c == '/')
                {
                    state = S_START;
                }
                else if (c == EOF)
                {
                    //TODO /* .....EOF
                }
                else
                {
                    state = S_B_COMMENT;
                }
                break;

            case S_ERROR:

                dynamic_string_init(&error_buffer);             //< init the buffer for incorrect lexem
                if (add_string(&error_buffer, buffer.buff))  //< in order to print full lexem, not only incorrect part
                {
                    if (c != '\n')
                    {
                        add_char(&error_buffer, c);                 //< we add last scanned char

                        while (!(isspace(c = fgetc(f))))
                            add_char(&error_buffer, c);
                    }
                }

                if (c == '\n')      //< we return eol if this was the whitespace
                    ungetc(c, f);

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

        if (state == S_ERROR)       //< we return last char which caused error state, in order to make correct error output
            ungetc(c, f);
    }
}