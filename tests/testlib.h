/**
 * @file testlib.c
 * @authors Matej Jurik
 * @date 16 Nov 2020
 * @brief Global interface for compiler test suites
 */

#ifndef __TESTLIB_H_
#define __TESTLIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Standard test interface

void setUp();
void tearDown();

#endif // __TESTLIB_H_