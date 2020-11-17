/**
 * @file parser.c
 * @authors Rebeka Cernianska, Marek Micek, Matej Jurik, Peter Rucek
 * @date 25 Oct 2020
 * @brief Parser Implementation
 */

#include "parser.h"

#define GET_TOKEN()  \
        do{ m.current_token = get_next_token(stdin); \
            (m.current_token.type == EOL)? m.current_line++ : 0 ; \
            (m.current_token.type == ERROR)? (lexical_error(m.current_line), 1) : 0 ;\
        }while(0)

#define CHECK_TOKEN(Type) \
        ((m.current_token.type == (Type))? true : \
        (syntax_error(m.current_token.type,m.current_line), \
        ((m.current_token.type == ID || m.current_token.type == STRING)? \
        free(m.current_token.data.s),false : false), false))

#define CHECK_NO_ERROR(Type) ((m.current_token.type == (Type))? true : false)

#define GET_AND_CHECK(Type) do {GET_TOKEN(); CHECK_TOKEN((Type));}while(0)

#define IS_DATA_TYPE() \
        (m.current_token.type == KEYWORD && \
        (m.current_token.data.k == K_STRING || \
         m.current_token.data.k == K_INT || \
         m.current_token.data.k == K_FLOAT64))? \
         true : (syntax_error(m.current_token.type,m.current_line),false)

#define GENERATE(func) if ((!func)) intern_error()

//global
Metadata m = {.current_line = 1, .index = 0, .local_table = NULL};
TNode *node;
TNode *array_of_trees[100]; // this could be macro
TNode *arr_suspected[ARR_TREE_RANGE];  //< stores all funcs suspected from no_definition
int tree_index = -1;
int suspected_tree_idx = -1;
char *built_in[] = {"inputs", "inputf", "inputi", "print", "int2float", "float2int",
"len", "substr", "ord", "chr"};

//TODO EXPRESSIONS !!!!!
// bez nich nemozem dorobit semantiku funkcii, lava strana priradenia je zatial bez datoveho typu
// takze nemozem skontrolovat zhodu medzi typmi ktore vracia fun a typmi na lavej strane
// preto sa v mojej implementacii nachadzaju zakomentovane casti, ktore bez expressions nemaju vyznam
// pri priradeni funkcie hadze syntakticku chybu : od, id = dobry_den("ahoj", 24)

/**
 * @brief Inits data structure of symtable to eliminate multi. insertion of one symbol
 * @param new_data Data unit that will be initionalized
 * @return Initionalized data ready for next use
 */
TData init_new_data(TData new_data)
{
    new_data.type = T_UNDEFINED;
    new_data.defined = new_data.is_var = new_data.is_function = new_data.in_block = false;
    new_data.param_counter = new_data.ret_counter = new_data.line = new_data.id_counter = 0;

    for (int i = 0; i < MAX_RET_VAL; i++)
    {
        new_data.retval_arr[i] = T_UNDEFINED;
        //new_data.id_type[i] = T_UNDEFINED;      //< bude sa pouzivat po dorobeni expressions
    }
    for (int j = 0; j < MAX_ARG; j++)
        new_data.arg_arr[j] = T_UNDEFINED;

    return new_data;
}

/**
 * @brief Inits another tree in array of suspected tree
 */
void add_suspected_tree()
{
    suspected_tree_idx++;
    arr_suspected[suspected_tree_idx] = init_symtable(arr_suspected[suspected_tree_idx]);
}

/**
 * @brief Deletes the whole array of suspected trees
 */
void delete_arr_suspected()
{
    for (int i = 0; i <= suspected_tree_idx; i++)
    {
        delete_symtable(arr_suspected[i]);
    }
}

/**
 * @brief Checks semantics of funcs suspected from no_definition
 */
