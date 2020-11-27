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

#define MAX 100

#define CODE_INT(_code) \
    do{ \
        char index[MAX_INDEX_LEN];   \
        sprintf(index, "%d", _code);    \
        CODE(index); \
    }while(0)

#define GEN_INT2FLAOT   \
    "LABEL $int2float\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "INT2FLOAT LF@%retval1 LF@%param1\n"    \
    "POPFRAME\n" \
    "RETURN\n"  \
    "\n"

#define GEN_FLOAT2INT   \
    "LABEL $float2int\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "FLOAT2INT LF@%retval1 LF@%param1\n"    \
    "POPFRAME\n" \
    "RETURN\n" \
    "\n"

#define GEN_LEN   \
    "LABEL $len\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "STRLEN LF@%retval1 LF@%param1\n"    \
    "POPFRAME\n" \
    "RETURN\n"  \
    "\n"

#define GEN_INPUTS1  \
    "LABEL $inputs\n" \
    "PUSHFRAME\n"

#define GEN_INPUTS2  \
    "READ LF@%retval1 string\n" \
    "DEFVAR LF@%str_len\n" \
    "STRLEN LF@%str_len LF@%retval1\n" \
    "JUMPIFNEQ $inputs_end LF@%str_len int@0\n" \
    "MOVE LF@%retval2 int@1\n" \
    "LABEL $inputs_end\n" \
    "POPFRAME\n" \
    "RETURN\n"  \
    "\n"

#define GEN_SUBSTR   \
    "LABEL $substr\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "DEFVAR LF@%retval2\n"    \
    "MOVE LF@%retval2 int@0\n"  \
    "MOVE LF@%retval1 string@\n"   \
    "DEFVAR LF@str_len\n"       \
    "DEFVAR LF@condition\n"     \
    "DEFVAR LF@loop_condition\n"    \
    "DEFVAR LF@max_n\n"         \
    "DEFVAR LF@char\n"          \
    "CREATEFRAME\n"             \
    "DEFVAR TF@%param1\n"         \
    "MOVE TF@%param1 LF@%param1\n"    \
    "CALL $len\n"                   \
    "MOVE LF@str_len TF@%retval1\n" \
    "JUMPIFEQ $substr&return LF@str_len int@0\n"    \
    "LT LF@condition LF@%param2 int@0\n"      \
    "JUMPIFEQ $substr_error LF@condition bool@true\n"   \
    "GT LF@condition LF@%param2 LF@str_len\n"   \
    "JUMPIFEQ $substr_error LF@condition bool@true\n"   \
    "EQ LF@condition LF@%param2 LF@str_len\n"   \
    "JUMPIFEQ $substr_error LF@condition bool@true\n"   \
    "LT LF@condition LF@%param3 int@0\n"      \
    "JUMPIFEQ $substr_error LF@condition bool@true\n"   \
    "MOVE LF@max_n LF@str_len\n"    \
    "SUB LF@max_n LF@max_n LF@%param2\n"    \
    "GT LF@condition LF@%param3 LF@max_n\n" \
    "JUMPIFNEQ $substr_loop LF@condition bool@true\n"   \
    "MOVE LF@%param3 LF@max_n\n"    \
    "LABEL $substr_loop\n"          \
    "GETCHAR LF@char LF@%param1 LF@%param2\n"      \
    "CONCAT LF@%retval1 LF@%retval1 LF@char\n"       \
    "ADD LF@%param2 LF@%param2 int@1\n"             \
    "SUB LF@%param3 LF@%param3 int@1\n"             \
    "GT LF@loop_condition LF@%param3 int@0\n"       \
    "JUMPIFEQ $substr_loop LF@loop_condition bool@true\n"   \
    "JUMP $substr&return\n"         \
    "LABEL $substr_error\n"         \
    "MOVE LF@%retval2 int@1\n"      \
    "LABEL $substr&return\n"        \
    "POPFRAME\n" \
    "RETURN\n"  \
    "\n"

