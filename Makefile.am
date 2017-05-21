bin_PROGRAMS = memperf
memperf_SOURCES = src/bench.c \
	src/main.c \
	src/opts.c \
	src/utils.c

check_PROGRAMS = tests
tests_CFLAGS = -DTESTS -Wno-unused-value
TESTS = $(check_PROGRAMS)
tests_SOURCES = src/bench.c \
	src/opts.c \
	src/utils.c \
	src/tests.c

man1_MANS = memperf.1
