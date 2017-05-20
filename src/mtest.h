/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */

/* ==== mtest version v0.0.1 ================================================ */


/* ==== include files ======================================================= */


#include <stdio.h>


/* ==== public macros ======================================================= */


/* ==========================================================================
    macro with definitions, call this macro no more and no less than ONCE in
    global scope. Its task is to define some variables used by mtest macros.
   ========================================================================== */


#define mt_defs()                                                              \
    const char *curr_test;                                                     \
    int mt_fail_flag = 0;                                                      \
    int mt_test_status;                                                        \
    int mt_total_tests = 0;                                                    \
    int mt_succed_tests = 0;                                                   \
    int mt_total_asserts = 0;                                                  \
    int mt_succed_asserts = 0


/* ==========================================================================
    macro runs test 'f'. 'f' is just a function (without parenthesis ()).
   ========================================================================== */


#define mt_run(f) do {                                                         \
    curr_test = #f;                                                            \
    mt_test_status = 0;                                                        \
    ++mt_total_tests;                                                          \
    fprintf(stdout, "test %-71s", curr_test);                                  \
    f();                                                                       \
    if (mt_test_status != 0)                                                   \
        fprintf(stdout, "[NK]\n");                                             \
    else                                                                       \
    {                                                                          \
        fprintf(stdout, "[OK]\n");                                             \
        ++mt_succed_tests;                                                     \
    } } while(0)


/* ==========================================================================
    simple assert, when expression 'e' is evaluated to false, assert message
    will be logged, and macro will force function to return
   ========================================================================== */


#define mt_assert(e) do {                                                      \
    ++mt_total_asserts;                                                        \
    if (!(e))                                                                  \
    {                                                                          \
        fprintf(stderr, "assert %d: %s, %s\n", __LINE__, curr_test, #e);       \
        mt_fail_flag = 1;                                                      \
        mt_test_status = -1;                                                   \
        return;                                                                \
    }                                                                          \
    ++mt_succed_asserts; } while (0)


/* ==========================================================================
    same as mt_assert, but function is not forced to return, and test can
    continue
   ========================================================================== */


#define mt_fail(e) do {                                                        \
    ++mt_total_asserts;                                                        \
    if (!(e))                                                                  \
    {                                                                          \
        fprintf(stderr, "assert %d: %s, %s\n", __LINE__, curr_test, #e);       \
        mt_fail_flag = 1;                                                      \
        mt_test_status = -1;                                                   \
    } else                                                                     \
        ++mt_succed_asserts; } while(0)


/* ==========================================================================
    prints tests summary and returns with 0 if all tests have passed, or -1
    when at least one test has failed
   ========================================================================== */


#define mt_return() do {                                                       \
    printf("summary: %d/%d tests passed, %d/%d asserts passed\n",              \
         mt_succed_tests, mt_total_tests, mt_succed_asserts, mt_total_asserts);\
    return mt_fail_flag; } while(0)

