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
bool expr(bool *func_call, unsigned num_of_id)
{
    Stack *s = createStack(STACK_SIZE);
    push(s,EN);
    int input_token;
    bool read = true;
    while(true)
    {
        if (read)
        {
            if (!expr_input(&input_token,func_call,num_of_id))
            {
                deleteStack(s);
                return false;
            }
            else
            {
                if (*func_call)
                {
                    deleteStack(s);
                    return true;
                }   
            }
        }

        int f = find_terminal(s);
        if (f < 0 || input_token < 0) 
        {
            deleteStack(s);
            return false;
        }
        TabItem to_perform = precedence_table[f][input_token];

        if (to_perform == T_ERR)
        {
            deleteStack(s);
            if (input_token == EN && f == EN) // input token == first terminal on stack == EN
            {
                return true;
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
            if (input_token == II)
            {
                push(s,SH);
            }
            else if (input_token == PM || input_token == MD || input_token == RO)
            {
                int tmp = pop(s);
                push(s,SH);
                push(s,tmp);
            }
            push(s,input_token);
            read = true; 
            continue;       
        }
        else if (to_perform == EQUAL)
        {
            push(s,input_token);
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