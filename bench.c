/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


/* ==== Include files ======================================================= */


#include "bench.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "opts.h"
#include "utils.h"


/* ==== Private macros   ==================================================== */


#define BENCH_START()                                \
    for (j = 0; j <= loops; ++j)                     \
    {                                                \
        if (opts.fill_random)                        \
        {                                            \
            bench_fill_random(dst, opts.block_size); \
            bench_fill_random(src, opts.block_size); \
        }                                            \
                                                     \
        /*
         * this tries to flush cpu cache - provided that user set
         * opts.cache_size big enough
         */                                          \
                                                     \
        memcpy(f1, f2, opts.cache_size);             \
        ts(start)

#define BENCH_END()                    \
    ts(finish);                        \
    ts_add_diff(taken, start, finish); \
}

/* ==== Private functions =================================================== */


/* ==========================================================================
    prints benchmark report to standard output
   ========================================================================== */


static void bench_report
(
    void*          taken,      /* time taken on data copying */
    float          copied      /* number of bytes copied */
)
{
    struct jedec   jd_bps;     /* bytes per second in jedec format */
    struct jedec   jd_copied;  /* number of bytes copied in jedec format */
    unsigned long  ns;         /* time taken copying data in nanoseconds */
    float          bps;        /* bytes per second rate */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ((ns = ts2ns(taken)) == 0)
    {
        ns = 1;
    }

    bps = copied / ns;
    bps *= 1000000;

    bytes2jedec(bps, &jd_bps);
    bytes2jedec(copied, &jd_copied);

    printf("copied %5lu %cB, in %5lu ns, rate %5lu %cB/s\n",
           jd_copied.val,
           jd_copied.pre,
           ns,
           jd_bps.val,
           jd_bps.pre);
}


/* ==========================================================================
    stores 'c' random bytes into 'p'
   ========================================================================== */


static void bench_fill_random
(
    void   *p,  /* location where random bytes will be stored */
    size_t  c   /* number of bytes to store in 'p' */
)
{
    while (c--)
    {
        *(unsigned char *)p = rand() % 256;
        p = (unsigned char *)p + 1;
    }
}


/* ==== Public functions ==================================================== */


/* ==========================================================================
    performs benchmark on pointers dst and src. dst and src can be heap or
    stack allocated.
   ========================================================================== */


int bench
(
    void         *dst,              /* destination pointer */
    void         *src,              /* source pointer */
    void         *f1,               /* first pointer used to flush cpu cache */
    void         *f2                /* second pointer used to flush cpu cache */
)
{
    void         *start;            /* timer indicating benchmark start */
    void         *finish;           /* timer indicating benchmark finish */
    void         *taken;            /* timer for time taken on benchmark */
    float         bytes_copied;     /* bytes copied in * iteration */
    size_t        loops;            /* loops needed to copy requested bytes */
    size_t        i;                /* iterator for loop */
    size_t        j;                /* iterator for loop */
    size_t        k;                /* iterator for loop */
    struct jedec  jd_block_size;    /* block size in jedec format */
    struct jedec  jd_report_intvl;  /* report interval value in jedec format */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    start = ts_new();
    finish = ts_new();
    taken = ts_new();

    srand(time(NULL));
    loops = opts.report_intvl / opts.block_size;
    bytes2jedec(opts.block_size, &jd_block_size);
    bytes2jedec(opts.report_intvl, &jd_report_intvl);

    printf("block size: %lu %cB, report every %lu %cB, iterations %lu\n",
           jd_block_size.val,
           jd_block_size.pre,
           jd_report_intvl.val,
           jd_report_intvl.pre,
           opts.num_intvl);

    /*
     * for systems that uses optimistic memory allocation (like linux) dst
     * and src may not really allocated just yet. dst and src will be
     * allocated when we first access them. This causes first memory copy
     * iteration to take much longer time causing program to show wrong
     * transfer rate. To prevent this behaviour we do a simple memory copy
     * here, so dst
     */

    memcpy(dst, src, opts.block_size);

    for (i = 0, j = 0; i != opts.num_intvl; ++i)
    {
        ts_reset(taken);

        switch (opts.method)
        {
        case METHOD_MEMCPY:
            BENCH_START();
            memcpy(dst, src, opts.block_size);
            BENCH_END();
            break;

        case METHOD_BBB:
            BENCH_START();

            for (k = 0; k != opts.block_size; ++k)
            {
                unsigned char  *d;  /* source pointer */
                unsigned char  *s;  /* destination pointer */
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                d = dst;
                s = src;

                d[k] = s[k];
            }

            BENCH_END();
            break;

        default:
            assert(0 && "test method not supported, should not get here");
        }

        bytes_copied = (float)j * opts.block_size;
        bench_report(taken, bytes_copied);
    }

    free(start);
    free(finish);
    free(taken);

    return 0;
}
