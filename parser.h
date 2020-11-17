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
#include "codegenerator.h"
#include "stack.h"
#include "expressions.h"

typedef struct
{
    Token current_token;
    Token previous_token;
    int current_line;
    TNode *global_table;
    TNode *local_table;
    TNode *suspected;   //< symtable for functions suspecetd from no_definition
    int index;          //< for searching of retval_arr and arg_arr
    
}Metadata;

// // Type of action being performed at the moment 
// typedef enum 
// {
//     CA_Definition,
//     CA_Assignment,
//     CA_If,
//     CA_For,
//     CA_Return

// } CurrentAction; //< Used in expression type determination

// typedef enum
// {
//     EXPR_SIMPLE,  //< Parsed as "operand => operator => operand" expr   
//     EXPR_GROUPPED //< Parsed as "... operator => operand" expr, ommiting left operand

// } tExpr; 

TData new_data_func, new_data_var;

TData init_new_data(TData new_data);

void check_suspected(TNode *root);

void program();

bool func();

void header_arg();

void header_ret();

bool func_header(bool *is_main);

void prolog();

bool expect_token(Token_type t_type, Keyword k);

bool statement();

void function_call(Token id);

void if_s();

void assignment_s(int number_of_id);

void for_s();

void return_s();

bool expression();

bool input(int *input_token, bool *func_call);

// bool __expression();

// bool comparison();

// bool term();

// bool factor();

// bool literal();

// bool curtok_matches(Token_type checkList[], int listSize);

#endif