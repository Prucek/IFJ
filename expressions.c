/**
 * @file expressions.c
 * @authors Peter Rucek ...
 * @date 16 Nov 2020
 * @brief Imlemenatation of expressions
 */

#include "expressions.h"


TabItem precedence_table[TABLE_SIZE][TABLE_SIZE] =
{ //||_(__||_)__||_+-_||_*/_||_r__||_i__||_$__||
    {SHIFT,EQUAL,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR}, // (
    {T_ERR,REDUC,REDUC,REDUC,REDUC,T_ERR,REDUC}, // )
    {SHIFT,REDUC,REDUC,SHIFT,REDUC,SHIFT,REDUC}, // +-
    {SHIFT,REDUC,REDUC,REDUC,REDUC,SHIFT,REDUC}, // */
    {SHIFT,REDUC,SHIFT,SHIFT,T_ERR,SHIFT,REDUC}, // r
    {T_ERR,REDUC,REDUC,REDUC,REDUC,T_ERR,REDUC}, // i
    {SHIFT,T_ERR,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR}  // $
                                        //^^^ only ok variation
};

// for debugging
void printStack(Stack *s)
{
    for(int i = 0; i < 10; i++)
        printf("%d |",s->array[i]);
}

/**
 * @brief Adding tokens to stack an selecting action to be done by precedence table
 */
bool expr(Data_type *expr_type, bool *func_call, unsigned num_of_id, bool is_bool)
{
    /** 
     * @todo Forbid - * / operations for strings 
     *       Catch division by zero
     *       Generate arithmetic operations assembly
     */ 

    Terminal input_terminal;
    bool expr_isbool = false; //< Decide wheater exrpessions is type of bool
    bool expr_semerror = false; //< Wait for data type collisions or undefined IDs and if any occur, be sure to return false
    bool can_be_func = false; //< Will not print error if ID is not defined, can still be a func call, and if not then print error
    bool check_zero_div = false;
    Terminal previous;

    Stack *s = createStack(STACK_SIZE);
    push(s,EN);

    bool read = true;
    
    while(true)
    {
        if (read)
        {
            if (!expr_input(&input_terminal,func_call,num_of_id))
            {
                deleteStack(s);
                return false;
            }
            else
            {
                if (can_be_func) // id was read without data type
                {
                    if (input_terminal.terType == LP && *func_call)
                    {
                        deleteStack(s);
                        return true;
                    }
                    else
                    {
                        no_definition_error(previous.token.data.s, previous.current_line);
                        free(previous.token.data.s);
                        if (!expr_semerror) expr_semerror = true;
                    }
                    can_be_func = false;
                }

                if (input_terminal.terType == II) //< Operand
                {
                    if (check_zero_div)
                    {
                        if (input_terminal.token.data.i == 0 || input_terminal.token.data.d == (double)0)
                        {
                            div_zero_error(input_terminal.current_line);
                        }
                        check_zero_div = false;
                    }
                    // Check terminal's data type
                    if (*expr_type == T_UNDEFINED) //< Still not determined expr type
                    {
                        if (input_terminal.dataType != T_UNDEFINED)
                            *expr_type = input_terminal.dataType;
                        else 
                        {
                            // can be error or func call
                            can_be_func = true;
                            previous = input_terminal;
                        }
                    }
                    else 
                    {
                        if (input_terminal.dataType != *expr_type)
                        {
                            // Invalid operand types
                            compatibility_error(data_types[*expr_type], input_terminal.current_line);
                            if (!expr_semerror) expr_semerror = true;
                        }
                    }

                    /** @todo Free if id came */
                    if (input_terminal.dataType == T_STRING) // || input_terminal.token.type == ID)   not working, invalid read
                    {
                        free(input_terminal.token.data.s); //< Stop holding on to IDs
                        input_terminal.token.data.s = NULL;
                    }
                }
                if (*expr_type == T_STRING)
                {
                    if (input_terminal.terType == PM || input_terminal.terType == MD )
                        if (input_terminal.token.type != ADD)
                        {
                            compatibility_error(data_types[*expr_type], input_terminal.current_line);
                            if (!expr_semerror) expr_semerror = true;
                        }
                }
                if (input_terminal.terType == MD && input_terminal.token.type == DIV)
                {
                    check_zero_div = true;
                }    
                if (input_terminal.terType == RO) //< Logical Operator
                {
                    if (!expr_isbool) expr_isbool = true;
                }
            }
        }

        int f = find_terminal(s);
        if (f < 0 || input_terminal.terType < 0) 
        {
            deleteStack(s);
            return false;
        }
        TabItem to_perform = precedence_table[f][input_terminal.terType];

        if (to_perform == T_ERR)
        {
            deleteStack(s);
            if (input_terminal.terType == EN && f == EN) // input token == first terminal on stack == EN
            {
                if (expr_semerror)
                {
                    return false;
                }
                if (expr_isbool == is_bool)
                {
                    return true;
                }                
                else
                {
                    compatibility_error(data_types[*expr_type], input_terminal.current_line);
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if (to_perform == REDUC)
        {
            if (!reduce(s))
            {
                return false;
            }
            read = false;
            continue;
        }
        else if (to_perform == SHIFT)
        {
            if (input_terminal.terType == II)
            {
                push(s,SH);
            }
            else if (input_terminal.terType == PM || input_terminal.terType == MD || input_terminal.terType == RO)
            {
                int tmp = pop(s);
                push(s,SH);
                push(s,tmp);
            }
            push(s,input_terminal.terType);
            read = true; 
            continue;       
        }
        else if (to_perform == EQUAL)
        {
            push(s,input_terminal.terType);
            if (!reduce(s))
            {
                deleteStack(s);
                return false;
            }   
            read = true;
            continue;
        }
    }      
}

/**
 * @brief Find first terminal in stack
 */
int find_terminal(Stack *s)
{
    for(int i = s->top; i > -1; i--)
    {
        if(s->array[i] != NT)
        {
            return s->array[i];
        }     
    }
    return -1;
}

/**
 * @brief Reduce what is on stack 
 */
bool reduce(Stack *s)
{
    int token[STACK_SIZE] = {0};
    int i = -1;
    do
    {
        i++;
        if (top(s) == STACK_ERROR) 
        {
            return false;
        }
        token[i] = pop(s);
    } while ( token[i] != SH && token[i] != LP );

    // implmented rules
    if((token[0] == II && token[1] == SH) ||
       (token[0] == RP && token[1] == NT && token[2] == LP ) ||
       (token[0] == NT && token[1] == PM && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == MD && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == RO && token[2] == NT && token[3] == SH)   )
    {
        push(s,NT);
        return true;
    }
    else
    {
        return false;
    }
}