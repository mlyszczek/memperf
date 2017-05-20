AC_INIT([memperf], [0.1.0])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AX_CHECK_COMPILE_FLAG([-Wall], [CFLAGS="$CFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra], [CFLAGS="$CFLAGS -Wextra"])
AC_CHECK_FUNCS(clock_gettime)
AC_REQUIRE_AUX_FILE([tap-driver.sh])
AC_PROG_CC
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
