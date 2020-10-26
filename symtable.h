/**
 * @file symtable.h
 * @authors Marek Micek, ...
 * @date 23 OCT 2020
 * @brief Interface for the table of symbols
 */

#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "error.h"


typedef enum 
{
    T_UNDEFINED,
    T_INT,
    T_FLOAT64,
    T_STRING,
    T_NIL,
    //T_BOOL  bonusove riesenie??? 

} Data_type;


typedef struct tData
{
    char *id;
    Data_type type;
    bool defined;
    bool global;
    bool is_function;           //< whether the symbol is func
    unsigned param_counter;     //< counts how many param. the func has

} TData;


typedef struct tnode
{
    int key;    // TKey key, bude implementovat parser
    TData data;
    struct tnode *lptr;
    struct tnode *rptr;

} TNode;


TNode* init_symtable(TNode *root);
TNode* search_symtable(TNode *root, int k);
TNode* rewrite_data(TNode *node, TData new_data);  //< rewrite old data in node with new one
TNode* create_node(TData d, int k);
TNode* insert_symtable(TNode *root, TData d, int k);
void delete_symtable(TNode *root);

/* funkcie na implemetaciu vymazania jedneho uzla, mozno nepotrebne*/

TNode* most_left_node(TNode *root);     //< returns the most left node suitable for delete
TNode* delete_node(TNode *root, int k); //< deletes the node with key k

#endif