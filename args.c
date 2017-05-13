 /*$2
 ===============================================================================
    Licensed under BSD 2-clause license. See LICENSE file for more information.
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ===============================================================================
 */

/*$2- Include Files*/

#include "args.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "version.h"

/*$2- Global Variables */

struct args args;

/*$2- Private Functions */

/* function resets 'args' variable to default values */
static void args_reset(void)
{
    args.block_size = 16 * 1024;
    args.report_intvl = 100 * 1024 * 1024;
    args.num_intvl = 10;
    args.clock = CLK_REALTIME;
    args.method = METHOD_MEMCPY;
    args.fill_random = 0;
}

/* returns argument from 'argv'. If passed 'argv' doesn't hold valid argument -1
 * will be returned.
 */
static char *args_get(char *argv)
{
    if (argv == NULL || argv[0] != '-' || islower(argv[1]) == 0)
    {
        return NULL;
    }

    return &argv[1];
}

static void args_print_help(void)
{
    printf("usage: memperf [-h | -v | options]\n"
           "\n"
           "arguments *MUST* be separated, ie -s -p is good while -sp is not\n"
           "parameters must be next to argument like '-b1024' not '-b 1024'\n"
           "\n"
           "options:\n"
           "\t-h           this help message\n"
           "\t-v           prints version and exists\n"
           "\t-f           fill memory block with random data before copy\n"
           "\t-b<mbytes>   size of a single memory block\n"
           "\t-r<mbytes>   print report every 'bytes' copied\n"
           "\t-i<number>   number of intervals\n"
           "\t-m<method>   copying method:\n"
           "\t-c<clock>    clock to use to calculate bandwith\n"
           "\n"
           "methods:\n"
           "\tmemcpy       copy data using buildin memcpy function\n"
           "\tbbb          byte by byte copy, simple for loop\n"
           "\n"
           "clocks:\n"
           "\trealtime     posix CLOCK_REALTIME clock is used\n"
           "\tclock        clock_t is used\n"
           );
}

/*$2- Public Functions */

int args_parse(int argc, char *argv[])
{
    long tmp;
    long mul;
    char *ep;

    args_reset();

    /* skip program name argument */

    argc--;
    argv++;

    while(argc--)
    {
        char *arg; /* current argument */

        if ((arg = args_get(*argv++)) == NULL)
        {
            fprintf(stderr, "Syntax error in arguments\n");
            args_print_help();
            exit(1);
        }

        switch (*arg++)
        {
            case 'v':
                printf(APP_VERSION"\n");
                exit(0);

            case 'h':
                args_print_help();
                exit(0);

            case 'f':
                args.fill_random = 1;
                break;

            case 'b':
                tmp = strtol(arg, &ep, 10);
                mul = 1;

                if (*ep)
                {
                    switch (*ep)
                    {
                    case 'G': mul *= 1024;
                    case 'M': mul *= 1024;
                    case 'K': mul *= 1024; break;
                    default:
                        fprintf(stderr,
                                "parameter %s for argument 'b' is invalid\n",
                                arg);
                        exit(0);
                    }
                }

                args.block_size = tmp * mul;
                break;

            case 'r':
                tmp = strtol(arg, &ep, 10);
                mul = 1;

                if (*ep)
                {
                    switch (*ep)
                    {
                    case 'G': mul *= 1024;
                    case 'M': mul *= 1024;
                    case 'K': mul *= 1024; break;
                    default:
                        fprintf(stderr,
                                "parameter %s for argument 'r' is invalid\n",
                                arg);
                        exit(0);
                    }
                }

                args.report_intvl = tmp * mul;
                break;

            case 'i':
                tmp = strtol(arg, &ep, 10);

                if (*ep)
                {
                    fprintf(stderr, "parameter %s for argument 'i' is invalid\n",
                            arg);
                    exit(2);
                }

                args.num_intvl = tmp;
                break;

            case 'c':
                if (strcmp(arg, "realtime") == 0)
                {
                    args.clock = CLK_REALTIME;
                }
                else if (strcmp(arg, "clock") == 0)
                {
                    args.clock = CLK_CLOCK;
                }
                else
                {
                    fprintf(stderr, "parameter %s for argument 'c' is invalid\n",
                            arg);
                    exit(2);
                }

                break;

            case 'm':
                if (strcmp(arg, "memcpy") == 0)
                {
                    args.clock = METHOD_MEMCPY;
                }
                else if (strcmp(arg, "bbb") == 0)
                {
                    args.clock = METHOD_BBB;
                }
                else
                {
                    fprintf(stderr, "parameter %s for argument 'm' is invalid\n",
                            arg);
                    exit(2);
                }

                break;

            default:
                fprintf(stderr, "Unknown option: %c (%02x)\n", *arg,
                        (unsigned char)*arg);
                args_print_help();
                exit(2);
        }
    }

    return 0;
}
