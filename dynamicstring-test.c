/**
 * @file buffer.c
 * @authors Rebeka Cernianska ...
 * @date 16 Oct 2020
 * @brief dynamic string tests
 */

#include "dynamicstring.h"

int main()
{
    dynamic_string str;
    dynamic_string_init(&str);
    char *tocopy = "hello";
    for (int i = 0; i < 10; i++)
    {
        add_string(&str, tocopy);
    }
    printf("pocet alokovanych miest:%d, pocet znakov:%d\n", str.alloc_len, str.len);

    // dyn_string_free(&str);
    //
    // for (int i = 0; i < 10; i++)
    // {
    //     add_char(&str, '\0');
    //     add_char(&str, 'd');
    // }

    dyn_string_free(&str);
    //printf("pocet alokovanych miest:%d, pocet znakov:%d\n", str.alloc_len, str.len);
    //printf("%s\n", str.buff);

}