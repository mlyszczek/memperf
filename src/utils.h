/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


#ifndef UTILS_H
#define UTILS_H 1

#include <stddef.h>

#ifdef TESTS
/*
 * disable prints when unit testing
 */
#define fprintf (void)sizeof
#define printf (void)sizeof
#endif

struct jedec
{
    unsigned long val;
    char pre;
};

void ts(void *tm);
void ts_add_diff(void *taken, void *start, void *finish);
void *ts_new(void);
void ts_reset(void *tm);
unsigned long ts2us(void *tm);
void bytes2jedec(float bytes, struct jedec *jedec);

#endif
