AC_PREREQ([2.69])
AC_INIT([libhep], [1.0.0], [support@sipcapture.org])
AC_CONFIG_SRCDIR([core_hep.c])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])
LT_INIT

AC_PROG_CC
AM_PROG_AR
AC_CANONICAL_HOST

AC_ARG_ENABLE([ipv6],
  AS_HELP_STRING([--disable-ipv6], [disable IPv6 HEP chunks (default: enabled)]),
  [enable_ipv6=$enableval], [enable_ipv6=yes])
AS_IF([test "x$enable_ipv6" = xyes], [AC_DEFINE([USE_IPV6], [1], [Enable IPv6])])

PKG_CHECK_MODULES([ZLIB], [zlib], [AC_DEFINE([USE_ZLIB], [1], [Enable zlib compression])],
  [AC_MSG_WARN([zlib not found — building without payload compression])])

AC_CHECK_HEADERS([netinet/ip.h netinet/tcp.h pthread.h])

AC_CONFIG_FILES([Makefile libhep.pc])
AC_OUTPUT
