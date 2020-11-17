/**
 * @file test_parser.c
 * @authors Matej Jurik
 * @date 16 Nov 2020
 * @brief Parser test suite
 */

#include "test_parser.h"

#define testerror(error) perror("**TESTERROR " __FILE__ "** "error)
#define testfail(fmt, ...) \
    fprintf(stderr, "**TESTFAIL " __FILE__"> %s** "fmt"\n", __func__, ##__VA_ARGS__)
// #define setError() if(error!=true) error=true

int main(void)
{
    return 0;
}