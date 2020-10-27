/**
 * @file parser.c
 * @authors Rebeka Cernianska, Marek Micek, Matej Jurik, Peter Rucek
 * @date 25 Oct 2020
 * @brief Parser Implementation
 */

#include "parser.h"

#define GET_TOKEN()  m.actual_token = get_next_token(stdin)
#define CHECK_TOKEN(Type) ((m.actual_token.type == Type)? true : \
        (syntax_error(Type,m.actual_line),false))
#define CHECK_NO_ERROR(Type) ((m.actual_token.type == Type)? true : false)
#define GET_AND_CHECK(Type) do {GET_TOKEN(); CHECK_TOKEN(Type);}while(0)
#define IS_DATA_TYPE() (m.actual_token.type == KEYWORD && \
                       (m.actual_token.data.k == K_STRING ||\
                        m.actual_token.data.k == K_INT ||\
                        m.actual_token.data.k == K_FLOAT64))? true : (syntax_error(m.actual_token.type,m.actual_line),false)

//global
Metadata m = {.actual_line = 1};

int program()
{
    prolog(); // if wrong only lex error ??????
    functions();
    return error_value;
}

void functions()
{
    while(func());
    CHECK_TOKEN(EoF);
}

bool func()
{
    if(!func_header())
    {
        return false;
    }

    //TODO...body 
    statement();

    if (!expect_token(BRACKET_RIGHT,K_ERROR))
    {
        syntax_error(BRACKET_RIGHT,m.actual_line);
    }

    func();
    return true;
}

bool func_header()
{
    if (!expect_token(KEYWORD,K_FUNC))
    {
        if(!CHECK_NO_ERROR(EoF))
        {
            syntax_error(KEYWORD,m.actual_line);
        }
        return false;
    }
    GET_AND_CHECK(ID);
    GET_AND_CHECK(PARENTHESIS_LEFT);
    GET_TOKEN();
    if (!CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    {
        header_arg();
    }
    GET_TOKEN();
    bool inside = false;
    if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
    {
        header_ret();
        inside = true;
    }
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

void header_ret()
{
    GET_TOKEN();
    if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    {
        return;
    }
    IS_DATA_TYPE();
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
 * @brief Reading seqeunce ID <data type> , ID <data type> ,... )
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

void statement()
{

}


void prolog()
{
    if (!expect_token(KEYWORD, K_PACKAGE))
    {
        syntax_error(KEYWORD,m.actual_line);
    }
    
    GET_TOKEN();
    if (m.actual_token.type != ID || 0) // TODO access symbol table t.data.s == main
    {
        syntax_error(m.actual_token.type,m.actual_line);
    }
}

bool expect_token(Token_type t_type, Keyword k) //to lexer.c ????????
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