/**
 * @file parser.c
 * @authors Rebeka Cernianska, Marek Micek, Matej Jurik, Peter Rucek
 * @date 25 Oct 2020
 * @brief Parser Implementation
 */

#include "parser.h"

#define GET_TOKEN()  m.actual_token = get_next_token(stdin); (m.actual_token.type == ERROR)? (lexical_error(m.actual_line), 1) : 0
#define CHECK_TOKEN(Type) ((m.actual_token.type == Type)? true : \
        (fprintf(stderr,"In function %s on line %d\n ",__func__, __LINE__) , syntax_error(Type,m.actual_line) , false))
#define CHECK_NO_ERROR(Type) ((m.actual_token.type == Type)? true : false)
#define GET_AND_CHECK(Type) do {GET_TOKEN(); CHECK_TOKEN(Type);}while(0)
#define IS_DATA_TYPE() (m.actual_token.type == KEYWORD && \
                       (m.actual_token.data.k == K_STRING ||\
                        m.actual_token.data.k == K_INT ||\
                        m.actual_token.data.k == K_FLOAT64))? true : (syntax_error(m.actual_token.type,m.actual_line),false)

//global
Metadata m = {.actual_line = 1, .index = 0, .local_table = NULL,};
TNode *node;

/**
 * @brief Inits data structure of symtable to eliminate multi. insertion of one symbol
 */
TData init_new_data(TData new_data)
{
    new_data.type = T_UNDEFINED;
    new_data.defined = new_data.is_var = new_data.is_function = new_data.in_block = false;
    new_data.param_counter = new_data.ret_counter = 0;

    for (int i = 0; i < MAX_RET_VAL; i++)
        new_data.retval_arr[i] = T_UNDEFINED;

    for (int j = 0; j < MAX_ARG; j++)
        new_data.arg_arr[j] = T_UNDEFINED;

    return new_data;
}

/**
 * @brief Syntax of program
 */
int program()
{
    m.local_table = init_symtable(m.local_table);
    prolog();
    while(func());
    node = search_symtable(m.global_table, "main");
    if (node == NULL)
    {
        no_definition_error("main", -1);    //< func main was not defined
    }

    delete_symtable(m.global_table);
    CHECK_TOKEN(EoF);
    delete_symtable(m.local_table);
    return error_value;
}

/**
 * @brief Checks syntax of function
 */
bool func()
{
    m.local_table = init_symtable(m.local_table);
    if(!func_header())
    {
        return false;
    }

    // body
    while(statement());

    func();

    delete_symtable(m.local_table);

    return true;
}

/**
 * @brief Checks syntax of function header
 */
