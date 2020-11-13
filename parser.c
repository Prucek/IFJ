/**
 * @file parser.c
 * @authors Rebeka Cernianska, Marek Micek, Matej Jurik, Peter Rucek
 * @date 25 Oct 2020
 * @brief Parser Implementation
 */

#include "parser.h"

#define GET_TOKEN()  do{ m.current_token = get_next_token(stdin); \
                    (m.current_token.type == EOL)? m.current_line++ : 0 ; \
                    (m.current_token.type == ERROR)? (lexical_error(m.current_line), 1) : 0 ;}while(0)
#define CHECK_TOKEN(Type) ((m.current_token.type == Type)? true : \
        (fprintf(stderr,"In function %s on line %d\n ",__func__, __LINE__) , syntax_error(Type,m.current_line) , false))
#define CHECK_NO_ERROR(Type) ((m.current_token.type == Type)? true : false)
#define GET_AND_CHECK(Type) do {GET_TOKEN(); CHECK_TOKEN(Type);}while(0)
#define IS_DATA_TYPE() (m.current_token.type == KEYWORD && \
                       (m.current_token.data.k == K_STRING ||\
                        m.current_token.data.k == K_INT ||\
                        m.current_token.data.k == K_FLOAT64))? true : (syntax_error(m.current_token.type,m.current_line),false)

//global
Metadata m = {.current_line = 1, .index = 0, .local_table = NULL, .suspected = NULL};
TNode *node;
TNode *array_of_trees[100];
int tree_index = -1;

/**
 * @brief Inits data structure of symtable to eliminate multi. insertion of one symbol
 * @param new_data Data unit that will be initionalized
 * @return Initionalized data ready for next use
 */
TData init_new_data(TData new_data)
{
    new_data.type = T_UNDEFINED;
    new_data.defined = new_data.is_var = new_data.is_function = new_data.in_block = false;
    new_data.param_counter = new_data.ret_counter = new_data.line = 0;

    for (int i = 0; i < MAX_RET_VAL; i++)
        new_data.retval_arr[i] = T_UNDEFINED;

    for (int j = 0; j < MAX_ARG; j++)
        new_data.arg_arr[j] = T_UNDEFINED;

    return new_data;
}

/**
 * @brief Checks semantics of funcs suspected from no_definition
 */
void check_suspected(TNode *root)
{
    // using PostOrder
    if (root != NULL)
    {
        check_suspected(root->lptr);
        check_suspected(root->rptr);

        if ((node = search_symtable(m.global_table, root->key)) == NULL)    //< func is not defined
        {
            no_definition_error(root->key, root->data.line);
        }
        else  //< func is defined, check params
        {
            if (node->data.param_counter != root->data.param_counter)   //< num of params not same
            {
                param_error(root->key, root->data.line);
            }
            else
            {
                for (unsigned i = 0; i < node->data.param_counter; i++)
                {
                    if (node->data.arg_arr[i] != root->data.arg_arr[i])     //< data type not same
                    {
                        param_error(root->key, root->data.line);
                        break;
                    }
                }
            }
        }
    }
}

void add_tree(TNode *root)
{
    tree_index++;
    array_of_trees[tree_index] = NULL;
    array_of_trees[tree_index] = malloc(sizeof(TNode *));
    if (array_of_trees[tree_index] == NULL)
    {
        intern_error();
    }
    array_of_trees[tree_index] = root;
}

bool search_all_trees(char *key)
{
    for (int i = 0; i <= tree_index; i++)
    {
        if ((node = search_symtable(array_of_trees[i], key)) != NULL)
        {

            return true;
        }
    }
    return false;
}

void delete_tree() //deletes youngest tree
{
    array_of_trees[tree_index] = NULL;
    tree_index--;
}

/**
 * @brief Syntax of program
 */
int program()
{
    m.global_table = init_symtable(m.global_table);
    prolog();
    while(func());
    node = search_symtable(m.global_table, "main");
    if (node == NULL)
    {
        no_definition_error("main", -1);    //< func main was not defined
    }
    if (m.suspected != NULL)    //< check funcs suspected from no_definition
    {
        check_suspected(m.suspected);
        delete_symtable(m.suspected);
    }

    delete_symtable(m.global_table);

    for (int i = 0; i < 100; i++)
    {
        if (array_of_trees[i] != NULL)
        {
            delete_symtable(array_of_trees[i]);
        }
    }
    
    CHECK_TOKEN(EoF);
    return error_value;
}

/**
 * @brief Checks syntax of function
 */
