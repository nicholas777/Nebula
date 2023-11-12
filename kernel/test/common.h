#ifndef _TEST_COMMON_H
#define _TEST_COMMON_H

typedef enum _test_type {
    INVALID = 0,
    TERMINAL, INTERRUPTS, MEMORY_MANAGER
} test_type_t;

#define MAX_TYPE MEMORY_MANAGER

typedef enum _test_result {
    success,
    failure
} test_result_t;

#include "kernel/io.h"

void test_print(const char *assert);

#define nequals(x, y) \
    test_print(#x " != " #y ": "); \
    if ((x) != (y)) { test_print("passed\n"); } \
    else { test_print("failed\n"); }

#define nequalsf(x, y) \
    test_print(#x " != " #y ": "); \
    if ((x) != (y)) { test_print("passed\n"); } \
    else { test_print("failed\n"); return failure; }

#define equals(x, y) \
    test_print(#x " == " #y ": "); \
    if ((x) == (y)) { test_print("passed\n"); } \
    else { test_print("failed\n"); }

#define equalsf(x, y) \
    test_print(#x " == " #y ": "); \
    if ((x) == (y)) { test_print("passed\n"); } \
    else { test_print("failed\n"); return failure; }

#define runs_start(msg) test_print("Code runs: " msg "\n");
#define runs_end() test_print("passed\n");

#endif
