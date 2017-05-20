/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


/* ==== Include files ======================================================= */


#include "utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "opts.h"


/* ==== Public functions ==================================================== */


/* ==========================================================================
    function returns timestamp in 'tm' location depending on the clock set in
    opts.h. If clock option set doesn't match passed 'tm' object, (like clock_t
    for CLK_CLOCK) behaviour is undefinied
   ========================================================================== */


void ts
(
    void  *tm  /* pointer to time where to store current timestamp */
)
{
#ifdef POSIX
    if (opts.clock == CLK_REALTIME)
    {
        /*
         * posix doesn't require CLOCK_MONOTONIC to be implemented, so we use
         * CLOCK_REALTIME which may ocasionally jump forward or backward, but
         * tests are relatively short, so this should not be a problem.
         */

        clock_gettime(CLOCK_REALTIME, tm);
    }
    else
#endif

    if (opts.clock == CLK_CLOCK)
    {
        *(clock_t *)tm = clock();
    }
    else
    {
        assert(0 && "clock not supported, should not get here");
    }
}


/* ==========================================================================
    creates new object for timestamp, returned pointer should be passed to
    free() when it is no longer needed
   ========================================================================== */


void *ts_new(void)
{
#ifdef POSIX
    if (opts.clock == CLK_REALTIME)
    {
        return malloc(sizeof(struct timespec));
    }
    else
#endif

    if (opts.clock == CLK_CLOCK)
    {
        return malloc(sizeof(clock_t));
    }
    else
    {
        assert(0 && "clock not suppoert, should not get here");
    }
}


/* ==========================================================================
    sets pointer to zero value. Check ts() for more information regarding
    undefinied behaviour that may occur
   ========================================================================== */


void ts_reset
(
    void  *tm  /* time object to zero */
)
{
#ifdef POSIX
    if (opts.clock == CLK_REALTIME)
    {
        memset(tm, 0, sizeof(struct timespec));
    }
    else
#endif

    if (opts.clock == CLK_CLOCK)
    {
        memset(tm, 0, sizeof(clock_t));
    }
    else
    {
        assert(0 && "clock not supported, should not get here");
    }
}


/* ==========================================================================
    function adds difference between 'finish' and 'start' to 'tm'. Simply put
    it is tm += finish - start; Check ts() for more information regarding
    undefinied behaviour that may occur
   ========================================================================== */


void ts_add_diff
(
    void  *tm,     /* finish - start will be added here */
    void  *start,  /* first point in time to differate */
    void  *finish  /* second point in time to differate */
)
{
#ifdef POSIX
    if (opts.clock == CLK_REALTIME)
    {
        struct timespec   d;  /* diff between finish and start */
        struct timespec  *t;  /* tm pointer */
        struct timespec  *s;  /* start pointer */
        struct timespec  *f;  /* finish pointer */
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        t = tm;
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
    else
#endif

    if (opts.clock == CLK_CLOCK)
    {
        clock_t  *t;  /* tm pointer */
        clock_t  *s;  /* start pointer */
        clock_t  *f;  /* finish pointer */
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        t = tm;
        s = start;
        f = finish;

        *t += *f -*s;
    }
    else
    {
        assert(0 && "clock not supported, should not get here");
    }
}


/* ==========================================================================
    function converts 'tm' object into microseconds. Check ts() for more
    information regarding undefinied behaviour that may occur
   ========================================================================== */


unsigned long ts2us
(
    void  *tm  /* time to convert to microseconds */
)
{
#ifdef POSIX
    if (opts.clock == CLK_REALTIME)
    {
        struct timespec  *t = tm;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        return t->tv_sec * 1000000l + t->tv_nsec / 1000;
    }
    else
#endif

    if (opts.clock == CLK_CLOCK)
    {
        return *(clock_t *)tm / (CLOCKS_PER_SEC / 1000000);
    }
    else
    {
        assert(0 && "clock not supported, should not get here");
    }
}


/* ==========================================================================
    converts bytes to jedec output. ie 153600 will be converted to 150K
   ========================================================================== */


void bytes2jedec
(
    float               bytes,  /* bytes to convert */
    struct jedec       *jedec   /* bytes in jedec format */
)
{
    static const int    KB = 1024;
    static const long   MB = 1024l * 1024;
    static const float  GB = 1024.0 * 1024 * 1024;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (bytes < 100 * KB)
    {
        jedec->val = bytes;
        jedec->pre = '\0';
    }
    else if (bytes < 100l * MB)
    {
        jedec->val = bytes / KB;
        jedec->pre = 'K';
    }
    else if (bytes < 100.0 * GB)
    {
        jedec->val = bytes / MB;
        jedec->pre = 'M';
    }
    else
    {
        jedec->val = bytes / GB;
        jedec->pre = 'G';
    }
}
