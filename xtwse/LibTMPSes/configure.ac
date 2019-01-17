#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([BXSESSION],0.1,KWAY)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE 
AC_CONFIG_SRCDIR([bxDef.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_DISABLE_STATIC
AC_PROG_LIBTOOL 
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lns':
# AC_CHECK_LIB([ns], [main])
# FIXME: Replace `main' with a function in `-lnsTwStk':
# AC_CHECK_LIB([nsTwStk], [main])

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset strchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
