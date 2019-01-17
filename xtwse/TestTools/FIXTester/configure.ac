#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])
AC_INIT([FIXTester], 0.1, KWAY)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([SesFix.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lExt':
AC_CHECK_LIB([Ext], [main])
# FIXME: Replace `main' with a function in `-lStkExt':
AC_CHECK_LIB([StkExt], [main])
# FIXME: Replace `main' with a function in `-lns':
AC_CHECK_LIB([xs], [main])
# FIXME: Replace `main' with a function in `-lnsSignon':
AC_CHECK_LIB([xsSignon], [main])
# FIXME: Replace `main' with a function in `-lnsSignonSvrDB':
AC_CHECK_LIB([xsSignonSvrDB], [main])
# FIXME: Replace `main' with a function in `-lnsTwStk':
AC_CHECK_LIB([xsTwStk], [main])
# FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([xsl], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MKTIME
AC_CHECK_FUNCS([memchr memset strchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