bool func()
{
    TNode *root = NULL;
    root = init_symtable(root);
    add_tree(root);
    TData new_data;
    new_data.type = ID;
    new_data.defined = true;
    new_data.is_var = true;
    new_data.is_function = false;
    new_data.in_block = true;

    array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data, "_");

    if(!func_header())
    {
        return false;
    }

    // body
    while(statement());

    delete_symtable(array_of_trees[tree_index]);
    delete_symtable(root);
    delete_tree();
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
            syntax_error(KEYWORD,m.current_line);
        }
        return false;
    }

    // function id
    GET_AND_CHECK(ID);
    // HERE add m.actual_token to symtable function id

    char *func_id = m.current_token.data.s;  //< store func id cause insertion to symtable will be later
    if ((node = search_symtable(m.global_table, func_id)) != NULL)
    {
        re_definition_error(func_id, m.current_line);    //< redefinition of func is forrbiden
    }

    new_data_func = init_new_data(new_data_func);
    new_data_func.defined = true;
    new_data_func.is_function = true;
    Token last_func = m.current_token;   //< to make correct free later

    // function parameters
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
            param_error(func_id, m.current_line);    //< func main cant have param or ret value
        }
    }

    m.global_table = insert_symtable(m.global_table, new_data_func, func_id);    //< insert whole func

    free(last_func.data.s);     //< to free token id
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
        //add m.actual_token.data.k to local_table for reference in the current block
        switch (m.current_token.data.k)
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
        syntax_error(m.current_token.type,m.current_line);
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
    //     m.current_line++;
    //     GET_TOKEN();
    // }
    CHECK_TOKEN(ID);
    new_data_var = init_new_data(new_data_var);
    Token prev = m.current_token;

    // HERE add parameters of function to symtable
    GET_TOKEN();
    if (IS_DATA_TYPE())
    {
        new_data_var.defined = true;
        new_data_var.is_var = true;
        new_data_func.param_counter++;

        switch (m.current_token.data.k)
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
    free(prev.data.s);      //< free token id
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
        syntax_error(m.current_token.type,m.current_line);
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
        GET_TOKEN();
    }

    if (CHECK_NO_ERROR(BRACKET_RIGHT))
    {
        return false;
    }
    else if (CHECK_NO_ERROR(KEYWORD))
    {
        if (m.current_token.data.k == K_IF)
        {
            if_s();
        }
        else if (m.current_token.data.k == K_FOR)
        {
            for_s();
        }
        else if (m.current_token.data.k == K_RETURN)
        {
            return_s();
        }
        else
        {
            syntax_error(m.current_token.type,m.current_line);
        }
    }
    else if (CHECK_TOKEN(ID))
    {
        Token last_id = m.current_token;     //< need to store token's id in case of function call
        TData new_data;
        new_data.type = m.current_token.type;
        new_data.is_var = true;
        new_data.in_block = true;
        new_data.defined = true;
        new_data.is_function = false;

        char *id_name;
        id_name = m.current_token.data.s;

        GET_TOKEN();
        int number_of_id = 1;
        bool tmp;
        // assignment to var statement with multiple ID
        if (CHECK_NO_ERROR(COMMA))
        {
            tmp = search_all_trees(id_name);

            if (tmp == false)
            { //mozno je nelegalne
                no_definition_error(id_name, m.current_line);
            }
            while(!CHECK_NO_ERROR(VAR_ASSIGN))
            {
                if (!CHECK_TOKEN(COMMA))
                {
                    break;
                }
                GET_TOKEN();
                if (!CHECK_TOKEN(ID))
                {
                    break;
                }

                tmp = search_all_trees(m.current_token.data.s);
                if (tmp == false)
                {
                    no_definition_error(m.current_token.data.s, m.current_line);
                }

                number_of_id++;
                GET_TOKEN();
            }

            assignment_s(number_of_id);
        }
        // definition of var statement
        else if (CHECK_NO_ERROR(DEF_OF_VAR))
        {
            if (!strcmp(id_name, "_"))
            {
                other_error();
            }
            tmp = search_symtable(array_of_trees[tree_index], id_name);
            if (tmp == false)
            {
                new_data.defined = true;
                array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data, id_name);
            }
            else
            {
                re_definition_error(id_name, m.current_line);
            }

            expression();
            GET_AND_CHECK(EOL);
        }
        // assignment to var statement
        else if (CHECK_NO_ERROR(VAR_ASSIGN))
        {
            tmp = search_all_trees(id_name);
            if (tmp == false)
            {
                no_definition_error(id_name, m.current_line);
            }

            assignment_s(number_of_id);
        }
        else if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
        {
            function_call(last_id);
            m.index = 0;
        }
        else
        {
            syntax_error(m.current_token.type,m.current_line);
        }

        free(last_id.data.s);   //< free token's id
    }

    return true;
}

/**
 * @brief Checks syntax of function call
 * @param id Stores the id of calling function
 */
