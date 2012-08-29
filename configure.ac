AC_INIT(src/lib/pkgr.c)  # makes sure build directory is correct
CFLAGS="${CFLAGS=} -Os -I./include"
AC_PREREQ(2.12)  # depends on autoconf version >= 2.12
AC_CANONICAL_SYSTEM
AM_SANITY_CHECK
AC_PROG_INSTALL
AC_ARG_PROGRAM
AM_INIT_AUTOMAKE(pkgr, 0.1)  # initialize automake
AC_PROG_LIBTOOL  # initializes libtool
AC_PROG_CC  # check for C compiler
AC_HEADER_STDC  # check for standard C headers
AC_CHECK_HEADERS(lua.h)
AC_CHECK_HEADERS(lauxlib.h)
AC_CHECK_HEADERS(lualib.h)
AC_CHECK_HEADERS(openssl/evp.h)
AC_CHECK_HEADERS(curl/curl.h)
AC_CHECK_HEADERS(sys/ioctl.h)
AC_CHECK_HEADERS(sys/stat.h)
AC_CHECK_HEADERS(sys/types.h)
AC_CHECK_HEADERS(libconfig.h)
AC_C_CONST
AC_SUBST(CFLAGS)
BUILD_OPS="lib cmd"
AC_ARG_ENABLE([command], AS_HELP_STRING([--disable-command], [disable command-line program]))
AM_CONDITIONAL([COMMAND], [test "x$enable_command" = "xno"])
AC_ARG_ENABLE([graphics], AS_HELP_STRING([--enable-graphics], [enable graphical front-end]))
AM_CONDITIONAL([GRAPHICS], [test "x$enable_graphics" = "xyes"])
AC_CONFIG_HEADERS([include/pkgr/build-config.h])
AC_CONFIG_FILES([
  Makefile
])
AC_OUTPUT
