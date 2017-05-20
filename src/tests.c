/* ==========================================================================
    Licensed under BSD 2clause license. See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


/* ==== Include files ======================================================= */


#include "mtest.h"

#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include "utils.h"
#include "opts.h"

#undef fprintf
#undef printf

mt_defs();


/* ==== Private functions =================================================== */


/* ==========================================================================
    returns heap allocated copy of 's'
   ========================================================================== */


static char *sdup
(
    const char *s   /* string to duplicate */
)
{
    char       *r;  /* copy of 's' */
    size_t     sl;  /* length of the 's' */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sl = strlen(s);

    if ((r = malloc(sl + 1)) == NULL)
    {
        return NULL;
    }

    memcpy(r, s, sl);
    r[sl] = '\0';

    return r;
}


/* ==========================================================================
    returns heap allocated copy of 's' with maximum size of 'l'. Returned string
    will be always null terminated.
   ========================================================================== */


static char *sndup
(
    const char *s,  /* string to duplicate */
    size_t      l   /* maximum ammout of byte to copy */
)
{
    char       *r;  /* copy of 's' */
    size_t     sl;  /* length of 's' */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sl = strlen(s);
    sl = l < sl ? l : sl;

    if ((r = malloc(sl + 1)) == NULL)
    {
        return NULL;
    }

    memcpy(r, s, sl);
    r[sl] = '\0';

    return r;
}


/* ==========================================================================
    converts option string into argc,argv pair. ie, "-a -b -c10 -darg" will be
    converted into array of 5 strings (as first argument is always program name,
    here we simulate it by always putting "memperf" as first option):

    argc == 5
    argv =
    {
        "memperf"
        "-a",
        "-b",
        "-c10",
        "-darg"
    };

    argv is returned via return pointer, and argc is returned as parameter
   ========================================================================== */


static char **str2opts
(
    const char  *opts,    /* string with options */
    int         *argc     /* number of options returned will be stored here */
)
{
    const char  *carg;    /* current argument being parsed */
    char       **rargv;   /* return_argv used to return allocated argv */
    char       **argv;    /* argv, pointers to options will be stored here */
    char         c;       /* currently parsed character of option */
    int          arglen;  /* number of arguments parsed */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*
     * 100 arguments should be more than enough for now and any future use
     */

    argv = malloc(100 * sizeof(char *));
    rargv = argv;
    *argv++ = sdup("memperf");
    *argc = 1;
    carg = opts;
    arglen = 0;

    if (opts == NULL)
    {
        return rargv;
    }

    while((c = *opts++))
    {
        if (c != ' ')
        {
            ++arglen;
            continue;
        }

        /*
         * c is space, opts points to next argument, but first dup previous
         * argument
         */

        *argv++ = sndup(carg, arglen);
        *argc += 1;
        carg = opts;
        arglen = 0;
    }

    if (arglen > 0)
    {
        /*
         * last argument wasn't duped in loop, dup it know
         */

        *argv = sndup(carg, arglen);
        *argc += 1;
    }

    return rargv;
}


/* ==========================================================================
    function looks for needle (n) in haystack (h).

    returns:
            0       n has not been found in h
            1       n has been found in h
   ========================================================================== */


static int is_in_array
(
    char        n,  /* needle to look for */
    const char *h   /* haystack to look for needle in */
)
{
    while (*h)
    {
        if (*h++ == n)
        {
            return 1;
        }
    }

    return 0;
}


/* ==========================================================================
    free all memory allocated by str2opts function
   ========================================================================== */


static void opts_free
(
    int     argc,  /* number of options */
    char  **argv   /* options to be freed */
)
{
    int     i;     /* iterator for the loop */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 0; i != argc; ++i)
    {
        free(argv[i]);
    }

    free(argv);
}


/* ==== Tests functions ===================================================== */

/* ==== utils.c tests ======================================================= */


void ts_new_clock(void)
{
    void *tm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_CLOCK;

    mt_assert(tm = ts_new());

    free(tm);
}


/* ==========================================================================
   ========================================================================== */


