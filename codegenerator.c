/**
 * @file codegenerator.c
 * @authors Peter Rucek ...
 * @date 13 Nov 2020
 * @brief Imlemenatation of code generator
 */

//zvolil som konvencie mien premennych na generovanie labelov, parametrov, retval
// LABEL $name, %param(i), %retval(i)
#include "codegenerator.h"
#include "stack.h"

#define MAX_INDEX_LEN 100

#define CODE(_code) if(!add_string(&code,(_code)))return false;

// Variadic args MUST be strings (otherwise segfault) & last arg must be "\n" (otherwise infinite cycle)
#define CODELN(...) if(!add_strings(&code, ##__VA_ARGS__)) return false;

#define CODE_INT(_code) \
    do{ \
        char index[MAX_INDEX_LEN];   \
        sprintf(index, "%d", _code);    \
        CODE(index); \
    }while(0)

dynamic_string code; // code will be stored here and flushed in the end of compilation to stdout if compilation went successful
char *index;
char *els;
Stack *index_stack;
char *for_index;
char *index2;
Stack *for_index_stack;

/**
 * @brief Creates new TF for params of function before function_call
 */
bool gen_createframe()
{
    CODE("CREATEFRAME\n");
    return true;
}

/**
 * @brief Determines value to be assigned to parameter
 * @param current_token Token which represents current id
 * @return True if code generation successful, otherwise false
 */
bool gen_param_val(Token current_token)
{
    dynamic_string tmp_str;
    dynstr_init(&tmp_str);
    char current_val[MAX_INDEX_LEN];
    unsigned char c;

    switch (current_token.type)
    {
        case INT:
            sprintf(current_val, "%d", current_token.data.i);
            CODELN("int@", current_val, "\n");
            break;

         case FLOAT64:
            sprintf(current_val, "%a", current_token.data.d);
            CODELN("float@", current_val, "\n");
            break;
        // malo by sa optimalizovat, najst vhodnu funkciu na parsovanie stringu..mozno strtok?
        // plus hadze lexikalny error ak dame do stringu len lomitko \ , to by nemalo
        case STRING:
            for (int i = 0; (c = (unsigned char) (current_token.data.s)[i]) != '\0'; i++)
            {
                if  (c == 92)
                {
                    unsigned char tmp_char = (unsigned char) (current_token.data.s)[i+1];
                    if (tmp_char == 'n')
                    {
                        tmp_char = '\n';
                        add_char(&tmp_str, '\\');
                        sprintf(current_val, "%03d", tmp_char);
                        add_string(&tmp_str, current_val);
                        i++;
                    }
                    else
                    {
                        add_char(&tmp_str, '\\');
                        sprintf(current_val, "%03d", c);
                        add_string(&tmp_str, current_val);
                    }
                }
                else if (c <= 32 || c == 35)
                {
                    add_char(&tmp_str, '\\');
                    sprintf(current_val, "%03d", c);
                    add_string(&tmp_str, current_val);
                }
                else
                {
                    add_char(&tmp_str, (char) c);
                }

            }
            CODELN("string@", tmp_str.buff, "\n");
            break;

        case ID:
            CODELN("LF@", current_token.data.s, "\n");

        default:
            break;
    }
    dynstr_free(&tmp_str);
    return true;
}

/**
 * @brief Generate needed header and alloc code buffer
 */
bool gen_header()
{
    dynstr_init(&code);

    CODE(".IFJcode20\n");
    index = malloc(sizeof(char));
    index2 = malloc(sizeof(char));
    els = malloc(sizeof(char));
    for_index = malloc(sizeof(char));
    if (index == NULL || els == NULL || for_index == NULL || index2 == NULL)
    {
        intern_error();
    }
    *index = 64;
    *index2 = 64;
    index_stack = createStack(100);
    for_index_stack = createStack(100);
    // constants needed, to add
    CODE("DEFVAR GF@_\n");
    CODE("DEFVAR GF@expr_result\n");
    CODE("MOVE GF@expr_result bool@true\n");
    CODE("JUMP $main\n");

    return true;
}

/**
 * @brief Generates beginning of main scope
 * @return True if code generation successful, otherwise false
 */
bool gen_main_start()
{
    CODE("LABEL $main\n");
    CODE("CREATEFRAME\n");
    CODE("PUSHFRAME\n");
    return true;
}

/**
 * @brief Generate function header
 */
bool gen_func_header(char *func_name)
{
    CODELN("LABEL $", func_name, "\n");
    //CODE("CREATEFRAME\n");    // vymazali by sme TF s posielajucimi parametrami
    CODE("PUSHFRAME\n");
    return true;
}

/**
 * @brief Generates definition of variables which will store ret value of function
 * @param ret_counter Determines how many ret values we will need
 * @return True if code generation successful, otherwise false
 */
bool gen_func_retval(unsigned ret_counter)
{
    for (unsigned i = 0; i < ret_counter; i++)
    {
        CODE("DEFVAR LF@%retval"); CODE_INT(i+1); CODE("\n");
    }
    return true;
}

/**
 * @brief Generates assignment of expr_result to retval of function
 * @param ret_counter Determines how many ret values we will need
 * @return True if code generation successful, otherwise false
 */
