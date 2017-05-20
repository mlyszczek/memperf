/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


/* ==== Include files ======================================================= */


#include <stdio.h>
#include <stdlib.h>

#include "bench.h"
#include "opts.h"


int main
(
    int    argc,   /* number of program options */
    char  *argv[]  /* list of program options */
)
{
    void  *dst;    /* destination address for benchmarking */
    void  *src;    /* source address for benchmarking */
    void  *f1;     /* first pointer used to flush cpu cache */
    void  *f2;     /* second pointer used to flush cpu cache */
    int    rc;     /* return code */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ((rc = opts_parse(argc, argv)) != 0)
    {
        if (rc == 1)
        {
            /*
             * help or version was passed, return with code 0, as this is not an
             * error case
             */

            return 0;
        }

        return -rc;
    }

    dst = malloc(opts.block_size);
    src = malloc(opts.block_size);

    f1 = malloc(opts.cache_size);
    f2 = malloc(opts.cache_size);

    if (dst == NULL || src == NULL || f1 == NULL || f2 == NULL)
    {
        fprintf(stderr, "Couldn't allocate requested memory block\n");
        goto error;
    }

    bench(dst, src, f1, f2);

error:
    free(dst);
    free(src);
    free(f1);
    free(f2);

    return 0;
}