void ts_clock(void)
{
    clock_t *tm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_CLOCK;

    mt_assert(tm = ts_new());
    ts_reset(tm);
    mt_fail(*tm == 0);
    ts(tm);
    mt_fail(*tm != 0);

    free(tm);
}


/* ==========================================================================
   ========================================================================== */


void ts_reset_clock(void)
{
    clock_t *tm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_CLOCK;

    mt_assert(tm = ts_new());
    ts_reset(tm);
    mt_fail(*tm == 0);

    free(tm);
}


/* ==========================================================================
   ========================================================================== */


void ts_add_diff_clock_single(void)
{
    clock_t *tm;
    clock_t *start;
    clock_t *finish;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_CLOCK;

    mt_fail(tm = ts_new());
    mt_fail(start = ts_new());
    mt_fail(finish = ts_new());

    if (tm == NULL || start == NULL || finish == NULL)
    {
        return;
    }

    ts_reset(tm);

    *start = 1000;
    *finish = 1500;

    ts_add_diff(tm, start, finish);
    mt_fail(*tm == 500);

    free(tm);
    free(start);
    free(finish);
}


/* ==========================================================================
   ========================================================================== */


void ts_add_diff_clock_multi(void)
{
    clock_t *tm;
    clock_t *start;
    clock_t *finish;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_CLOCK;

    mt_fail(tm = ts_new());
    mt_fail(start = ts_new());
    mt_fail(finish = ts_new());

    if (tm == NULL || start == NULL || finish == NULL)
    {
        return;
    }

    ts_reset(tm);

    *start = 1000;
    *finish = 1500;

    ts_add_diff(tm, start, finish);
    mt_fail(*tm == 500);

    *start = 2000;
    *finish = 3000;

    ts_add_diff(tm, start, finish);
    mt_fail(*tm == 1500);

    free(tm);
    free(start);
    free(finish);
}


/* ==========================================================================
   ========================================================================== */


#ifdef HAVE_CLOCK_GETTIME
void ts_new_realtime(void)
{
    void *tm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_REALTIME;

    mt_assert(tm = ts_new());

    free(tm);
}


/* ==========================================================================
   ========================================================================== */


void ts_realtime(void)
{
    struct timespec *tm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_REALTIME;

    mt_assert(tm = ts_new());
    ts_reset(tm);
    mt_fail(tm->tv_sec == 0);
    mt_fail(tm->tv_nsec == 0);
    ts(tm);
    mt_fail(tm->tv_sec != 0);

    free(tm);
}


/* ==========================================================================
   ========================================================================== */


void ts_reset_realtime(void)
{
    struct timespec *tm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_REALTIME;

    mt_assert(tm = ts_new());
    ts_reset(tm);
    mt_fail(tm->tv_sec == 0);
    mt_fail(tm->tv_nsec == 0);

    free(tm);
}


/* ==========================================================================
   ========================================================================== */


void ts_add_diff_realtime_single_simple(void)
{
    struct timespec *tm;
    struct timespec *start;
    struct timespec *finish;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_REALTIME;

    mt_fail(tm = ts_new());
    mt_fail(start = ts_new());
    mt_fail(finish = ts_new());

    if (tm == NULL || start == NULL || finish == NULL)
    {
        return;
    }

    ts_reset(tm);

    start->tv_sec = 1000;
    start->tv_nsec = 0;
    finish->tv_sec = 1500;
    finish->tv_nsec = 0;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 500);
    mt_fail(tm->tv_nsec == 0);

    free(tm);
    free(start);
    free(finish);
}


/* ==========================================================================
   ========================================================================== */


void ts_add_diff_realtime_single_with_nsec_no_overflow(void)
{
    struct timespec *tm;
    struct timespec *start;
    struct timespec *finish;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_REALTIME;

    mt_fail(tm = ts_new());
    mt_fail(start = ts_new());
    mt_fail(finish = ts_new());

    if (tm == NULL || start == NULL || finish == NULL)
    {
        return;
    }

    ts_reset(tm);

    start->tv_sec = 1000;
    start->tv_nsec = 100;
    finish->tv_sec = 1500;
    finish->tv_nsec = 400;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 500);
    mt_fail(tm->tv_nsec == 300);

    free(tm);
    free(start);
    free(finish);
}


