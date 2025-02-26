#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([xl4unibase], [1.1.4], [shiro@excelfore.com])
AC_SUBST(PACKAGE_DESC,"Excelfore universal base library")

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects foreign])
AM_PROG_AR
LT_PREREQ([2.2])
LT_INIT([static])
AC_CONFIG_SRCDIR([unibase.h])

# Checks for programs.
PKG_PROG_PKG_CONFIG
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O
AC_PROG_CC_C99

# pkg-config dependency
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Check if posix platform
AC_CHECK_HEADERS([pthread.h],
	[AM_CONDITIONAL(POSIX_PLATFORM,[true])])

# Check for command line options
AC_ARG_ENABLE([unittest],
	AS_HELP_STRING([--disable-unittest],[disable building unittest programs]))
AC_ARG_ENABLE([unittest_memtest],
	AS_HELP_STRING([--enable-unittest-memtest],[check memory leak in unittest]))

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h string.h])

if test x"$enable_unittest" != "xno"; then
AC_CHECK_HEADERS([setjmp.h cmocka.h],[],
	[AC_MSG_ERROR([you may need to install libcmocka-dev package])],
	[[#include <string.h>
	#include <stdio.h>
	#include <setjmp.h>
	]])
fi

# conditional build
AM_CONDITIONAL(UNITTEST_MEMTEST, [test x"$enable_unittest_memtest" = "xyes"])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNC([malloc], [], [AC_MSG_ERROR([malloc() function not found.])])
AC_CHECK_FUNC([realloc], [], [AC_MSG_ERROR([realloc() function not found.])])
AC_CHECK_FUNC([memmove], [], [AC_MSG_ERROR([memmove() function not found.])])
AC_CHECK_FUNC([memset], [], [AC_MSG_ERROR([memset() function not found.])])
AC_CHECK_FUNC([strtol], [], [AC_MSG_ERROR([strtol() function not found.])])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([
    Makefile
    xl4unibase.pc])

AC_OUTPUT
