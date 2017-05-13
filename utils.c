 /*$2
 ===============================================================================
    Licensed under BSD 2-clause license. See LICENSE file for more information.
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ===============================================================================
 */

/*$2- Include files */

#include "utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*$2- Public functions */

/*
 * function returns timestamp depending on the 'clk' parameter. Pointer to 'c'
 * must be apropriate in regard of 'clk', if types doesn't match, result is
 * undefinied.
 *
 */

void ts(void *tm)
{
    if (args.clock == CLK_REALTIME)
    {
        /*
         * posix doesn't require CLOCK_MONOTONIC to be implemented, so we use
         * CLOCK_REALTIME which may ocasionally jump forward or backward, but
         * tests are relatively short, so this should not be a problem.
         */

        clock_gettime(CLOCK_REALTIME, tm);
    }
    else if (args.clock == CLK_CLOCK)
    {
        clock_t *t = tm;
        *t = clock();
    }
    else
    {
        assert(0 && "clock not supported, should not get here");
    }
}

void *ts_new(void)
{
    if (args.clock == CLK_REALTIME)
    {
        return malloc(sizeof(struct timespec));
    }
    else if (args.clock == CLK_CLOCK)
    {
        return malloc(sizeof(clock_t));
    }
    else
    {
        assert(0 && "clock not suppoert, should not get here");
    }
}

void ts_reset(void *tm)
{
    if (args.clock == CLK_REALTIME)
    {
        memset(tm, 0, sizeof(struct timespec));
    }
    else if (args.clock == CLK_CLOCK)
    {
        memset(tm, 0, sizeof(clock_t));
    }
    else
    {
        assert(0 && "clock not supported, should not get here");
    }
}

void ts_add_diff(void *taken, void *start, void *finish)
{
    if (args.clock == CLK_REALTIME)
    {
        struct timespec d;
        struct timespec *t;
        struct timespec *s;
        struct timespec *f;

        t = taken;
        s = start;
        f = finish;

        if ((f->tv_nsec - s->tv_nsec) < 0)
        {
            d.tv_sec = f->tv_sec - s->tv_sec - 1;
            d.tv_nsec = f->tv_nsec - s->tv_nsec + 1000000000;
        }
        else
        {
            d.tv_sec = f->tv_sec - s->tv_sec;
            d.tv_nsec = f->tv_nsec - s->tv_nsec;
        }

        t->tv_sec += d.tv_sec;
        t->tv_nsec += d.tv_nsec;

        if (t->tv_nsec >= 1000000000)
        {
            t->tv_sec += 1;
            t->tv_nsec -= 1000000000;
        }
    }
    else if (args.clock == CLK_CLOCK)
    {
        clock_t *t;
        clock_t *s;
        clock_t *f;

        t = taken;
        s = start;
        f = finish;

        *t += *f - *s;
    }
    else
    {
        assert(0 && "clock not supported, should not get here");
    }
}

unsigned long ts2ns(void *tm)
{
    if (args.clock == CLK_REALTIME)
    {
        struct timespec *t = tm;

        return t->tv_sec * 1000000ll + t->tv_nsec / 1000;
    }
    else if (args.clock == CLK_CLOCK)
    {
        clock_t *t = tm;

        return *t / (CLOCKS_PER_SEC / 1000000);
    }
    else
    {
        assert(0 && "clock not supported, should not get here");
    }
}

void bytes2jedec(size_t bytes, struct jedec *jedec)
{
    if (bytes < 100 * 1024l)
    {
        jedec->val = bytes;
        jedec->pre = '\0';
    }
    else if (bytes < 100 * 1024l * 1024l)
    {
        jedec->val = bytes / 1024l;
        jedec->pre = 'K';
    }
    else if (bytes < 100 * 1024l * 1024l * 1024l)
    {
        jedec->val = bytes / (1024l * 1024l);
        jedec->pre = 'M';
    }
    else
    {
        jedec->val = bytes / (1024l * 1024l * 1024l);
        jedec->pre = 'G';
    }
}
