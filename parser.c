/**
 * @file parser.c
 * @authors Rebeka Cernianska, Marek Micek, Matej Jurik, Peter Rucek
 * @date 25 Oct 2020
 * @brief Parser Implementation
 */

#include "parser.h"

#define GET_TOKEN()  m.actual_token = get_next_token(stdin); (m.actual_token.type == ERROR)? lexical_error(m.actual_line), 1 : 0
#define CHECK_TOKEN(Type) ((m.actual_token.type == Type)? true : \
        (fprintf(stderr,"In function %s on line %d\n ",__func__, __LINE__) , syntax_error(Type,m.actual_line) , false))
#define CHECK_NO_ERROR(Type) ((m.actual_token.type == Type)? true : false)
#define GET_AND_CHECK(Type) do {GET_TOKEN(); CHECK_TOKEN(Type);}while(0)
#define IS_DATA_TYPE() (m.actual_token.type == KEYWORD && \
                       (m.actual_token.data.k == K_STRING ||\
                        m.actual_token.data.k == K_INT ||\
                        m.actual_token.data.k == K_FLOAT64))? true : (syntax_error(m.actual_token.type,m.actual_line),false)

//global
Metadata m = {.actual_line = 1};

/**
 * @brief Syntax of program
 */
int program()
{
    prolog();
    while(func());
    CHECK_TOKEN(EoF);
    return error_value;
}

/**
 * @brief Checks syntax of function
 */
bool func()
{
    if(!func_header())
    {
        return false;
    }

    // body 
    while(statement());

    func();
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

    // function parameters
    GET_AND_CHECK(PARENTHESIS_LEFT);
    GET_TOKEN();
    if (!CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    { 
        header_arg();
    }

    // function return values
    GET_TOKEN();
    bool inside = false;
    if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
    {
        header_ret();
        inside = true;
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
    m.actual_line++;

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
    IS_DATA_TYPE();
    // HERE add return value of function to symtable
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
    // HERE add parameters of function to symtable
    GET_TOKEN();
    IS_DATA_TYPE();
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
        // HERE add m.actual_token to symtable
        GET_TOKEN();
        int number_of_id = 1;

        // assignment to var statement with multiple ID
        if (CHECK_NO_ERROR(COMMA))
        {
            while(!CHECK_NO_ERROR(VAR_ASSIGN))
            {
                CHECK_TOKEN(COMMA);
                GET_AND_CHECK(ID);
                // HERE add m.actual_token to symtable
                number_of_id++;
                GET_TOKEN();
            }

            assignment_s(number_of_id);
        }
        // definition of var statement
        else if (CHECK_NO_ERROR(DEF_OF_VAR))
        {
            expression();
            GET_AND_CHECK(EOL);
            m.actual_line++;
        }
        // assignment to var statement
        else if (CHECK_NO_ERROR(VAR_ASSIGN))
        {
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
            previous = STRING;
            continue;
        }
        else if (CHECK_NO_ERROR(ID))
        {
            previous = ID;
            continue;
        }
    }
}

/**
 * @brief Checks syntax of if statement
 */
void if_s()
{
    // if 
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
    GET_TOKEN();
    if(CHECK_NO_ERROR(ID))
    {
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
        GET_AND_CHECK(VAR_ASSIGN);
        expression(); //only one ???
        GET_AND_CHECK(BRACKET_LEFT);
    }
    else if (CHECK_TOKEN(BRACKET_LEFT)){;}

    GET_AND_CHECK(EOL);
    m.actual_line++;

    // body
    while(statement());
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
    if (CHECK_NO_ERROR(ID));
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