dynamic_string code; // code will be stored here and flushed in the end of compilation to stdout if compilation went successful
char *index;
char *els;
Stack *index_stack;
char *for_index;
char *index2;
Stack *for_index_stack;

/**
 * @brief Generates all built_in functions
 */
bool gen_built_func()
{
    CODE(GEN_INT2FLAOT);
    CODE(GEN_FLOAT2INT);
    CODE(GEN_LEN);
    CODE(GEN_INPUTS1);
    gen_func_retval(2);
    CODE(GEN_INPUTS2);
    CODE(GEN_SUBSTR);
    return true;
}

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
                    if (c != '"') add_char(&tmp_str, (char) c);
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
    CODE("DEFVAR GF@tmp1\n");
    CODE("DEFVAR GF@tmp2\n");
    CODE("MOVE GF@expr_result bool@true\n");
    CODE("JUMP $main\n");
    CODE("\n");

    return true;
}

/**
 * @brief Generates beginning of main scope
 * @return True if code generation successful, otherwise false
 */
bool gen_main_start()
{
    CODE("LABEL $main\n");
    CODE(" CREATEFRAME\n");
    CODE(" PUSHFRAME\n");
    return true;
}

/**
 * @brief Generate function header
 */
bool gen_func_header(char *func_name)
{
    CODELN("LABEL $", func_name, "\n");
    //CODE("CREATEFRAME\n");    // vymazali by sme TF s posielajucimi parametrami
    CODE(" PUSHFRAME\n");
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
        CODE(" DEFVAR LF@%retval"); CODE_INT(i+1); CODE("\n");
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
        CODE(" MOVE LF@%retval"); CODE_INT(i+1); CODE(" GF@expr_result\n");
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
    CODE(" MOVE LF@"); CODE(id); CODE(" TF@%retval"); CODE_INT(retval_index); CODE("\n");
    return true;
}

/**
 * @brief Generates calling of function
 * @param func_id Stores id of function to be called
 * @return True if code generation successful, otherwise false
 */
bool gen_func_call(char *func_id)
{
    CODELN(" CALL $", func_id, "\n");
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
    CODE(" DEFVAR TF@%param"); CODE_INT(param_index); CODE("\n");
    CODE(" MOVE TF@%param"); CODE_INT(param_index); CODE(" ");
    if (!gen_param_val(current_token))
    {
        return false;
    }
    return true;
}

/**
 * @brief Genrates build_in function print
 */
