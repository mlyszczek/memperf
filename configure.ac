AC_INIT([memperf], [0.1.0], [michal.lyszczek@bofc.pl])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AX_CHECK_COMPILE_FLAG([-Wall], [CFLAGS="$CFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra], [CFLAGS="$CFLAGS -Wextra"])
AC_CHECK_FUNCS(clock_gettime)
AC_DEFINE([_POSIX_C_SOURCE], [199309L], [Define the POSIX version])
AC_PROG_CC
AC_PROG_CC_C89
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
