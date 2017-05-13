 /*$2
 ===============================================================================
    Licensed under BSD 2-clause license. See LICENSE file for more information.
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ===============================================================================
 */

#ifndef ARGS_H
#define ARGS_H 1

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

struct args
{
    size_t block_size;
    size_t report_intvl;
    size_t num_intvl;
    enum clock clock;
    enum method method;
    int fill_random;
};

extern struct args args;
int args_parse(int argc, char *argv[]);

#endif
