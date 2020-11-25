/**
 * @file codegenerator.h
 * @authors Peter Rucek ...
 * @date 13 Nov 2020
 * @brief Inteface for codegenerator.c
 */

// GF
// LF
// TF
// example : GF@_x

// <symb> = constant
// examples :
// float@0x1.2666666666666p+0, bool@true,
// nil@nil nebo int@-5.

// <var>
// example :
// DEFVAR LF@a
// MOVE LF@a int@5

// <label>
// example :
// LABEL ahoj
// JUMP ahoj

// <type> ∈ {int, float, string, bool}

// instuctions :

// MOVE ⟨var⟩ ⟨symb⟩
// CREATEFRAME
// PUSHFRAME
// POPFRAME
// DEFVAR ⟨var⟩
// CALL ⟨label⟩
// RETURN

// PUSHS ⟨symb⟩
// POPS ⟨var⟩
// CLEARS

// ADD ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// SUB ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// MUL ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// DIV ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// IDIV ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// ADDS/SUBS/MULS/DIVS/IDIVS Zásobníkové verze instrukcí ADD, SUB, MUL, DIV a IDIV

// LT/GT/EQ ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// LTS/GTS/EQS Zásobníková verze instrukcí LT/GT/EQ

// AND/OR/NOT ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// ANDS/ORS/NOTS Zásobníková verze instrukcí AND, OR a NOT

// INT2FLOAT ⟨var⟩ ⟨symb⟩
// FLOAT2INT ⟨var⟩ ⟨symb⟩
// INT2CHAR ⟨var⟩ ⟨symb⟩
// STRI2INT ⟨var⟩ ⟨symb1⟩ ⟨symb2

// INT2FLOATS/FLOAT2INTS/
// INT2CHARS/STRI2INTS
// Zásobníkové verze konverzních instrukcí

// READ ⟨var⟩ ⟨type⟩
// WRITE ⟨symb⟩

// CONCAT ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// STRLEN ⟨var⟩ ⟨symb⟩
// GETCHAR ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
// SETCHAR ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩

// TYPE ⟨var⟩ ⟨symb⟩
// LABEL ⟨label⟩
// JUMP ⟨label⟩
// JUMPIFEQ ⟨label⟩ ⟨symb1⟩ ⟨symb2⟩
// JUMPIFNEQ ⟨label⟩ ⟨symb1⟩ ⟨symb2⟩
// JUMPIFEQS/JUMPIFNEQS ⟨label⟩ Zásobníková verze JUMPIFEQ, JUMPIFNEQ
// EXIT ⟨symb⟩
// BREAK
// DPRINT ⟨symb⟩



#ifndef _CODEGENERATOR_H_
#define _CODEGENERATOR_H_

#include "dynamicstring.h"
#include "symtable.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

bool gen_built_func();

bool gen_header();

void gen_dispose();

bool gen_main_start();

bool gen_func_header(char *func_name);

bool gen_func_call(char *func_id);

bool gen_func_retval(unsigned ret_counter);

bool gen_func_return(char *func_id, unsigned ret_counter);

bool gen_retval_assign(char *id, unsigned retval_index);

bool gen_param_pass(Token current_token, int param_index);

bool gen_param_val(Token current_token);

bool gen_var_def(char *id);

bool gen_createframe();

bool gen_main_end();

bool gen_print();

bool gen_stack_inst(int instruction, bool is_float);

bool gen_term(char *type, char* constant);

void float2hex(double d, char *buf);

void int2str(int n, char *buf);

void str2our_str(char *dst, char *src);

bool gen_operation(int type);

bool gen_expr_result();

bool gen_expr_begin();

bool gen_expr_end();
bool if_label();

bool if_jump();

bool else_jump();

bool else_label();

bool if_end_label();

bool for_header();

bool for_condition_eval();

bool for_body();

bool for_end();

bool gen_code_end();

bool gen_func_end(char *func_id);

void flush();

#endif