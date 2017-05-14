 /*$2
 ===============================================================================
    Licensed under BSD 2-clause license. See LICENSE file for more information.
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ===============================================================================
 */

#ifndef OPTS_H
#define OPTS_H 1

#include <stddef.h>

enum clock
{
    CLK_REALTIME,
    CLK_CLOCK
};

enum method
{
    METHOD_MEMCPY,
    METHOD_BBB
};

struct opts
{
    size_t block_size;
    unsigned long num_intvl;
    float report_intvl;
    enum clock clock;
    enum method method;
    int fill_random;
};

extern struct opts opts;
int opts_parse(int argc, char *argv[]);

#endif
