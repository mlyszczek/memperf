/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */

#include <stdio.h>

#define mt_defs()                                                              \
    const char *curr_test;                                                     \
    int mt_fail_flag = 0;                                                      \
    int mt_test_status;                                                        \
    int mt_total_tests = 0;                                                    \
    int mt_succed_tests = 0;                                                   \
    int mt_total_asserts = 0;                                                  \
    int mt_succed_asserts = 0

#define mt_allow_print()                                                       \
    freopen("/dev/tty", "w", stderr);                                          \
    freopen("/dev/tty", "w", stdout)

#define mt_disable_print()                                                     \
    fclose(stderr);                                                            \
    fclose(stdout)

#define mt_run(f)                                                              \
    curr_test = #f;                                                            \
    mt_test_status = 0;                                                        \
    ++mt_total_tests;                                                          \
    fprintf(stdout, "test %-71s", curr_test);                                  \
    mt_disable_print();                                                        \
    f();                                                                       \
    mt_allow_print();                                                          \
    if (mt_test_status != 0)                                                   \
        fprintf(stdout, "[NK]\n");                                             \
    else                                                                       \
    {                                                                          \
        fprintf(stdout, "[OK]\n");                                             \
        ++mt_succed_tests;                                                     \
    }

#define mt_assert(e)                                                           \
    ++mt_total_asserts;                                                        \
    if (!(e))                                                                  \
    {                                                                          \
        mt_allow_print();                                                      \
        fprintf(stderr, "assert %d: %s, %s\n", __LINE__, curr_test, #e);       \
        mt_disable_print();                                                    \
        mt_fail_flag = 1;                                                      \
        mt_test_status = -1;                                                   \
        return;                                                                \
    }                                                                          \
    ++mt_succed_asserts

#define mt_fail(e)                                                             \
    ++mt_total_asserts;                                                        \
    if (!(e))                                                                  \
    {                                                                          \
        mt_allow_print();                                                      \
        fprintf(stderr, "assert %d: %s, %s\n", __LINE__, curr_test, #e);       \
        mt_disable_print();                                                    \
        mt_fail_flag = 1;                                                      \
        mt_test_status = -1;                                                   \
    } else                                                                     \
        ++mt_succed_asserts

#define mt_return()                                                            \
    printf("summary: %d/%d tests passed, %d/%d asserts passed\n",              \
         mt_succed_tests, mt_total_tests, mt_succed_asserts, mt_total_asserts);\
    return mt_fail_flag