/* ==========================================================================
   ========================================================================== */


void ts_add_diff_realtime_single_with_nsec_overflow_down(void)
{
    struct timespec *tm;
    struct timespec *start;
    struct timespec *finish;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_REALTIME;

    mt_fail(tm = ts_new());
    mt_fail(start = ts_new());
    mt_fail(finish = ts_new());

    if (tm == NULL || start == NULL || finish == NULL)
    {
        return;
    }

    ts_reset(tm);

    start->tv_sec = 1000;
    start->tv_nsec = 200000000;
    finish->tv_sec = 1500;
    finish->tv_nsec = 100000000;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 499);
    mt_fail(tm->tv_nsec == 900000000);

    free(tm);
    free(start);
    free(finish);
}


/* ==========================================================================
   ========================================================================== */


void ts_add_diff_realtime_single_with_nsec_overflow_up(void)
{
    struct timespec *tm;
    struct timespec *start;
    struct timespec *finish;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_REALTIME;

    mt_fail(tm = ts_new());
    mt_fail(start = ts_new());
    mt_fail(finish = ts_new());

    if (tm == NULL || start == NULL || finish == NULL)
    {
        return;
    }

    ts_reset(tm);

    start->tv_sec = 1000;
    start->tv_nsec = 300000000;
    finish->tv_sec = 1500;
    finish->tv_nsec = 800000000;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 500);
    mt_fail(tm->tv_nsec == 500000000);

    start->tv_sec = 1000;
    start->tv_nsec = 100000000;
    finish->tv_sec = 1500;
    finish->tv_nsec = 800000000;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 1001);
    mt_fail(tm->tv_nsec == 200000000);

    free(tm);
    free(start);
    free(finish);
}


/* ==========================================================================
   ========================================================================== */


void ts_add_diff_realtime_multi_combined(void)
{
    struct timespec *tm;
    struct timespec *start;
    struct timespec *finish;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    opts.clock = CLK_REALTIME;

    mt_fail(tm = ts_new());
    mt_fail(start = ts_new());
    mt_fail(finish = ts_new());

    if (tm == NULL || start == NULL || finish == NULL)
    {
        return;
    }

    ts_reset(tm);

    start->tv_sec = 1000;
    start->tv_nsec = 0;
    finish->tv_sec = 1500;
    finish->tv_nsec = 0;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 500);
    mt_fail(tm->tv_nsec == 0);


    start->tv_sec = 1000;
    start->tv_nsec = 0;
    finish->tv_sec = 1500;
    finish->tv_nsec = 0;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 1000);
    mt_fail(tm->tv_nsec == 0);

    start->tv_sec = 1000;
    start->tv_nsec = 400000000;
    finish->tv_sec = 1500;
    finish->tv_nsec = 900000000;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 1500);
    mt_fail(tm->tv_nsec == 500000000);


    start->tv_sec = 1000;
    start->tv_nsec = 0;
    finish->tv_sec = 1500;
    finish->tv_nsec = 700000000;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 2001);
    mt_fail(tm->tv_nsec == 200000000);

    start->tv_sec = 1000;
    start->tv_nsec = 500000001;
    finish->tv_sec = 1500;
    finish->tv_nsec = 300000000;

    ts_add_diff(tm, start, finish);
    mt_fail(tm->tv_sec == 2500);
    mt_fail(tm->tv_nsec == 999999999);

    free(tm);
    free(start);
    free(finish);
}
#endif


/* ==========================================================================
   ========================================================================== */