bool func_header()
{
    // checking EOF
    if (!expect_token(KEYWORD,K_FUNC))
    {
        if(!CHECK_NO_ERROR(EoF))
        {
            syntax_error(KEYWORD,m.actual_line);
        }
        return false;
    }

    // function id
    GET_AND_CHECK(ID);
    // HERE add m.actual_token to symtable function id

    char *func_id = m.actual_token.data.s;  //< store func id cause insertion to symtable will be later
    if ((node = search_symtable(m.global_table, func_id)) != NULL)
    {
        re_definition_error(func_id, m.actual_line);    //< redefinition of func is forrbiden
    }

    new_data_func = init_new_data(new_data_func);
    new_data_func.defined = true;
    new_data_func.is_function = true;

    // function parameters
    free(m.actual_token.data.s);
    GET_AND_CHECK(PARENTHESIS_LEFT);
    GET_TOKEN();
    if (!CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    {
        header_arg();
        m.index = 0;
    }

    // function return values
    GET_TOKEN();
    bool inside = false;
    if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
    {
        header_ret();
        inside = true;
        m.index = 0;
    }

    // chcecking {EOL at the end of function header
    if (!inside)
    {
        CHECK_TOKEN(BRACKET_LEFT);
    }
    else if (inside)
    {
        GET_AND_CHECK(BRACKET_LEFT);
    }
    GET_AND_CHECK(EOL);
    if (!strcmp(func_id, "main"))
    {
        if (new_data_func.param_counter != 0 || new_data_func.ret_counter != 0)
        {
            param_error(func_id, m.actual_line);    //< func main cant have param or ret value
        }
    }

    m.actual_line++;
    m.global_table = insert_symtable(m.global_table, new_data_func, func_id);    //< insert whole func
    return true;
}

/**
 * @brief Checks syntax of seqeunce <data type> , <data type> ,...)
 */
void header_ret()
{
    GET_TOKEN();
    if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    {
        return;
    }
    if (IS_DATA_TYPE())
    // HERE add return value of function to symtable
    {
        new_data_func.ret_counter++;
        switch (m.actual_token.data.k)
        {
        case K_INT:
            new_data_func.retval_arr[m.index++] = T_INT;
            break;
        case K_FLOAT64:
            new_data_func.retval_arr[m.index++] = T_FLOAT64;
            break;
        case K_STRING:
            new_data_func.retval_arr[m.index++] = T_STRING;
            break;

        default:
            break;
        }
    }
    GET_TOKEN();

    if (CHECK_NO_ERROR(COMMA))
    {
        header_ret();
    }
    else if (CHECK_NO_ERROR(PARENTHESIS_RIGHT)) // recursion condition
    {
        return;
    }
     else
    {
        syntax_error(m.actual_token.type,m.actual_line);
    }
}

/**
 * @brief Checks syntax of seqeunce ID <data type> , ID <data type> ,... )
 */
void header_arg()
{
    // BONUS FUNEXP not on purpose
    // static int i = 0; // EOL cannot be before first parameter
    // if (CHECK_NO_ERROR(EOL) && i) // EOL can be after parameter
    // {
    //     m.actual_line++;
    //     GET_TOKEN();
    // }
    CHECK_TOKEN(ID);
    new_data_var = init_new_data(new_data_var);
    Token prev = m.actual_token;

    // HERE add parameters of function to symtable
    free(m.actual_token.data.s);
    GET_TOKEN();
    if (IS_DATA_TYPE())
    {
        new_data_var.defined = true;
        new_data_var.is_var = true;
        new_data_func.param_counter++;

        switch (m.actual_token.data.k)
        {
        case K_INT:
            new_data_var.type = T_INT;
            new_data_func.arg_arr[m.index++] = T_INT;
            m.global_table = insert_symtable(m.global_table, new_data_var, prev.data.s);     //< insert parameter
            break;
        case K_FLOAT64:
            new_data_var.type = T_FLOAT64;
            new_data_func.arg_arr[m.index++] = T_FLOAT64;
            m.global_table = insert_symtable(m.global_table, new_data_var, prev.data.s);     //< insert parameter
            break;
        case K_STRING:
            new_data_var.type = T_STRING;
            new_data_func.arg_arr[m.index++] = T_STRING;
            m.global_table = insert_symtable(m.global_table, new_data_var, prev.data.s);     //< insert parameter
            break;

        default:
            break;
        }
    }
    GET_TOKEN();

    if (CHECK_NO_ERROR(COMMA))
    {
        //i++; part of FUNEXP
        GET_TOKEN();
        header_arg();
    }
    else if (CHECK_NO_ERROR(PARENTHESIS_RIGHT)) // recursion condition
    {
        return;
    }
    else
    {
        syntax_error(m.actual_token.type,m.actual_line);
    }
}

/**
 * @brief Checks what statement has come
 * @return False if } was read, else true
 */
bool statement()
{

    // skip EOL's

    GET_TOKEN();
    while(CHECK_NO_ERROR(EOL))
    {
        m.actual_line++;
        GET_TOKEN();
    }

    if (CHECK_NO_ERROR(BRACKET_RIGHT))
    {
        return false;
    }
    else if (CHECK_NO_ERROR(KEYWORD))
    {
        if (m.actual_token.data.k == K_IF)
        {
            if_s();
        }
        else if (m.actual_token.data.k == K_FOR)
        {
            for_s();
        }
        else if (m.actual_token.data.k == K_RETURN)
        {
            return_s();
        }
        else
        {
            syntax_error(m.actual_token.type,m.actual_line);
        }
    }
    else if (CHECK_TOKEN(ID))
    {
        TData new_data;
        new_data.type = m.actual_token.type;
        new_data.is_var = true;
        new_data.in_block = true;

        char *id_name;
        id_name = malloc(sizeof(char)*strlen(m.actual_token.data.s));
        if (id_name == NULL)
        {
            intern_error();
        }

        for (unsigned int i = 0; i < strlen(m.actual_token.data.s); i++)
        {
            id_name[i] = m.actual_token.data.s[i];
        }
        free(m.actual_token.data.s);

        GET_TOKEN();
        int number_of_id = 1;
        TNode *tmp = NULL;
        // assignment to var statement with multiple ID
        if (CHECK_NO_ERROR(COMMA))
        {
            tmp = search_symtable(m.local_table, id_name);
            if (tmp == NULL)
            { //mozno je nelegalne
                no_definition_error(id_name, m.actual_line);
            }
            free(id_name);
            while(!CHECK_NO_ERROR(VAR_ASSIGN))
            {
                CHECK_TOKEN(COMMA);
                GET_AND_CHECK(ID);

                tmp = search_symtable(m.local_table, m.actual_token.data.s);
                if (tmp == NULL)
                { //mozno je nelegalne
                    //FREE_TOKEN_DATA();
                    no_definition_error(m.actual_token.data.s, m.actual_line);
                }
                free(m.actual_token.data.s);

                number_of_id++;
                GET_TOKEN();
            }

            assignment_s(number_of_id);
        }
        // definition of var statement
        else if (CHECK_NO_ERROR(DEF_OF_VAR))
        {
            tmp = search_symtable(m.local_table, id_name);
            if (tmp == NULL)
            {
                m.local_table = insert_symtable(m.local_table, new_data, id_name);
                //free(id_name);
            }
            else
            {
                //FREE_TOKEN_DATA();
                re_definition_error(id_name, m.actual_line);

            }
            free(id_name);
            expression();
            GET_AND_CHECK(EOL);
            m.actual_line++;
        }
        // assignment to var statement
        else if (CHECK_NO_ERROR(VAR_ASSIGN))
        {
            tmp = search_symtable(m.local_table, id_name);
            if (tmp == NULL)
            {
                //FREE_TOKEN_DATA();
                no_definition_error(id_name, m.actual_line);
            }
            free(id_name);
            assignment_s(number_of_id);
        }
        else if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
        {
            function_call();
        }
        else
        {
            syntax_error(m.actual_token.type,m.actual_line);
        }
        //free(id_name);
    }

    return true;
}

/**
 * @brief Checks syntax of function call
 */
void function_call()
{
    Token_type previous = PARENTHESIS_LEFT;
    while(true)
    {
        GET_TOKEN();
        if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
        {
            return;
        }
        else if (CHECK_NO_ERROR(COMMA) && previous != COMMA)
        {
            previous = COMMA;
            continue;
        }
        // EOL's not implemented, not sure if FUNEXP or obligatory
        // can only be terms
        // TODO potrebujem dvojfazov7 priechod...funkcia uz musi byt v symtable
        else if (CHECK_NO_ERROR(INT))
        {
            previous = INT;
            continue;
        }
        else if (CHECK_NO_ERROR(FLOAT64))
        {
            previous = FLOAT64;
            continue;
        }
        else if (CHECK_NO_ERROR(STRING))
        {
            free(m.actual_token.data.s);
            previous = STRING;
            continue;
        }
        else if (CHECK_NO_ERROR(ID))
        {
            free(m.actual_token.data.s);
            previous = ID;
            continue;
        }
        else
        {
            syntax_error(m.actual_token.type,m.actual_line);
        }
    }
}

/**
 * @brief Checks syntax of if statement
 */
void if_s()
{
    // if
    // m.local_table = init_symtable(m.local_table);
    expression();
    GET_AND_CHECK(BRACKET_LEFT);
    GET_AND_CHECK(EOL);
    m.actual_line++;
    while(statement());

    // else
    GET_AND_CHECK(KEYWORD);
    if (m.actual_token.data.k != K_ELSE)
    {
        syntax_error(m.actual_token.type,m.actual_line);
    }
    GET_AND_CHECK(BRACKET_LEFT);
    GET_AND_CHECK(EOL);
    m.actual_line++;
    while(statement());
    GET_AND_CHECK(EOL);
    m.actual_line++;
    // if (m.local_table != NULL)
    // {
    //     delete_symtable(m.local_table);
    // }
}

/**
 * @brief Checks syntax of assignment to var statement
 * @param number_of_id is number of ID before =
 */
void assignment_s(int number_of_id)
{
    for (int i = 1; i <= number_of_id; i++)
    {
        expression();
        if (i == number_of_id)
        {
            break;
        }
        GET_AND_CHECK(COMMA);
    }
    GET_AND_CHECK(EOL);
    m.actual_line++;
}

/**
 * @brief Checks syntax of for statement
 */
void for_s()
{
    // inicialization (can be epmty)
    //m.local_table = init_symtable(m.local_table);
    GET_TOKEN();
    if(CHECK_NO_ERROR(ID))
    {
        free(m.actual_token.data.s);
        GET_AND_CHECK(DEF_OF_VAR);
        expression();
        GET_AND_CHECK(SEMICLN);
    }
    else if (CHECK_TOKEN(SEMICLN)){;}

    // condition
    expression();
    GET_AND_CHECK(SEMICLN);

    // increment / decrement (can be epmty)
    GET_TOKEN();
    if(CHECK_NO_ERROR(ID))
    {
        free(m.actual_token.data.s);
        GET_AND_CHECK(VAR_ASSIGN);
        expression(); //only one ???
        GET_AND_CHECK(BRACKET_LEFT);
    }
    else if (CHECK_TOKEN(BRACKET_LEFT)){;}

    GET_AND_CHECK(EOL);
    m.actual_line++;

    // body
    while(statement());
    //delete_symtable(m.local_table);
}

/**
 * @brief Checks syntax of return statement
 */
void return_s()
{
    while (true)
    {
        expression();
        GET_TOKEN();
        if (CHECK_NO_ERROR(COMMA))
        {
            continue;
        }
        else if (CHECK_NO_ERROR(EOL))
        {
            m.actual_line++;
            break;
        }
        else
        {
            syntax_error(m.actual_token.type,m.actual_line);
        }
    }
}

/**
 * @brief Checks syntax of expressions
 */
void expression()
{
    // Wimko TODO
    // for now
    // IMPORTANT when reading ID and then "(" call function call and return
    GET_TOKEN();
    if (CHECK_NO_ERROR(ID))
    {
        free(m.actual_token.data.s);
    }
    else (CHECK_TOKEN(INT));
}

/**
 * @brief Checks syntax of the prolog
 */
void prolog()
{
    if (!expect_token(KEYWORD, K_PACKAGE))
    {
        syntax_error(KEYWORD,m.actual_line);
    }

    if (!expect_token(ID,K_ERROR) || 0) // TODO access symbol table t.data.s == main
    {
        syntax_error(m.actual_token.type,m.actual_line);
    }
}

/**
 * @brief Skip EOL's and expect certain token to come
 * @param t_type is token type to expect
 * @param k is expected Keyword, if expecting other than Keyword, set to K_ERROR
 * @return true if token came, else false
 */
bool expect_token(Token_type t_type, Keyword k)
{
    while(true)
    {
        GET_TOKEN();
        if (m.actual_token.type == t_type && ((k == K_ERROR)? true : m.actual_token.data.k == k))
        {
            return true;
        }
        else if (m.actual_token.type == EOL)
        {
            m.actual_line++;
            continue;
        }
        else
        {
            return false;
        }
    }
}