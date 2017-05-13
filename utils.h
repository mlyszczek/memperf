 /*$2
 ===============================================================================
    Licensed under BSD 2-clause license. See LICENSE file for more information.
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ===============================================================================
 */

#ifndef UTILS_H
#define UTILS_H 1

#include "args.h"

struct jedec
{
    size_t val;
    char pre;
};

void ts(void *tm);
void ts_add_diff(void *taken, void *start, void *finish);
void *ts_new(void);
void ts_reset(void *tm);
unsigned long ts2ns(void *tm);
void bytes2jedec(size_t bytes, struct jedec *jedec);

#endif