void check_suspected()
{
    for (int idx = 0; idx <= suspected_tree_idx; idx++)
    {
        TNode *root = arr_suspected[idx];
        bool sem_error = false;

        if ((node = search_symtable(m.global_table, root->key)) == NULL)    //< func is not defined
        {
            no_definition_error(root->key, root->data.line);
        }
        else  //< func is defined, check params
        {
            if (node->data.param_counter != root->data.param_counter) //|| node->data.ret_counter != root->data.id_counter)
            {
                sem_error = true;
            }
            else
            {
                for (unsigned i = 0; i < node->data.param_counter; i++)
                {
                    if (node->data.arg_arr[i] != root->data.arg_arr[i])     //< data type not same
                    {
                        sem_error = true;
                        break;
                    }
                }
            }
            if (sem_error)
            {
                param_error(root->key, root->data.line);
            }
       }
    }
}

/**
 * @brief Checks semantics of buil_in functions
 * @param root Root of global symtable
*/
void check_built(TNode *root)
{
    if (root != NULL)
    {
        //using PostOrder
        check_built(root->lptr);
        check_built(root->rptr);

        bool sem_error = false;
        bool is_built = is_built_fun(root->key);    //< whether current func is built_in
        if (is_built)
        {
            if (!strcmp(root->key, "inputs"))
            {
                if (root->data.param_counter != 0) // || root->data.id_counter != 2)
                {
                    sem_error = true;
                }
                /*else if (root->data.id_type[0] != T_STRING || root->data.id_type[1] != T_INT)
                {
                    sem_error = true;
                }*/
            }
            else if (!strcmp(root->key, "inputi"))
            {
                if (root->data.param_counter != 0)  // || root->data.id_counter != 2)
                {
                    sem_error = true;
                }
                /*else if (root->data.id_type[0] != T_INT || root->data.id_type[1] != T_INT)
                {
                    sem_error = true;
                }*/
            }
            else if (!strcmp(root->key, "inputf"))
            {
                if (root->data.param_counter != 0) // root->data.id_counter != 2)
                {
                    sem_error = true;
                }
                /*else if (root->data.id_type[0] != T_FLOAT64 || root->data.id_type[1] != T_INT)
                {
                    sem_error = true;
                }*/
            }
            else if (!strcmp(root->key, "int2float"))
            {
                if (root->data.param_counter != 1)  // || root->data.id_counter != 1)
                {
                    sem_error = true;
                }
                else if (root->data.arg_arr[0] != T_INT) // || root->data.id_type[0] != T_FLOAT64)
                {
                    sem_error = true;
                }
            }
            else if (!strcmp(root->key, "float2int"))
            {
                if (root->data.param_counter != 1) // || root->data.id_counter != 1)
                {
                    sem_error = true;
                }
                else if (root->data.arg_arr[0] != T_FLOAT64) // || root->data.id_type[0] != T_INT)
                {
                    sem_error = true;
                }
            }
            else if (!strcmp(root->key, "len"))
            {
                if (root->data.param_counter != 1) // || root->data.id_counter != 1)
                {
                    sem_error = true;
                }
                else if (root->data.arg_arr[0] != T_STRING) // || root->data.id_type[0] != T_INT)
                {
                    sem_error = true;
                }
            }
            else if (!strcmp(root->key, "substr"))
            {
                if (root->data.param_counter != 3)  // || root->data.id_counter != 2)
                {
                    sem_error = true;
                }
                else if (root->data.arg_arr[0] != T_STRING || root->data.arg_arr[1] != T_INT || root->data.arg_arr[2] != T_INT) //|| root->data.id_type[0] != T_STRING || root->data.id_type[1] != T_INT)
                {
                    sem_error = true;
                }
            }
            else if (!strcmp(root->key, "ord"))
            {
                if (root->data.param_counter != 2) // || root->data.id_counter != 2)
                {
                    sem_error = true;
                }
                else if (root->data.arg_arr[0] != T_STRING || root->data.arg_arr[1] != T_INT) //|| root->data.id_type[0] != T_INT || root->data.id_type[1] != T_INT)
                {
                    sem_error = true;
                }
            }
            else if (!strcmp(root->key, "chr"))
            {
                if (root->data.param_counter != 1) // || root->data.id_counter != 2)
                {
                    sem_error = true;
                }
                else if (root->data.arg_arr[0] != T_INT) // || root->data.id_type[0] != T_STRING || root->data.id_type[1] != T_INT) 
                {
                    sem_error = true;
                }
            }
            else //< print
            {
                /*if (root->data.id_counter != 0)
                {
                    sem_error = true;
                }*/
            }
        }
        if (sem_error)
        {
            param_error(root->key, root->data.line);
        }
    }
}

