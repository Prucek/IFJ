/**
 * @file dynamicstring.h
 * @authors Rebeka Cernianska ...
 * @date 16 Oct 2020
 * @brief Inteface for buffer.c program
 */

#ifndef _DYNAMICSTRING_H_
#define _DYNAMICSTRING_H_

#include "lexer.h"
#include "error.h"
#define BLOCK 8

typedef struct dynamic_string
{
 int len; //pocet znakov ulozenych v stringu
 int alloc_len; //pocet znakov kolko moze byt ulozenych v stringu
 char *buff;

} dynamic_string;

void dynamic_string_init(dynamic_string *str); //inicializuje string pred prvym pouzitim

int add_char(dynamic_string *str, char c); //prida znak na prve prazdne miesto v stringu

int add_string(dynamic_string *str, char *str_to_copy); //prida string na koniec stringu napr. keyword, aby sa nemuselo po znakoch

void dyn_string_free(dynamic_string *str); //uvolni alokovanu pamat a umozni opatovne pouzitie buffera; je potrebne pouzit vzdy ked chceme pouzit ten isty buffer na nove data

int cmp_dyn_and_const(dynamic_string *dyn_str, const char *const_str); //na porovnavanie s klucovymi slovami, a pod.; vrati 0 ked sa stringy rovnaju, vrati -1 ak su pouzite pointre s hodnotou NULL

#endif