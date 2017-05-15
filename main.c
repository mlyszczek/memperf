/*$2
 ===============================================================================
    Licensed under BSD 2-clause license. See LICENSE file for more information.
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ===============================================================================
 */


/*$2- Included Files =========================================================*/


#include <stdlib.h>
#include <stdio.h>

#include "opts.h"
#include "bench.h"


/*
 -------------------------------------------------------------------------------
 -------------------------------------------------------------------------------
 */
int main(
    int     argc,   /* */
    char*   argv[]) /* */
{
    /*~~~~~~~~~~*/
    void*   dst;    /* destination address for benchmarking */
    void*   src;    /* source address for benchmarking */
    void*   f1;     /* first pointer used to flush cpu cache */
    void*   f2;     /* second pointer used to flush cpu cache */
    int     rc;     /* return code */
    /*~~~~~~~~~~*/

    if ((rc = opts_parse(argc, argv)) != 0)
    {
        return -rc;
    }

    dst = malloc(opts.block_size);
    src = malloc(opts.block_size);

    f1 = malloc(opts.cache_size);
    f2 = malloc(opts.cache_size);

    if (dst == NULL || src == NULL || f1 == NULL || f2 == NULL)
    {
        fprintf(stderr, "Couldn't allocate requested memory block\n");
        free(dst);
        free(src);
        free(f1);
        free(f2);
        exit(3);
    }

    bench(dst, src, f1, f2);

    free(dst);
    free(src);
    free(f1);
    free(f2);

    return 0;
}