bool gen_print(Token current_token, unsigned act_param_counter)
{
    if (act_param_counter == 1) CODE("DEFVAR TF@%param\n");
    CODE("MOVE TF@%param ");
    if (!gen_param_val(current_token)) return false;
    CODE("WRITE TF@%param\n");
    
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
    CODELN("JUMPIFEQ $end_for", for_index," GF@expr_result bool@false", "\n");
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
 * @param id Id of new var
 * @return True if generation successful, else false
 */
bool gen_var_def(char *id)
{
    CODELN(" DEFVAR LF@", id, "\n");
    CODELN(" MOVE LF@", id, " GF@expr_result", "\n");
    return true;
}

bool gen_var_ass(char *id)
{
    CODELN(" MOVE LF@", id, " GF@expr_result", "\n");
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
    CODE("\n");
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
    CODE("\n");

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
 * @brief casts double number @param d into hex string @param buf
 */
void float2hex(double d, char *buf)
{
    sprintf(buf, "%a", d);
}

/**
 * @brief casts integer number @param n into string @param buf
 */
void int2str(int n, char *buf)
{
    sprintf(buf, "%d", n);
}

/**
 * @brief edits @param src string into @param dst string which is needed in 3AC
 */
void str2our_str(char *dst, char *src)
{
    unsigned j = 0;
    bool escape = false;
    for (unsigned i = 1; i < strlen(src)-1; i++, j++)
    {
        if ((src[i] >= 0 && src[i] <= 32 )|| src[i] == 34 || src[i] == 35 || src[i] == 92)
        {
            if (escape)// \", \\ ,
            {
                escape = false;
                if (src[i] == '\\') src[i] = '\\';
                if (src[i] == '\"') src[i] = '\"';
            }
            else if (src[i] == 92)
            {
                escape = true;
                j--;
                continue;
            }

            char buf[3];
            dst[j++] = '\\'; dst[j++] = '0';
            int2str(src[i],buf);
            if(strlen(buf) == 1)
            {
                dst[j++] = '0';
                dst[j] = buf[0];
            }
            if(strlen(buf) == 2)
            {
                dst[j++] = buf[0];
                dst[j] = buf[1];
            }
        }
        else if (escape) // \n, \t
        {
            escape = false;
            if (src[i] == 'n') src[i] = '\n';
            if (src[i] == 't') src[i] = '\t';
            if (src[i] == 'x') // \xhh
            {
                char buf[3];
                buf[0] = src[++i];
                buf[1] = src[++i];
                int num = (int)strtol(buf, NULL, 16); // from hex to int
                int2str(num,buf);
                dst[j++] = '\\';
                if(strlen(buf) == 1)
                {
                    dst[j++] = '0';
                    dst[j++] = '0';
                    dst[j] = buf[0];
                }
                if(strlen(buf) == 2)
                {
                    dst[j++] = '0';
                    dst[j++] = buf[0];
                    dst[j] = buf[1];
                }
                if(strlen(buf) == 3)
                {
                    dst[j++] = buf[0];
                    dst[j++] = buf[1];
                    dst[j] = buf[2];
                }
                continue;
            }
            char buf[3];
            dst[j++] = '\\'; dst[j++] = '0';
            int2str(src[i],buf);
            if(strlen(buf) == 1)
            {
                dst[j++] = '0';
                dst[j] = buf[0];
            }
            if(strlen(buf) == 2)
            {
                dst[j++] = buf[0];
                dst[j] = buf[1];
            }
        }
        else
        {
            dst[j] = src[i];
        }
    }
}

// for debugging
bool gen_expr_begin()
{
    CODE("# EXPR BEGIN\n")
    return true;
}

// for debugging
bool gen_expr_end()
{
    CODE("# EXPR END\n")
    return true;
}

/**
 * @brief Generates term(string, int, float64, ID)
 * @param type is type of term in string
 * @param constant is the data/ value of term
 */
bool gen_term(char *type, char* constant)
{
    CODELN("  PUSHS ",type,"@",constant,"\n");
    return true;
}

/**
 * @brief Generates operation to be performed on stack
 * @param type should be Terminal_type, but codegenerator.h would have needed parser.h 
 * @param type is enum of the operation
 */
bool gen_operation(int type)
{
    if (type == 2)
        CODE("  ADDS\n");
    if (type == 3)
        CODE("  SUBS\n");
    if (type == 4)
        CODE("  MULS\n");
    if (type == 5) // !!!
        CODE("  DIVS\n");
    if (type == 6)
        CODE("  GTS\n");
    if (type == 7)
        CODE("  LTS\n");
    if (type == 8)
        CODE("  EQS\nNOTS\n");
    if (type == 9) //LES
    {
        CODE("  POPS GF@tmp1\n");
        CODE("  POPS GF@tmp2\n");
        CODE("  LTS\n");
        CODE("  PUSHS GF@tmp1\n");
        CODE("  PUSHS GF@tmp2\n");
        CODE("  EQS\n");
        CODE("  ORS\n");
    }

    if (type == 10) //GES
    {
        CODE("  POPS GF@tmp1\n");
        CODE("  POPS GF@tmp2\n");
        CODE("  GTS\n");
        CODE("  PUSHS GF@tmp1\n");
        CODE("  PUSHS GF@tmp2\n");
        CODE("  EQS\n");
        CODE("  ORS\n");
    }
    if (type == 11)
        CODE("  EQS\n");

    return true;
}

/**
 * @brief expression result is poped into global GF@expr_result, where others can read it form
 */
bool gen_expr_result()
{
    CODE("  POPS GF@expr_result\n");
    gen_expr_end();
    return true;
}

/**
 * @brief free allocated code buffers
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