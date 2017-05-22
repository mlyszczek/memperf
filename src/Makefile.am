bin_PROGRAMS = memperf
memperf_SOURCES = bench.c main.c opts.c utils.c

check_PROGRAMS = tests
tests_CFLAGS = -DTESTS -Wno-unused-value
tests_SOURCES = bench.c opts.c utils.c tests.c

TESTS = $(check_PROGRAMS)
LOG_DRIVER = $(top_srcdir)/tap-driver
