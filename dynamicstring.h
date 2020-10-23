/**
 * @file dynamicstring.h
 * @authors Rebeka Cernianska, Peter Rucek, Matej Jurik ...
 * @date 16 Oct 2020
 * @brief Inteface for dynamicstring.c program
 */

#ifndef _DYNAMICSTRING_H_
#define _DYNAMICSTRING_H_


#include "error.h"
#define BLOCK 8

typedef struct dynamic_string
{
    int len; //pocet znakov ulozenych v stringu
    int alloc_len; //pocet znakov kolko moze byt ulozenych v stringu
    char *buff;
} dynamic_string;


void dynamic_string_init(dynamic_string *str);

int add_char(dynamic_string *str, char c);

int add_string(dynamic_string *str, char *str_to_copy);

void dyn_string_free(dynamic_string *str);

int cmp_dyn_and_const(dynamic_string *dyn_str, const char *const_str);

#endif