bool gen_func_return(char *func_id, unsigned ret_counter)
{
    for (unsigned i = 0; i < ret_counter; i++)
    {
        //HERE call for expression result
        CODE("MOVE LF@%retval"); CODE_INT(i+1); CODE(" GF@expr_result\n");
    }
    CODELN("JUMP $", func_id, "&return", "\n");
    return true;
}

/**
 * @brief Generates assignment of return values to vars on left side
 * @param id Id of var on left side
 * @param retval_index Index of return value
 * @return True if code generation successful, otherwise false
 */
bool gen_retval_assign(char *id, unsigned retval_index)
{
    CODE("MOVE LF@"); CODE(id); CODE(" TF@%retval"); CODE_INT(retval_index); CODE("\n");
    return true;
}

/**
 * @brief Generates calling of function
 * @param func_id Stores id of function to be called
 * @return True if code generation successful, otherwise false
 */
bool gen_func_call(char *func_id)
{
    CODELN("CALL $", func_id, "\n");
    return true;
}

/**
 * @brief Generates passing of values to params in function
 * @param current_token Token which represents current parameter
 * @param param_index Stores index of parameter
 * @return True if code generation successful, otherwise false
 */
bool gen_param_pass(Token current_token, int param_index)
{
    CODE("DEFVAR TF@%param"); CODE_INT(param_index); CODE("\n");
    CODE("MOVE TF@%param"); CODE_INT(param_index); CODE(" ");
    if (!gen_param_val(current_token))
    {
        return false;
    }
    return true;
}

bool gen_inputs()
{
    CODE("LABEL $inputs\n");
    gen_func_retval(2);
    CODE("READ LF@%retval1 string\n");
    CODE("DEFVAR LF@%str_len\n");
    CODE("STRLEN LF@%str_len LF@%retval1\n");
    CODE("JUMPIFNEQ $inputs_end LF@%str_len int@0\n");
    CODE("MOVE LF@%retval2 int@1\n");
    CODE("LABEL $inputs_end\n");
    CODE("RETURN\n");
    return true;
}

// bool gen_inputs_call()
// {
//     CODE("JUMP inputs\n");
//     return true;
// }

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
    CODELN("LABEL $if", index, "\n");
    return true;
}

/**
 * @brief Generate jump over else branch to end of if
 */
bool if_jump()
{
    *els = top(index_stack);
    CODELN("JUMP $if_end", els, "\n");
    return true;
}

/**
 * @brief Generate jump over if branch to else
 */
bool else_jump()
{
    CODELN("JUMPIFEQ $else", index, " GF@expr_result bool@false", "\n");
    return true;
}

/**
 * @brief Generate else branch
 */
bool else_label()
{
    *els = top(index_stack);
    CODELN("LABEL $else", els, "\n");
    return true;
}

/**
 * @brief Generate end of if
 */
bool if_end_label()
{
    *els = top(index_stack);
    CODELN("LABEL $if_end", els, "\n");
    pop(index_stack);
    return true;
}

/**
 * @brief Generate for header
 */
bool for_header()
{
    (*index2)++;
    push(for_index_stack, *index2);
    *for_index = top(for_index_stack);
    CODELN("LABEL $for", for_index, "\n");
    CODELN("LABEL $condition", for_index, "\n");
    return true;
}


/**
 * @brief Generate for header
 */
bool for_condition_eval()
{
    *for_index = top(for_index_stack);
    CODELN("JUMPIFEQ $end_for", for_index," GF@expr_result bool@true", "\n");
    CODELN("JUMP $for_body", for_index, "\n");
    CODELN("LABEL $increment", for_index, "\n");
    return true;
}

/**
 * @brief Generate for header
 */
bool for_body()
{
    *for_index = top(for_index_stack);
    CODELN("JUMP $condition", for_index, "\n");
    CODELN("LABEL $for_body", for_index, "\n");
    return true;
}

/**
 * @brief Generate for header
 */
bool for_end()
{
    *for_index = top(for_index_stack);
    CODELN("JUMP $increment", for_index, "\n");
    CODELN("LABEL $end_for", for_index, "\n");
    pop(for_index_stack);
    return true;
}

/**
 * @brief Defines new var in LF
 * @param id Id ov new var
 * @return True if generationa successful, else false
 */
bool gen_var_def(char *id)
{
    CODELN("DEFVAR LF@", id, "\n");
    CODELN("MOVE LF@", id, " GF@expr_result", "\n");
    return true;
}

/**
 * @brief Generate main end
 */
bool gen_main_end()
{
    CODE("POPFRAME\n");
    CODE("CLEARS\n");
    CODE("JUMP $end_of_code\n");
    return true;
}

/**
 * @brief Generate function end
 */
bool gen_func_end(char *func_id)
{
    CODELN("LABEL $", func_id, "&return", "\n");
    CODE("POPFRAME\n");
    CODE("RETURN\n");
    return true;
}

/**
 * @brief Generate label where to jump after main end, so other functions are not executed
 */
bool gen_code_end()
{
    CODE("LABEL $end_of_code\n");
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
    free(for_index);
}

/**
 * @brief Puts code buffer to stdout
 */
void flush()
{
    puts(code.buff);
    gen_dispose();
}