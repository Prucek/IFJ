/**
 * @file parser.h
 * @authors Peter Rucek ...
 * @date 25 Oct 2020
 * @brief Interface for the parser
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "error.h"
#include "symtable.h"


typedef struct
{
    Token actual_token;
    int actual_line;
    TNode *global_table;
    TNode *local_table;
    TNode *suspected;   //< symtable for functions suspecetd from no_definition
    int index;          //< for searching of retval_arr and arg_arr
    
}Metadata;


TData new_data_func, new_data_var;

TData init_new_data(TData new_data);

void check_suspected(TNode *root);

int program();

bool func();

void header_arg();

void header_ret();

bool func_header();

void prolog();

bool expect_token(Token_type t_type, Keyword k);

bool statement();

void function_call(Token id);

void if_s();

void assignment_s(int number_of_id);

void for_s();

void return_s();

void expression();

#endif