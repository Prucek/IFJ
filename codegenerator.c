/**
 * @file codegenerator.c
 * @authors Peter Rucek ...
 * @date 13 Nov 2020
 * @brief Imlemenatation of code generator
 */

#include "codegenerator.h"

#define CODE(_code) if(!add_string(&code,(_code)))return false;

dynamic_string code; // code will be stored here and flushed in the end of compilation to stdout if compilation went successful

/**
 * @brief Generate needed header and alloc code buffer
 */
bool gen_header()
{
    dynstr_init(&code);
    CODE(".IFJcode20\n");

    // constants needed, to add
    CODE("DEFVAR GF@_\n");
    CODE("DEFVAR GF@expr_result\n");
    
    CODE("JUMP main\n");

    return true;
}

/**
 * @brief free allocated code buffer
 */
void gen_dispose()
{
    dynstr_free(&code);
}

/**
 * @brief Generate function header
 */
bool gen_func_header(char *func_name)
{
    CODE("LABEL "); CODE(func_name); CODE("\n");
    CODE("CREATEFRAME\n");
    CODE("PUSHFRAME\n");

    return true;
}

bool gen_print()
{
    CODE("WRITE string@Hello\\032World!\\010\n");

    return true;
}

/**
 * @brief Generate main end
 */
bool gen_main_end()
{
    CODE("POPFRAME\n");
    CODE("CLEARS");

    return true;
}

/**
 * @brief Generate function end
 */
bool gen_func_end()
{
    CODE("POPFRAME\n");
    CODE("RETURN");

    return true;
}

/**
 * @brief Puts code buffer to stdout
 */
void flush()
{
    puts(code.buff);
    gen_dispose();
}