LIBS =
SRCS = bench.c opts.c utils.c
MAIN = memperf

STD = -std=c89
WARN = -Wall -Wextra -pedantic
CFLAGS = $(STD) $(WARN)

VERSION = `grep "define VERSION" version.h | cut -d \" -f2`
VERSION_MAJOR = `grep "define VERSION" version.h | cut -d \" -f2 | cut -d. -f1`
DESTDIR ?= `cat .destdir`
CC ?=
INC ?=
LINC ?=
POSIX ?=

ifdef POSIX
	STD += -D_POSIX_C_SOURCE=199309L
	CFLAGS += -DPOSIX
endif

OBJS = $(SRCS:.c=.o)

.PHONY: depend clean debug release

all: release

release: CFLAGS += -O2
release: $(MAIN)

debug: CFLAGS += -O0 -ggdb -g3
debug: $(MAIN)

test_debug: CFLAGS += -O0 -ggdb -g3
test_debug: test

test: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(LINC) $(LDFLAGS) -o $(MAIN)-tests $(OBJS) tests.c
	@./memperf-tests

install:
	install -m 0755 -D -t $(DESTDIR)/bin memperf
	echo $(DESTDIR) > .destdir

uninstall:
	$(RM) $(DESTDIR)/bin/memperf

clean:
	$(RM) *.o *~ $(MAIN) $(MAIN)-tests

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(LINC) $(LDFLAGS) -o $(MAIN) $(OBJS) main.c

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
