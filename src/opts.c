/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */

/* ==== Include files ======================================================= */


#include "opts.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "version.h"


/* ==== Private macros ====================================================== */


#define HAS_OPTARG()                                                    \
    do                                                                  \
    {                                                                   \
        if (*optarg == '\0')                                            \
        {                                                               \
            fprintf(stderr, "missing argument for option '%c'\n", opt); \
            return -2;                                                  \
        }                                                               \
    }                                                                   \
    while (0);


/* ==== Global variables ==================================================== */


struct opts opts;


/* ==== Private functions =================================================== */


/* ==========================================================================
    function resets 'opts' variable to default values
   ========================================================================== */


static void opts_reset(void)
{
    opts.block_size = 16 * 1024;
    opts.report_intvl = 100 * 1024 * 1024;
    opts.num_intvl = 10;
    opts.method = METHOD_MEMCPY;
    opts.cache_size = 1 * 1024 * 1024;

#if HAVE_CLOCK_GETTIME
    opts.clock = CLK_REALTIME;
#else
    opts.clock = CLK_CLOCK;
#endif
}


/* ==========================================================================
    returns argument from 'argv'. If passed 'argv' doesn't hold valid argument
    -1 will be returned.
   ========================================================================== */


static char opts_get
(
    char*  argv  /* pointer to option */
)
{
    if (argv == NULL || argv[0] != '-' || islower(argv[1]) == 0)
    {
        return -1;
    }

    return argv[1];
}


/* ==========================================================================
    Prints help message. Who would suspect?

    Function uses more than one print, as ANSI C only requires compilers to
    support at least 509 long string literal
   ========================================================================== */


static void opts_print_help(void)
{
    printf(
"usage: memperf [-h | -v | options]\n"
"\n"
"arguments *MUST* be separated, ie -s -p is good while -sp is not\n"
"parameters must be next to argument like '-b1024' not '-b 1024'\n"
"\n"
"options:\n"
"\t-h           this help message\n"
"\t-v           prints version and exists\n"
"\t-b<mbytes>   size of a single memory block\n"
"\t-r<mbytes>   print report every 'mbytes' copied\n"
"\t-l<mbytes>   size of the cpu cache, if 0 cache flush is disabled\n"
"\t-i<number>   number of intervals\n"
);

    printf(
"\t-m<method>   copying method\n"
"\t-c<clock>    clock to use to calculate bandwith\n"
"\n"
"methods:\n"
"\tmemcpy       copy data using buildin memcpy function\n"
"\tbbb          byte by byte copy, simple for loop\n"
"\n"
"clocks:\n"
#if HAVE_CLOCK_GETTIME
"\trealtime     posix CLOCK_REALTIME clock is used\n"
#endif
"\tclock        clock_t is used\n"
);
}

/* ==== Public functions ==================================================== */


/* ==========================================================================
    resets global 'opts' object with default values, and parses input options
    overwritting default settings. If help (-h) or version (-v) option is found
    anywhere in the argv, program prints what first was found and returns with
    code 1.

    returns:
             1      -h or -v was passed
             0      all options parsed, program can continue
            -1      syntax error (ie not started with '-')
            -2      invalid argument for the option
            -3      given option is not recognized (not available)
   ========================================================================== */


int opts_parse
(
    int    argc,   /* number of arguments */
    char  *argv[]  /* array of arguments */
)
{
    float  tmp;    /* temp variable for parsing */
    long   mul;    /* multiplication for <mbytes> arguments */
    char  *ep;     /* error pointer of strtol function */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*
     * first set program options to default
     */

    opts_reset();

    /*
     * skip program name
     */

    argc--;
    argv++;

    while (argc--)
    {
        signed char  opt;     /* current option */
        char        *optarg;  /* current option argument */
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        optarg = (*argv) + 2;

        /*
         * read option, only "-C' is allowed where 'C' is [a-z]. pointer to
         * 'C' is returned and argv is set to point to next option
         */

        if ((opt = opts_get(*argv++)) == -1)
        {
            fprintf(stderr, "Syntax error in arguments\n");
            opts_print_help();
            return -1;
        }

        switch (opt)
        {
        case 'v':
            printf(APP_VERSION "\n");
            return 1;

        case 'h':
            opts_print_help();
            return 1;

        case 'b':
        case 'r':
        case 'l':
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
                    return -2;
                }
            }

            if (opt == 'b')
            {
                opts.block_size = tmp * mul;
            }
            else if (opt == 'r')
            {
                opts.report_intvl = tmp * mul;
            }
            else
            {
                opts.cache_size = tmp * mul;
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
                return -2;
            }

            opts.num_intvl = tmp;
            break;

        case 'c':
            HAS_OPTARG();

#if HAVE_CLOCK_GETTIME
            if (strcmp(optarg, "realtime") == 0)
            {
                opts.clock = CLK_REALTIME;
            }
            else
#endif

            if (strcmp(optarg, "clock") == 0)
            {
                opts.clock = CLK_CLOCK;
            }
            else
            {
                fprintf(stderr,
                        "parameter %s for optargument 'c' is invalid\n",
                        optarg);
                return -2;
            }

            break;

        case 'm':
            HAS_OPTARG();

            if (strcmp(optarg, "memcpy") == 0)
            {
                opts.method = METHOD_MEMCPY;
            }
            else if (strcmp(optarg, "bbb") == 0)
            {
                opts.method = METHOD_BBB;
            }
            else
            {
                fprintf(stderr,
                        "parameter %s for optargument 'm' is invalid\n",
                        optarg);
                return -2;
            }

            break;

        default:
            fprintf(stderr,
                    "Unknown option: %c (%02x)\n",
                    *optarg,
                    (unsigned char) *optarg);
            opts_print_help();
            return -3;
        }
    }

    return 0;
}
