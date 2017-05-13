 /*$2
 ===============================================================================
    Licensed under BSD 2-clause license. See LICENSE file for more information.
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ===============================================================================
 */

/*$2- Include files */

#include "bench.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "args.h"
#include "utils.h"

static void bench_report(void *taken, size_t copied)
{
    struct jedec jd_bps;
    struct jedec jd_copied;
    unsigned long ns;
    double bps;

    if ((ns = ts2ns(taken)) == 0)
    {
        ns = 1;
    }

    bps = (double)copied / ns;
    bps *= 1000000;

    bytes2jedec(bps, &jd_bps);
    bytes2jedec(copied, &jd_copied);

    printf("copied %5zu %cB, in %5lu ns, rate %5zu %cB/s\n",
            jd_copied.val, jd_copied.pre, ns, jd_bps.val, jd_bps.pre);
}

static void bench_fill_random(void *p, size_t c)
{
    unsigned char *m = p;

    while (c--)
    {
        *m++ = rand() % 256;
    }
}

int bench(void *p1, void *p2)
{
    struct jedec jd_block_size;
    struct jedec jd_report_intvl;
    void *start;
    void *finish;
    void *taken;
    size_t bytes_copied;
    size_t loops;
    size_t i, j, k;

    start = ts_new();
    finish = ts_new();
    taken = ts_new();

    srand(time(NULL));
    loops = args.report_intvl / args.block_size;
    bytes2jedec(args.block_size, &jd_block_size);
    bytes2jedec(args.report_intvl, &jd_report_intvl);

    printf("block size: %zu %cB, report every %zu %cB, iterations %zu\n",
            jd_block_size.val, jd_block_size.pre,
            jd_report_intvl.val, jd_report_intvl.pre,
            args.num_intvl);

    /*
     * for systems that uses optimistic memory allocation (like linux) p1 and p2
     * are not really allocated just yet. p1 and p2 will be allocated when we
     * first access them. This causes first memory copy iteration to take much
     * longer time causing program to show wrong transfer rate. To prevent this
     * behaviour we do a simple memory copy here, so p1 and p2 are allocated.
     */
    memcpy(p1, p2, args.block_size);

    for (i = 0; i != args.num_intvl; ++i)
    {
        ts_reset(taken);

        switch (args.method)
        {
        case METHOD_MEMCPY:

            for (j = 0; j <= loops; ++j)
            {
                if (args.fill_random)
                {
                    bench_fill_random(p1, args.block_size);
                    bench_fill_random(p2, args.block_size);
                }

                ts(start);
                memcpy(p1, p2, args.block_size);
                ts(finish);
                ts_add_diff(taken, start, finish);
            }

            break;

        case METHOD_BBB:

            for (j = 0; j <= loops; ++j)
            {
                if (args.fill_random)
                {
                    bench_fill_random(p1, args.block_size);
                    bench_fill_random(p2, args.block_size);
                }

                ts(start);

                for (k = 0; k != args.block_size; ++k)
                {
                    unsigned char *s1;
                    unsigned char *s2;

                    s1 = p1;
                    s2 = p2;

                    s1[k] = s2[k];
                }

                ts(finish);
                ts_add_diff(taken, start, finish);
            }

            break;
        }

        bytes_copied = j * args.block_size;
        bench_report(taken, bytes_copied);
    }

    free(start);
    free(finish);
    free(taken);

    return 0;
}