void function_call(Token id)
{
    Token_type previous = PARENTHESIS_LEFT;
    unsigned act_param_counter = 0;
    bool sem_error = false;                     //< indicates semantic error
    new_data_func = init_new_data(new_data_func);
    new_data_func.is_function = true;
    new_data_func.line = m.current_line;

    while(true)
    {
        GET_TOKEN();
        if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
        {
            if ((node = search_symtable(m.global_table, id.data.s)) != NULL)    //< func is defined for sure
            {
                if (node->data.param_counter != act_param_counter)      //< num of params not same
                {
                    sem_error = true;
                }
            }
            else        //< not sure whether func defined
            {
                m.suspected = insert_symtable(m.suspected, new_data_func, id.data.s);   //< check this func after whole file read
            }

            break;
        }
        else if (CHECK_NO_ERROR(COMMA) && previous != COMMA)
        {
            previous = COMMA;
            continue;
        }
        // EOL's not implemented, not sure if FUNEXP or obligatory
        // can only be terms
        // TODO potrebujem dvojfazovy priechod...funkcia uz musi byt v symtable
        else if (CHECK_NO_ERROR(INT))
        {
            act_param_counter++;
            if ((node = search_symtable(m.global_table, id.data.s)) != NULL)    //< func is defined for sure
            {
                if (node->data.arg_arr[m.index++] != T_INT)     //< data type of calling func not same
                {
                    sem_error = true;
                }
            }
            else        //< not sure whether func defined
            {
                new_data_func.param_counter++;
                new_data_func.arg_arr[m.index++] = T_INT;

            }   // end of semantic analysis

            previous = INT;
            continue;
        }
        else if (CHECK_NO_ERROR(FLOAT64))
        {
            act_param_counter++;
            if ((node = search_symtable(m.global_table, id.data.s)) != NULL)    //< func is defined for sure
            {
                if (node->data.arg_arr[m.index++] != T_FLOAT64)     //< data type of calling func not same
                {
                    sem_error = true;
                }
            }
            else        //< not sure whether func defined
            {
                new_data_func.param_counter++;
                new_data_func.arg_arr[m.index++] = T_FLOAT64;

            }   // end of semantic analysis

            previous = FLOAT64;
            continue;
        }
        else if (CHECK_NO_ERROR(STRING))
        {
            act_param_counter++;
            if ((node = search_symtable(m.global_table, id.data.s)) != NULL)    //< func is defined for sure
            {
                if (node->data.arg_arr[m.index++] != T_STRING)     //< data type of calling func not same
                {
                    sem_error = true;
                }
            }
            else        //< not sure whether func defined
            {
                new_data_func.param_counter++;
                new_data_func.arg_arr[m.index++] = T_STRING;

            }   // end of semantic analysis

            free(m.current_token.data.s);
            previous = STRING;
            continue;
        }
        //TODO prienik s Rebekinou castou
        else if (CHECK_NO_ERROR(ID))
        {
            free(m.current_token.data.s);
            previous = ID;
            continue;
        }
        else
        {
            syntax_error(m.current_token.type,m.current_line);
        }
    }
    if (sem_error)
    {
        param_error(id.data.s, m.current_line);
    }
}

/**
 * @brief Checks syntax of if statement
 */
void if_s()
{
    TNode *root = NULL;
    root = init_symtable(root);
    add_tree(root);

    expression();
    GET_AND_CHECK(BRACKET_LEFT);
    GET_AND_CHECK(EOL);

    while(statement());

    // else
    GET_AND_CHECK(KEYWORD);
    if (m.current_token.data.k != K_ELSE)
    {
        syntax_error(m.current_token.type,m.current_line);
    }
    GET_AND_CHECK(BRACKET_LEFT);
    GET_AND_CHECK(EOL);

    while(statement());
    GET_AND_CHECK(EOL);

    delete_symtable(array_of_trees[tree_index]);
    delete_tree();
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
}

/**
 * @brief Checks syntax of for statement
 */
void for_s()
{
    TNode *root = NULL;
    root = init_symtable(root);
    add_tree(root);

    GET_TOKEN();
    if(CHECK_NO_ERROR(ID))
    {
        free(m.current_token.data.s);
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
        free(m.current_token.data.s);
        GET_AND_CHECK(VAR_ASSIGN);
        expression(); //only one ???
        GET_AND_CHECK(BRACKET_LEFT);
    }
    else if (CHECK_TOKEN(BRACKET_LEFT)){;}

    GET_AND_CHECK(EOL);

    // body
    while(statement());

    delete_symtable(array_of_trees[tree_index]);
    delete_tree();
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
            break;
        }
        else
        {
            syntax_error(m.current_token.type,m.current_line);
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
    // TODO resolve coming from id,id 7

    GET_TOKEN();
    if (CHECK_NO_ERROR(ID))
    {
        free(m.current_token.data.s);
    }
    else if(CHECK_NO_ERROR(INT))
    {

    }
    else if(CHECK_NO_ERROR(STRING))
    {
        
    }
    else if(CHECK_NO_ERROR(FLOAT64))
    {
        
    }
    else
    {
        syntax_error(m.current_token.type,m.current_line);
    }
}

/**
 * @brief Checks syntax of the prolog
 */
void prolog()
{
    if (!expect_token(KEYWORD, K_PACKAGE))
    {
        syntax_error(KEYWORD,m.current_line);
    }

    if (expect_token(ID,K_ERROR))
    {
        if (strcmp("main",m.current_token.data.s) != 0)
        {
            syntax_error(m.current_token.type,m.current_line);
        }
        free(m.current_token.data.s);
    }
    else
    {
        syntax_error(m.current_token.type,m.current_line);
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
        if (m.current_token.type == t_type && ((k == K_ERROR)? true : m.current_token.data.k == k))
        {
            return true;
        }
        else if (m.current_token.type == EOL)
        {
            continue;
        }
        else
        {
            return false;
        }
    }
}