/**
 * @brief Compares current func id with all possible built_in function
 * @return True if func id is one of built_in function, else false 
 */
bool is_built_fun(char *func_id)
{
    for (int i = 0; i < BUILT_FUNC_NUM; i++)
    {
        if (!strcmp(func_id, built_in[i]))
        {
            return true;
        }
    }
    return false;
}

void add_tree()
{
    tree_index++;
    array_of_trees[tree_index] = NULL;
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
void program()
{
    m.global_table = init_symtable(m.global_table);

    GENERATE(gen_header());

    prolog();
    while(true)
    {
        if (!func() && (m.current_token.type == EoF))
        {
            break;
        }
    }

    node = search_symtable(m.global_table, "main");
    if (node == NULL)
    {
        no_definition_error("main", -1);    //< func main was not defined
    }
    if (suspected_tree_idx > -1)    //< check funcs suspected from no_definition
    {
        check_suspected();
        delete_arr_suspected();
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

    GENERATE(gen_code_end());

    if (error_value == 0) // No error
    {
        flush();
    }
    else
    {
        gen_dispose();
    }
}

/**
 * @brief Checks syntax of function
 */
bool func()
{
    add_tree();
    array_of_trees[tree_index] = init_symtable(array_of_trees[tree_index]);

    TData new_data;
    new_data.type = ID;
    new_data.defined = true;
    new_data.is_var = true;
    new_data.is_function = false;
    new_data.in_block = true;

    array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data, "_");

    bool is_main = false;
    if(!func_header(&is_main))
    {
        return false;
    }

    // body
    while(statement());

    if(is_main)
    {
        GENERATE(gen_main_end());
    }
    else
    {
        GENERATE(gen_func_end());
    }

    delete_symtable(array_of_trees[tree_index]);
    //delete_symtable(root);
    delete_tree();
    return true;
}

/**
 * @brief Checks syntax of function header
 */
bool func_header(bool *is_main)
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
    bool missing_id = false;
    Token last_func;
    char *func_id;

    GET_TOKEN();
    if (!CHECK_TOKEN(ID))
    {
        missing_id = true;
    }
    else 
    {
        func_id = m.current_token.data.s;  //< store func id cause insertion to symtable will be later
        bool is_built = is_built_fun(func_id);

        if ((node = search_symtable(m.global_table, func_id)) != NULL || is_built)
        {
            re_definition_error(func_id, m.current_line);    //< redefinition of func is forrbiden
        }

        GENERATE(gen_func_header(func_id));

        new_data_func = init_new_data(new_data_func);
        new_data_func.defined = true;
        new_data_func.is_function = true;
        last_func = m.current_token;   //< to make correct free later        
    }

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

    if (!missing_id)
    {
        if (!strcmp(func_id, "main"))
        {
            *is_main = true;

            if (new_data_func.param_counter != 0 || new_data_func.ret_counter != 0)
            {
                param_error(func_id, m.current_line);    //< func main cant have param or ret value
            }
        }

        m.global_table = insert_symtable(m.global_table, new_data_func, func_id);    //< insert whole func
        free(last_func.data.s);     //< to free token id
    }

    GET_AND_CHECK(EOL);
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
                array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data_var, prev.data.s);    //< insert parameter
                break;
            case K_FLOAT64:
                new_data_var.type = T_FLOAT64;
                new_data_func.arg_arr[m.index++] = T_FLOAT64;
                array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data_var, prev.data.s);    //< insert parameter
                break;
            case K_STRING:
                new_data_var.type = T_STRING;
                new_data_func.arg_arr[m.index++] = T_STRING;
                array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data_var, prev.data.s);    //< insert parameter
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
    
    if (CHECK_NO_ERROR(EoF))
    {
        syntax_error(m.current_token.type, m.current_line);
        return false;
    }
    else if (CHECK_NO_ERROR(BRACKET_RIGHT))
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
        unsigned number_of_id = 1;
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

                free(m.current_token.data.s);

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
                other_error(m.current_line);
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
            CHECK_TOKEN(EOL);
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
            bool is_built = is_built_fun(last_id.data.s);
            number_of_id--;     //< func's id must be reduced
            function_call(last_id, number_of_id, is_built);
            m.index = 0;
            
            if (is_built)
            {
                check_built(m.global_table);  //< check semantic of built_in function
            }
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
void function_call(Token id, unsigned num_of_id, bool is_built)
{
    Token_type previous = PARENTHESIS_LEFT;
    unsigned act_param_counter = 0;
    bool sem_error = false;                     //< indicates semantic error
    new_data_func = init_new_data(new_data_func);
    new_data_func.is_function = true;
    new_data_func.line = m.current_line;
    new_data_func.id_counter = num_of_id;

    while(true)
    {
        GET_TOKEN();
        if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
        {
            if (!strcmp(id.data.s, "main"))     //< calling main is not allowed
            {
                other_error(m.current_line);
                break;
            }
            if (!(is_built))
            {
                if ((node = search_symtable(m.global_table, id.data.s)) != NULL)    //< func is defined for sure
                {
                    if (node->data.param_counter != act_param_counter || node->data.ret_counter != num_of_id)      //< num of params or ret vals not same
                    {
                        sem_error = true;
                    }
                }
                else        //< not sure whether func defined
                {
                    add_suspected_tree();
                    arr_suspected[suspected_tree_idx] = insert_symtable(arr_suspected[suspected_tree_idx], new_data_func, id.data.s);   //< check this func after whole file read
                }
            }
            else    //< func is built_in, we insert it to global table and check semantics later
            {
                m.global_table = insert_symtable(m.global_table, new_data_func, id.data.s);
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
            break;
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
    add_tree();
    array_of_trees[tree_index] = init_symtable(array_of_trees[tree_index]);


    expression();
    CHECK_TOKEN(BRACKET_LEFT);
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
void assignment_s(unsigned number_of_id)
{
    for (unsigned i = 1; i <= number_of_id; i++)
    {
        if (!expression())
        {
            GET_AND_CHECK(EOL);
            return;
        }
        if (i == number_of_id)
        {
            break;
        }
        CHECK_TOKEN(COMMA);
    }
    CHECK_TOKEN(EOL);
}

/**
 * @brief Checks syntax of for statement
 */
void for_s()
{
    add_tree();
    array_of_trees[tree_index] = init_symtable(array_of_trees[tree_index]);


    GET_TOKEN();
    if(CHECK_NO_ERROR(ID))
    {
        char *id_name;
        id_name = m.current_token.data.s;
        TData new_data;
        new_data.type = m.current_token.type;
        new_data.is_var = true;
        new_data.in_block = true;
        new_data.defined = true;
        new_data.is_function = false;

        GET_AND_CHECK(DEF_OF_VAR);
        array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data, id_name);
        free(id_name);
        expression();
        CHECK_TOKEN(SEMICLN);
    }
    else if (CHECK_TOKEN(SEMICLN)){;}

    // condition
    expression();
    CHECK_TOKEN(SEMICLN);

    // increment / decrement (can be epmty)
    GET_TOKEN();
    if(CHECK_NO_ERROR(ID))
    {
        free(m.current_token.data.s);
        GET_AND_CHECK(VAR_ASSIGN);
        expression(); //only one ???
        CHECK_TOKEN(BRACKET_LEFT);
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

        if (CHECK_NO_ERROR(COMMA))
        {
            continue;
        }
        else if (CHECK_NO_ERROR(EOL))
        {
            break;
        }
        else if (CHECK_NO_ERROR(EoF))
        {
            syntax_error(m.current_token.type,m.current_line);
            break;
        }
        else
        {
            syntax_error(m.current_token.type,m.current_line);
        }
    }
}

/**
 * @brief Parses a single expression
 * @param action Specifies where the expression to be parsed is used (preceding tokens or
 *               control structures after which / in which the expr is used).
 *               Determines expr parsing options.
 * @post after parsing valid expression, m.current_token.type is EOL
 *       after parsing invalid expression, m.current_token.type ....
 */
bool expression()
{
    /** @todo Implement semantic analysis for variables & functions within expressions
     *        When reading ID and then "(" call function call and return
     */

    // switch (action)
    // {
    // case CA_Assignment:
    // case CA_If:
    // case CA_For:
    // case CA_Definition:
    //     if (!__expression(EXPR_SIMPLE))
    //     {
    //         syntax_error(m.current_token.type, m.current_line);
    //     }
    //     break;
    // case CA_Return:
    //     // No. of return values semantics
    //     if (!__expression(EXPR_SIMPLE))
    //     {
    //         syntax_error(m.current_token.type, m.current_line);
    //     }
    //     break;
    // default:
    //     intern_error();
    //     break;
    // }
    bool func_call = false;
    if (!expr(&func_call))
    {
        syntax_error(m.current_token.type,m.current_line);
    }
    if (func_call)
        return false;
    else
        return true;
    
}

/**
 * @brief Reads input for expressions and ajusts it
 */
bool input(int *input_token, bool *func_call)
{
    m.previous_token = m.current_token;
    GET_TOKEN();
    if (CHECK_NO_ERROR(ADD) || CHECK_NO_ERROR(SUB))
    {
        *input_token = PM;
    }
    else if (CHECK_NO_ERROR(MUL) || CHECK_NO_ERROR(DIV))
    {
        *input_token = MD;
    }
    else if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
    {
        if (m.previous_token.type == ID)
        {
            function_call(m.previous_token,1,false); // todo 
            *func_call = true;
        }
        *input_token = LP;
    }
    else if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    {
        *input_token = RP;
    }
    else if (CHECK_NO_ERROR(ID) || CHECK_NO_ERROR(INT) ||
             CHECK_NO_ERROR(STRING) || CHECK_NO_ERROR(FLOAT64))
    {
        if (CHECK_NO_ERROR(ID) || CHECK_NO_ERROR(STRING))
        {
            if (CHECK_NO_ERROR(ID))
            {
                m.previous_token = m.current_token;
            }
            free(m.current_token.data.s);
        }
        *input_token = II;
    }
    else if (CHECK_NO_ERROR(GT) || CHECK_NO_ERROR(LT) ||
             CHECK_NO_ERROR(EQ) || CHECK_NO_ERROR(NE) ||
             CHECK_NO_ERROR(LE) || CHECK_NO_ERROR(GE)   )
    {
        *input_token = RO;
    }
    else if (CHECK_NO_ERROR(EOL) || CHECK_NO_ERROR(COMMA) ||
             CHECK_NO_ERROR(SEMICLN) || CHECK_NO_ERROR(BRACKET_LEFT))
    {
        *input_token = EN;
    }
    else
    {
        return false;
    }
    return true;
}

// /**
//  * @brief Parse a whole expression - entrypoint to expression parsing
//  *        Serves as an abstraction of expression() to ignore CurrentAction decision tree
//  * @param exprType Expression type indicator ( @see parser.h > tExpr for notes )
//  * @return bool
//  */
// bool __expression(tExpr exprType)
// {
//     return comparison(exprType);
// }

// /**
//  * @brief Parse "operand => logical operator => operand" type of expressions
//  * @param exprType Expression type indicator
//  * @return bool
//  */
// bool comparison(tExpr exprType)
// {
//     if (exprType == EXPR_SIMPLE) {
//         if (!term(exprType)) return false; //< Parse left operand only when parsing a simple expr
//     }
//     else {
//         term(exprType); //< Groupped expr -> move down according to precedence table
//     }

//     Token_type expected[] = {GT, LT, NE, LE, GE, EQ};
//     while (curtok_matches(expected, 6))
//     {
//         if (!term(EXPR_SIMPLE)) return false;
//     }

//     return true;
// }

// /**
//  * @brief Parse "operand => add or sub operator => operand" type of expressions
//  * @param exprType Expression type indicator
//  * @return bool
//  */
// bool term(tExpr exprType)
// {
//     if (exprType == EXPR_SIMPLE) {
//         if (!factor(exprType)) return false; //< Parse left operand only when parsing a simple expr
//     }
//     else {
//         factor(exprType); //< Groupped expr -> move down according to precedence table
//     }

//     Token_type expected[] = {ADD, SUB};
//     while(curtok_matches(expected, 2))
//     {
//         if (!factor(EXPR_SIMPLE)) return false;
//     }
    
//     return true;
// }

// /**
//  * @brief Parse "operand => mul or div operator => operand" type of expressions and function calls
//  *        After a successful parsing of expr literal, loads next token to be used 
//  *        as an operation depictor. 
//  *        Parsing of EXPR_GROUPPED expression type ( @see parser.h > tExpr ) begins here
//  *        in order to maintain operator precedence
//  * @param exprType Expression type indicator
//  * @return bool
//  */
// bool factor(tExpr exprType)
// {
//     if (exprType == EXPR_SIMPLE) { //< Parse left operand only when parsing a simple expr
//         if (!literal()) 
//             return false; 
//         else 
//         {
//             /** @todo Fix function literals + groupped expr */
//             if (CHECK_NO_ERROR(ID)) //< Check for function call
//             {
//                 GET_TOKEN();
//                 if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
//                 {
//                     if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
//                     {
//                         // Function parsed - get next token
//                         GET_TOKEN();
//                     }
//                     else {
//                         return false;
//                     }
//                 }
//                 // Not a function call - continue as normal
//             }
//             else
//                 GET_TOKEN(); //< If a literal was parsed successfully, get next token (operator)
//         }
//     }

//     Token_type expected[] = {MUL, DIV};
//     while(curtok_matches(expected, 2))
//     {
//         if (!literal()) return false;
//     }

//     return true;
// }

// /**
//  * @brief Parse elementary parts of expression. Supports nested expressions.
//  *        Note that groupped expressions are essentialy expressions with expr as left operand
//  *        so calling __expression(EXPR_GROUPPED) ( @see expression() ) tries to parse only right
//  *        operand.
//  *        Also note that this function serves as the base case to the recursive descent parsing.
//  * @return bool
//  */
// bool literal()
// {
//     // IMPLEMENT SEMANTIC CHECKING (ID & FUNC)

//     GET_TOKEN();

//     Token_type expected[] = {INT, FLOAT64, STRING, ID};
//     if (curtok_matches(expected, 4))
//     {
//         return true;
//     }
//     else if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
//     {
//         if (!__expression(EXPR_SIMPLE)) return false; //< Parse nested (priority) expression

//         // GET_TOKEN();
//         if (CHECK_TOKEN(PARENTHESIS_RIGHT)) //< End of nested (priority) expression
//         {
//             GET_TOKEN(); 
//             if (!CHECK_TOKEN(EOL)) //< Check whether expression still continues
//             { 
//                 // Continue aftern nested expr
//                 if (!__expression(EXPR_GROUPPED)) return false;  
//             }

//             return true; //< Expression ends with EOL
//         }
//         else { //< Nested expression has no exiting right parenthesis
//             return false;
//         }
//     }
//     else { //< Current token matches no valid expression tokens 
//         return false;
//     }
// }

// /**
//  * @brief Check if current token is in a given checkList
//  * @param checkList Array of tokens to check the current token against
//  * @param listSize Size of checkList
//  * @return bool
//  */
// bool curtok_matches(Token_type checkList[], int listSize)
// {
//     bool match = false;

//     for (int idx = 0; idx < listSize; idx++)
//     {
//         if (m.current_token.type == checkList[idx])
//         {
//             match = true;
//             break;
//         }
//     } 

//     if (!match) return false;
//     return true;
// }

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
        if (CHECK_NO_ERROR(t_type) && ((k == K_ERROR)? true : m.current_token.data.k == k))
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