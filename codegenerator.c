/**
 * @file codegenerator.c
 * @authors Peter Rucek ...
 * @date 13 Nov 2020
 * @brief Imlemenatation of code generator
 */

#include "codegenerator.h"
#include "stack.h"

#define CODE(_code) if(!add_string(&code,(_code)))return false;

// Variadic args MUST be strings (otherwise segfault) & last arg must be "\n" (otherwise infinite cycle)
#define CODELN(...) if(!add_strings(&code, ##__VA_ARGS__)) return false;

dynamic_string code; // code will be stored here and flushed in the end of compilation to stdout if compilation went successful
char *index;
char *els;
Stack *index_stack;

/**
 * @brief Generate needed header and alloc code buffer
 */
bool gen_header()
{
    dynstr_init(&code);

    CODE(".IFJcode20\n");
    index = malloc(sizeof(char));
    els = malloc(sizeof(char));
    if (index == NULL || els == NULL)
    {
        intern_error();
    }
    *index = 64;
    index_stack = createStack(100);
    // constants needed, to add
    CODE("DEFVAR GF@_\n");
    CODE("DEFVAR GF@expr_result\n");
    CODE("MOVE GF@expr_result bool@true\n");
    CODE("JUMP main\n");
    return true;
}

/**
 * @brief Generate function header
 */
bool gen_func_header(char *func_name)
{
    CODELN("LABEL ", func_name, "\n");
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
 * @brief Generate if branch
 */
bool if_label()
{
    (*index)++;
    push(index_stack, *index);
    CODELN("LABEL if", index, "\n");
    return true;
}
/**
 * @brief Generate jump over else branch to end of if
 */
bool if_jump()
{
    *els = top(index_stack);
    CODELN("JUMP end", els, "\n");
    return true;
}
/**
 * @brief Generate jump over if branch to else
 */
bool else_jump()
{
    CODELN("JUMPIFEQ else", index, " GF@expr_result bool@false", "\n");
    return true;
}
/**
 * @brief Generate else branch
 */
bool else_label()
{
    *els = top(index_stack);
    CODELN("LABEL else", els, "\n");
    return true;
}
/**
 * @brief Generate end of if
 */
bool if_end_label()
{
    *els = top(index_stack);
    CODELN("LABEL end", els, "\n");
    pop(index_stack);
    return true;
}

// bool gen_var_def(char *id)
// {
//     CODE("DEFVAR "); CODE(id); CODE("\n");
//     return true;
// }

/**
 * @brief Generate main end
 */
bool gen_main_end()
{
    CODE("POPFRAME\n");
    CODE("CLEARS\n");
    CODE("JUMP end_of_code\n");
    return true;
}

/**
 * @brief Generate function end
 */
bool gen_func_end()
{
    CODE("POPFRAME\n");
    CODE("RETURN\n");
    return true;
}

/**
 * @brief Generate label where to jump after main end, so other functions are not executed
 */
bool gen_code_end()
{
    CODE("LABEL end_of_code\n");
    return true;
}

/**
 * @brief free allocated code buffer
 */
void gen_dispose()
{
    dynstr_free(&code);
    free(index);
    free(els);
}

/**
 * @brief Puts code buffer to stdout
 */
void flush()
{
    puts(code.buff);
    gen_dispose();
}