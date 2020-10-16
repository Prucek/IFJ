/**
 * @file buffer.h
 * @authors Rebeka Cernianska ...
 * @date 16 Oct 2020
 * @brief inteface for buffer.c program
 */

#ifndef _BUFFERH_
#define _BUFFERH_

#include "lexer.h"
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


#endif