void bytes2jedec_test(void)
{
    static const int    KB = 1024;
    static const long   MB = 1024l * 1024;
    static const float  GB = 1024.0 * 1024 * 1024;
    static const float  TB = 1024.0 * 1024 * 1024 * 1024;

    float               b;
    struct jedec        res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    srand(time(NULL));

    for (b = 1; b < 100 * KB; b++)
    {
        bytes2jedec(b, &res);
        mt_assert(res.val == b);
        mt_assert(res.pre == '\0');
    }

    for (b = 100 * KB; b < 100 * MB; b += rand() % KB)
    {
        bytes2jedec(b, &res);
        mt_assert(res.val == (size_t)(b / KB));
        mt_assert(res.pre == 'K');
    }

    for (b = 100 * MB; b < 100 * GB; b += rand () % MB)
    {
        bytes2jedec(b, &res);
        mt_assert(res.val == (size_t)(b / MB));
        mt_assert(res.pre == 'M');
    }

    for (b = 100 * GB; b < 999 * TB; b += rand() % 1024l * MB)
    {
        bytes2jedec(b, &res);
        mt_assert(res.val == (size_t)(b / GB));
        mt_assert(res.pre == 'G');
    }
}


/* ==== opts.c tests ======================================================== */


void opts_parse_default_all(void)
{
    char **argv;
    int    argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("", &argc);
    opts_parse(argc, argv);

    mt_fail(opts.block_size == 16 * 1024);
    mt_fail(opts.report_intvl == 100 * 1024 * 1024);
    mt_fail(opts.num_intvl == 10);
    mt_fail(opts.method == METHOD_MEMCPY);
    mt_fail(opts.cache_size == 1 * 1024 * 1024);

#ifdef HAVE_CLOCK_GETTIME
    mt_fail(opts.clock == CLK_REALTIME);
#else
    mt_fail(opts.clock == CLK_CLOCK);
#endif

    opts_free(argc, argv);
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_b_invalid_prefixes(void)
{
    char  **argv;
    int     argc;
    char    c;
    char    param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (c = 'A'; c != 'Z' + 1; ++c)
    {
        sprintf(param, "-b1%c", c);
        argv = str2opts(param, &argc);

        if (c == 'G' || c == 'M' || c == 'K')
        {
            mt_fail(opts_parse(argc, argv) == 0);
        }
        else
        {
            mt_fail(opts_parse(argc, argv) == -2);
        }

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_b_invalid_param(void)
{
    char  **argv;
    int     argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("-banan", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);


    argv = str2opts("-b", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_b_bytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-b%lu", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == i);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_b_kbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000 ; i <<= 1)
    {
        sprintf(param, "-b%luK", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == i * 1024l);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_b_mbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-b%luM", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == i * 1024l * 1024);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_b_gbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-b%luG", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == i * 1024l * 1024 * 1024);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_r_invalid_prefixes(void)
{
    char  **argv;
    int     argc;
    char    c;
    char    param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (c = 'A'; c != 'Z' + 1; ++c)
    {
        sprintf(param, "-r1%c", c);
        argv = str2opts(param, &argc);

        if (c == 'G' || c == 'M' || c == 'K')
        {
            mt_fail(opts_parse(argc, argv) == 0);
        }
        else
        {
            mt_fail(opts_parse(argc, argv) == -2);
        }

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_r_invalid_param(void)
{
    char  **argv;
    int     argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("-ranan", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);


    argv = str2opts("-r", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_r_bytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-r%lu", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == i);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_r_kbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000 ; i <<= 1)
    {
        sprintf(param, "-r%luK", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == i * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_r_mbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-r%luM", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == i * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_r_gbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-r%luG", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == i * 1024 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_l_invalid_prefixes(void)
{
    char  **argv;
    int     argc;
    char    c;
    char    param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (c = 'A'; c != 'Z' + 1; ++c)
    {
        sprintf(param, "-l1%c", c);
        argv = str2opts(param, &argc);

        if (c == 'G' || c == 'M' || c == 'K')
        {
            mt_fail(opts_parse(argc, argv) == 0);
        }
        else
        {
            mt_fail(opts_parse(argc, argv) == -2);
        }

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_l_invalid_param(void)
{
    char  **argv;
    int     argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("-lanan", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);

    argv = str2opts("-l", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_l_bytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-l%lu", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == i);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_l_kbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000 ; i <<= 1)
    {
        sprintf(param, "-l%luK", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == i * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_l_mbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-l%luM", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == i * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_l_gbytes(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-l%luG", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == 10);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == i * 1024 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_i(void)
{
    char         **argv;
    int            argc;
    unsigned long  i;
    char           param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (i = 1; i < 1000000; i <<= 1)
    {
        sprintf(param, "-i%lu", i);
        argv = str2opts(param, &argc);
        opts_parse(argc, argv);

        mt_fail(opts.block_size == 16 * 1024);
        mt_fail(opts.report_intvl == 100 * 1024 * 1024);
        mt_fail(opts.num_intvl == i);
        mt_fail(opts.method == METHOD_MEMCPY);
        mt_fail(opts.cache_size == 1 * 1024 * 1024);

    #ifdef HAVE_CLOCK_GETTIME
        mt_fail(opts.clock == CLK_REALTIME);
    #else
        mt_fail(opts.clock == CLK_CLOCK);
    #endif

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_i_invalid_param(void)
{
    char  **argv;
    int     argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("-ianan", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);


    argv = str2opts("-i5K", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);


    argv = str2opts("-i", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_m(void)
{
    char **argv;
    int    argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("-mmemcpy", &argc);
    opts_parse(argc, argv);

    mt_fail(opts.block_size == 16 * 1024);
    mt_fail(opts.report_intvl == 100 * 1024 * 1024);
    mt_fail(opts.num_intvl == 10);
    mt_fail(opts.method == METHOD_MEMCPY);
    mt_fail(opts.cache_size == 1 * 1024 * 1024);

#ifdef HAVE_CLOCK_GETTIME
    mt_fail(opts.clock == CLK_REALTIME);
#else
    mt_fail(opts.clock == CLK_CLOCK);
#endif

    opts_free(argc, argv);


    argv = str2opts("-mbbb", &argc);
    opts_parse(argc, argv);

    mt_fail(opts.block_size == 16 * 1024);
    mt_fail(opts.report_intvl == 100 * 1024 * 1024);
    mt_fail(opts.num_intvl == 10);
    mt_fail(opts.method == METHOD_BBB);
    mt_fail(opts.cache_size == 1 * 1024 * 1024);

#ifdef HAVE_CLOCK_GETTIME
    mt_fail(opts.clock == CLK_REALTIME);
#else
    mt_fail(opts.clock == CLK_CLOCK);
#endif
    opts_free(argc, argv);
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_m_invalid_param(void)
{
    char  **argv;
    int     argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("-manan", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);


    argv = str2opts("-m", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_c(void)
{
    char **argv;
    int    argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("-cclock", &argc);
    opts_parse(argc, argv);

    mt_fail(opts.block_size == 16 * 1024);
    mt_fail(opts.report_intvl == 100 * 1024 * 1024);
    mt_fail(opts.num_intvl == 10);
    mt_fail(opts.method == METHOD_MEMCPY);
    mt_fail(opts.cache_size == 1 * 1024 * 1024);
    mt_fail(opts.clock == CLK_CLOCK);

    opts_free(argc, argv);

#ifdef HAVE_CLOCK_GETTIME
    argv = str2opts("-crealtime", &argc);
    opts_parse(argc, argv);

    mt_fail(opts.block_size == 16 * 1024);
    mt_fail(opts.report_intvl == 100 * 1024 * 1024);
    mt_fail(opts.num_intvl == 10);
    mt_fail(opts.method == METHOD_MEMCPY);
    mt_fail(opts.cache_size == 1 * 1024 * 1024);
    mt_fail(opts.clock == CLK_REALTIME);

    opts_free(argc, argv);
#endif
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_opt_c_invalid_param(void)
{
    char  **argv;
    int     argc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    argv = str2opts("-canan", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);


    argv = str2opts("-c", &argc);

    mt_fail(opts_parse(argc, argv) == -2);

    opts_free(argc, argv);
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_unknown_opts(void)
{
    static const char *allowed_opts = "hvbrlimc";

    char  **argv;
    int     argc;
    char    c;
    char    param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (c = 'a'; c != 'z' + 1; ++c)
    {
        sprintf(param, "-%c", c);
        argv = str2opts(param, &argc);

        if (is_in_array(c, allowed_opts))
        {
            mt_fail(opts_parse(argc, argv) != -3);
        }
        else
        {
            mt_fail(opts_parse(argc, argv) == -3);
        }

        opts_free(argc, argv);
    }
}


/* ==========================================================================
   ========================================================================== */


void opts_parse_syntax_error(void)
{
    char  **argv;
    int     argc;
    char    c;
    char    param[16];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for (c = 'A'; c != 'Z' + 1; ++c)
    {
        sprintf(param, "-%c", c);
        argv = str2opts(param, &argc);

        mt_fail(opts_parse(argc, argv) == -1);

        opts_free(argc, argv);
    }

    argv = str2opts("banan", &argc);
    mt_fail(opts_parse(argc, argv) == -1);
    opts_free(argc, argv);

    argv = str2opts("-", &argc);
    mt_fail(opts_parse(argc, argv) == -1);
    opts_free(argc, argv);

    argv = str2opts("--", &argc);
    mt_fail(opts_parse(argc, argv) == -1);
    opts_free(argc, argv);

    argv = str2opts("-10b", &argc);
    mt_fail(opts_parse(argc, argv) == -1);
    opts_free(argc, argv);
}


/* ==== bench.c tests ======================================================= */





/* ==== Public functions ==================================================== */


int main(void)
{
#ifdef HAVE_CLOCK_GETTIME
    mt_run(ts_new_realtime);
    mt_run(ts_realtime);
    mt_run(ts_reset_realtime);
    mt_run(ts_add_diff_realtime_single_simple);
    mt_run(ts_add_diff_realtime_single_with_nsec_no_overflow);
    mt_run(ts_add_diff_realtime_single_with_nsec_overflow_down);
    mt_run(ts_add_diff_realtime_single_with_nsec_overflow_up);
    mt_run(ts_add_diff_realtime_multi_combined);
#endif

    mt_run(ts_new_clock);
    mt_run(ts_clock);
    mt_run(ts_reset_clock);

    mt_run(ts_add_diff_clock_single);
    mt_run(ts_add_diff_clock_multi);

    mt_run(bytes2jedec_test);
    mt_run(opts_parse_default_all);

    mt_run(opts_parse_opt_b_bytes);
    mt_run(opts_parse_opt_b_kbytes);
    mt_run(opts_parse_opt_b_mbytes);
    mt_run(opts_parse_opt_b_gbytes);
    mt_run(opts_parse_opt_r_bytes);
    mt_run(opts_parse_opt_r_kbytes);
    mt_run(opts_parse_opt_r_mbytes);
    mt_run(opts_parse_opt_r_gbytes);
    mt_run(opts_parse_opt_l_bytes);
    mt_run(opts_parse_opt_l_kbytes);
    mt_run(opts_parse_opt_l_mbytes);
    mt_run(opts_parse_opt_l_gbytes);
    mt_run(opts_parse_opt_i);
    mt_run(opts_parse_opt_m);
    mt_run(opts_parse_opt_c);
    mt_run(opts_parse_opt_b_invalid_prefixes);
    mt_run(opts_parse_opt_r_invalid_prefixes);
    mt_run(opts_parse_opt_l_invalid_prefixes);
    mt_run(opts_parse_opt_b_invalid_param);
    mt_run(opts_parse_opt_r_invalid_param);
    mt_run(opts_parse_opt_l_invalid_param);
    mt_run(opts_parse_opt_i_invalid_param);
    mt_run(opts_parse_opt_m_invalid_param);
    mt_run(opts_parse_opt_c_invalid_param);
    mt_run(opts_parse_unknown_opts);
    mt_run(opts_parse_syntax_error);


    mt_return();
}
