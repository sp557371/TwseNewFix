#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([XTWSE],0.1,KWAY)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([main01.cpp])
AC_CONFIG_HEADER([config.h])
#AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
#AC_PROG_INSTALL
#AC_PROG_LN_S
#AC_PROG_MAKE_SET
#AC_PROG_RANLIB
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lccgnu2':
AC_CHECK_LIB([ccgnu2], [main])
# FIXME: Replace `main' with a function in `-lns':
AC_CHECK_LIB([xs], [main])
# FIXME: Replace `main' with a function in `-lnsSignon':
AC_CHECK_LIB([xsSignon], [main])
# FIXME: Replace `main' with a function in `-lnsSignonSvrDB':
AC_CHECK_LIB([xsSignonSvrDB], [main])
# FIXME: Replace `main' with a function in `-lnsTwStk':
AC_CHECK_LIB([xsTwStk], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([memchr memset strchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
