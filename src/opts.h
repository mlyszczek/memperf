/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


#ifndef OPTS_H
#define OPTS_H 1

#include <stddef.h>

#include "config.h"

enum clock
{
#ifdef HAVE_CLOCK_GETTIME
    CLK_REALTIME,
#endif
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
    size_t cache_size;
    unsigned long num_intvl;
    float report_intvl;
    enum clock clock;
    enum method method;
};

extern struct opts opts;
int opts_parse(int argc, char *argv[]);

#endif
