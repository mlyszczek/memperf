/*$2
 ===============================================================================
    Licensed under BSD 2-clause license. See LICENSE file for more information.
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ===============================================================================
 */


/*$2- Include Files ==========================================================*/


#include "opts.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "version.h"


/*$2- Private macros =========================================================*/


#define HAS_OPTARG() \
    do \
    { \
        if (*optarg == '\0') \
        { \
            fprintf(stderr, "missing argument for option '%c'\n", opt); \
            exit(2); \
        } \
    } \
    while (0);


/*$2- Global Variables =======================================================*/


struct opts opts;


/*$2- Private Functions ======================================================*/


/*
 -------------------------------------------------------------------------------
    function resets 'opts' variable to default values
 -------------------------------------------------------------------------------
 */

static void opts_reset(void)
{
    opts.block_size = 16 * 1024;
    opts.report_intvl = 100 * 1024 * 1024;
    opts.num_intvl = 10;
    opts.clock = CLK_REALTIME;
    opts.method = METHOD_MEMCPY;
    opts.fill_random = 0;
}

/*
 -------------------------------------------------------------------------------
    returns argument from 'argv'. If passed 'argv' doesn't hold valid argument
    -1 will be returned.
 -------------------------------------------------------------------------------
 */

static char opts_get(
    char*   argv)   /* pointer to option */
{
    if (argv == NULL || argv[0] != '-' || islower(argv[1]) == 0)
    {
        return -1;
    }

    return argv[1];
}

/*
 -------------------------------------------------------------------------------
    Prints help message. Who would suspect?
 -------------------------------------------------------------------------------
 */

static void opts_print_help(void)
{
/*$off*/

    printf(
"usage: memperf [-h | -v | options]\n"
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
"\t-m<method>   copying method:\n");

    printf(
"\t-c<clock>    clock to use to calculate bandwith\n"
"\n"
"methods:\n"
"\tmemcpy       copy data using buildin memcpy function\n"
"\tbbb          byte by byte copy, simple for loop\n"
"\n"
"clocks:\n"
"\trealtime     posix CLOCK_REALTIME clock is used\n"
"\tclock        clock_t is used\n");

/*$on*/
}

/*$2- Public Functions =======================================================*/


/*
 -------------------------------------------------------------------------------
    parses input options. if help (-h) or version (-v) option is found, program
    prints apropriate message and function returns with return code 0. If error
    occurs while parsing (syntax error), function exits with return code -1. -2
    is returned when ivalid option was passed or options has invalid arguments.
    options are stored in global variable 'opts'. Function first sets 'opts'
    with default values which are overwritten by user defined ones.
 -------------------------------------------------------------------------------
 */

int opts_parse(
    int     argc,   /* number of arguments */
    char*   argv[]) /* array of arguments */
{
    /*~~~~~~~~~~*/
    float   tmp;            /* temp variable for parsing */
    long    mul;            /* multiplication for <mbytes> arguments */
    char*   ep;             /* error pointer of strtol function */
    /*~~~~~~~~~~*/

    /* first set program options to default */

    opts_reset();

    /* skip program name */

    argc--;
    argv++;

    while (argc--)
    {
        /*~~~~~~~~~~~~~~~*/
        signed char opt;    /* current option */
        char*       optarg; /* current option argument */
        /*~~~~~~~~~~~~~~~*/

        optarg = (*argv) + 2;

        /*
         * read option, only "-C' is allowed where 'C' is [a-z]. pointer to
         * 'C' is returned and argv is set to point to next option
         */

        if ((opt = opts_get(*argv++)) == -1)
        {
            fprintf(stderr, "Syntax error in arguments\n");
            opts_print_help();
            exit(1);
        }

        switch (opt)
        {
        case 'v':
            printf(APP_VERSION "\n");
            exit(0);

        case 'h':
            opts_print_help();
            exit(0);

        case 'f':
            opts.fill_random = 1;
            break;

        case 'b':
        case 'r':
            HAS_OPTARG();

            tmp = (float)strtod(optarg, &ep);
            mul = 1;

            if (*ep)
            {
                switch (*ep)
                {
                case 'G':
                    mul *= 1024;

                case 'M':
                    mul *= 1024;

                case 'K':
                    mul *= 1024;
                    break;

                default:
                    fprintf(stderr,
                            "parameter %s for argument '%c' is invalid\n",
                            optarg,
                            opt);
                    exit(2);
                }
            }

            if (opt == 'b')
            {
                opts.block_size = tmp * mul;
            }
            else
            {
                opts.report_intvl = tmp * mul;
            }

            break;

        case 'i':
            HAS_OPTARG();

            tmp = strtol(optarg, &ep, 10);

            if (*ep)
            {
                fprintf(stderr,
                        "parameter %s for argument 'i' is invalid\n",
                        optarg);
                exit(2);
            }

            opts.num_intvl = tmp;
            break;

        case 'c':
            HAS_OPTARG();

            if (strcmp(optarg, "realtime") == 0)
            {
                opts.clock = CLK_REALTIME;
            }
            else if (strcmp(optarg, "clock") == 0)
            {
                opts.clock = CLK_CLOCK;
            }
            else
            {
                fprintf(stderr,
                        "parameter %s for optargument 'c' is invalid\n",
                        optarg);
                exit(2);
            }

            break;

        case 'm':
            HAS_OPTARG();

            if (strcmp(optarg, "memcpy") == 0)
            {
                opts.clock = METHOD_MEMCPY;
            }
            else if (strcmp(optarg, "bbb") == 0)
            {
                opts.clock = METHOD_BBB;
            }
            else
            {
                fprintf(stderr,
                        "parameter %s for optargument 'm' is invalid\n",
                        optarg);
                exit(2);
            }

            break;

        default:
            fprintf(stderr,
                    "Unknown option: %c (%02x)\n",
                    *optarg,
                    (unsigned char) *optarg);
            opts_print_help();
            exit(2);
        }
    }

    return 0;
}
