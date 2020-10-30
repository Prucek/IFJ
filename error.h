/**
 * @file error.h
 * @authors Marek Micek, Peter Rucek ....
 * @date 16 OCT 2020
 * @brief Interface for the error program
 */

#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "dynamicstring.h"

extern int error_value; //< Represents exit code of compiler

// Error interface

void lexical_error(char *str, int line);
void syntax_error(); //< Prototype arguments missing!!
void no_definition_error();
void re_definition_error();
void type_error();
void compatibility_error();
void param_error();
void other_error();
void div_zero_error();
void intern_error();


#endif