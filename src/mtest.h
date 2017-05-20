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


/* ==========================================================================
    macro runs test 'f'. 'f' is just a function (without parenthesis ()).
   ========================================================================== */


#define mt_run(f) do {                                                         \
    curr_test = #f;                                                            \
    mt_test_status = 0;                                                        \
    ++mt_total_tests;                                                          \
    f();                                                                       \
    if (mt_test_status != 0)                                                   \
        fprintf(stdout, "not ok %d - %s\n", mt_total_tests, curr_test);        \
    else                                                                       \
        fprintf(stdout, "ok %d - %s\n", mt_total_tests, curr_test);            \
    } while(0)


/* ==========================================================================
    simple assert, when expression 'e' is evaluated to false, assert message
    will be logged, and macro will force function to return
   ========================================================================== */


#define mt_assert(e) do {                                                      \
    if (!(e))                                                                  \
    {                                                                          \
        fprintf(stderr, "# assert %d: %s, %s\n", __LINE__, curr_test, #e);     \
        mt_fail_flag = 1;                                                      \
        mt_test_status = -1;                                                   \
        return;                                                                \
    } } while (0)


/* ==========================================================================
    same as mt_assert, but function is not forced to return, and test can
    continue
   ========================================================================== */


#define mt_fail(e) do {                                                        \
    if (!(e))                                                                  \
    {                                                                          \
        fprintf(stderr, "# assert %d: %s, %s\n", __LINE__, curr_test, #e);     \
        mt_fail_flag = 1;                                                      \
        mt_test_status = -1;                                                   \
    } } while(0)


/* ==========================================================================
    prints tests summary and returns with 0 if all tests have passed, or -1
    when at least one test has failed
   ========================================================================== */


#define mt_return() do {                                                       \
    fprintf(stdout, "1..%d\n", mt_total_tests);                                \
    return mt_fail_flag; } while(0)
