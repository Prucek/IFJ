/**
 * @file codegenerator.c
 * @authors Peter Rucek ...
 * @date 13 Nov 2020
 * @brief Imlemenatation of code generator
 */

#include "codegenerator.h"

#define CODE(_code) if(!add_string(&code,(_code)))return false;

// Variadic args MUST be strings (otherwise segfault) & last arg must be "\n" (otherwise infinite cycle)
#define CODELN(...) if(!add_strings(&code, ##__VA_ARGS__)) return false;

#define MAX 100

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
 * @brief Generate function header
 */
bool gen_func_header(char *func_name)
{
    // CODE("LABEL "); CODE(func_name); CODE("\n"); // Old
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


void float2hex(double d, char *buf) 
{
    sprintf(buf, "%a", d);
}

void int2str(int n, char *buf)
{
    sprintf(buf, "%d", n);
}

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

bool gen_expr_begin()
{
    CODE("# EXPR BEGIN\n")
    return true;
}

bool gen_expr_end()
{
    CODE("# EXPR END\n")
    return true;
}

bool gen_term(char *type, char* constant)
{
    CODELN("PUSHS ",type,"@",constant,"\n");
    return true;
}

bool gen_operation(int type)
{
    if (type == 2) 
        CODE("ADDS\n");
    if (type == 3)
        CODE("SUBS\n");
    if (type == 4)
        CODE("MULS\n");
    if (type == 5) // !!!
        CODE("DIVS\n");
    if (type == 6)
        CODE("GTS\n");
    if (type == 7)
        CODE("LTS\n");
    if (type == 8)
        CODE("EQS\nNOTS\n");
    if (type == 9) //LES
        CODE("\n");
    if (type == 10) //GES
        CODE("\n");
    if (type == 11)
        CODE("EQS\n");
    
    return true;
}

bool gen_expr_result()
{
    CODE("POPS GF@expr_result\n");
    gen_expr_end();
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
 * @brief Puts code buffer to stdout
 */
void flush()
{
    puts(code.buff);
    gen_dispose();
}