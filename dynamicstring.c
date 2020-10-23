/**
 * @file dynamicstring.c
 * @authors Rebeka Cernianska, Peter Rucek...
 * @date 16 Oct 2020
 * @brief Flexible buffer for saving token data
 */

#include <stdio.h>
#include <stdlib.h>

#include "dynamicstring.h"

void dynamic_string_init(dynamic_string *str) //prepares the structure before use
{
    str->len = 0;
    str->alloc_len = 0;
    str->buff = NULL;
}

int add_char(dynamic_string *str, char c) //adds char to the first empty spot in the buffer
{
    if (str->len == 0) //when the buffer is empty, we allocate memory
    {
        str->buff = malloc(BLOCK);

        if (str->buff == NULL)
        {
            fprintf(stderr, "Error allocating dynamic string\n");
            return 1;
        }

        str->alloc_len = BLOCK;
    }
    else if (str->len+1 >= str->alloc_len) //reallocating memory when more is needed
    {
        str->buff = realloc(str->buff, str->alloc_len+BLOCK);

        if (str->buff == NULL)
        {
            fprintf(stderr, "Error reallocating dynamic string\n");
            return 1;
        }

        str->alloc_len += BLOCK;
    }

    str->buff[str->len++] = c; //set new data and ending char of string
    str->buff[str->len] = '\0';
    return 0;
}

int add_string(dynamic_string *str, char *str_to_copy) //adds string to the buffer, starting on the first empty position
{
    if (str->len == 0) //if the buffer is empty, we allocate memory
    {
        str->buff = malloc(strlen(str_to_copy)+1); //we only allocate the needed space

        if (str->buff == NULL)
        {
            fprintf(stderr, "Error allocating dynamic string\n");
            return 1;
        }

        str->alloc_len = strlen(str_to_copy); //set how much space is available in the buffer currently
    }
    else if (str->len+strlen(str_to_copy) >= (unsigned)str->alloc_len) //if we need more space than is available
    {
        str->buff = realloc(str->buff, str->alloc_len+strlen(str_to_copy)+1); //we only realloc the amount of space we need

        if (str->buff == NULL)
        {
            fprintf(stderr, "Error reallocating dynamic string\n");
            return 1;
        }

        str->alloc_len += strlen(str_to_copy);
    }

    for (unsigned int i = 0; i < strlen(str_to_copy); i++) //transfering data to the target buffer
    {
        str->buff[i+str->len] = str_to_copy[i];
    }

    str->len = strlen(str_to_copy)+str->len;
    str->buff[str->len] = '\0';

    return 0;
}

void dyn_string_free(dynamic_string *str) //allows to re-use buffer, needs to be used before a buffer is used again
{
    if (str->buff != NULL)
    {
        free(str->buff);
    }
    str->len = 0;
    str->alloc_len = 0;
}


int cmp_dyn_and_const(dynamic_string *dyn_str, const char *const_str) //compares a dyn_str with a string, returns 0 if strings are equal
{
    if (dyn_str->buff == NULL || const_str == NULL)
    {
        return -1;
    }
    return strcmp(dyn_str->buff, const_str);
}