LIBS =
SRCS = bench.c opts.c utils.c main.c
MAIN = memperf

STD = -std=c99 -D_POSIX_C_SOURCE=199309L
WARN = -Wall -Wextra -pedantic
CFLAGS = $(STD) $(WARN)

VERSION = `grep "define VERSION" version.h | cut -d \" -f2`
VERSION_MAJOR = `grep "define VERSION" version.h | cut -d \" -f2 | cut -d. -f1`
DESTDIR ?= `cat .destdir`
CC ?=
INC ?=
LINC ?=
THREADS ?=


OBJS = $(SRCS:.c=.o)

.PHONY: depend clean debug release

all: release

release: CFLAGS += -O2
release: $(MAIN)

debug: CFLAGS += -O0 -ggdb -g3
debug: $(MAIN)

install:
	install -m 0755 -D -t $(DESTDIR)/bin memperf
	echo $(DESTDIR) > .destdir

uninstall:
	$(RM) $(DESTDIR)/bin/memperf

clean:
	$(RM) *.o *~ $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(LINC) $(LDFLAGS) -o $(MAIN) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
