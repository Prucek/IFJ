/**
 * @file error.h
 * @authors Marek Micek ....
 * @date 16 OCT 2020
 * @brief Interface for the error program
 */

#ifndef _ERROR_H_
#define _ERROR_H_

#include "lexer.h"


int error_value;    //< represents exit code of compiler


void lexical_error();
void syntax_error();
void no_defination_error();
void re_defination_error();
void type_error();
void compabiliy_error();
void param_error();
void other_error();
void div_zero_error();
void intern_error();


#endif