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
    /*~~~~~~~*/
    void*   p1;
    void*   p2;
    int     rc;
    /*~~~~~~~*/

    if ((rc = opts_parse(argc, argv)) != 0)
    {
        return -rc;
    }

    p1 = malloc(opts.block_size);
    p2 = malloc(opts.block_size);

    if (p1 == NULL || p2 == NULL)
    {
        fprintf(stderr, "Couldn't allocate requested memory block\n");
        free(p1);
        free(p2);
        exit(3);
    }

    bench(p1, p2);

    free(p1);
    free(p2);

    return 0;
}
