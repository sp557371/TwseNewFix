#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([XFT], 0.1, kway)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([FileLog.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
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
#AC_CHECK_HEADERS([fcntl.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
#AC_STRUCT_TM

# Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([bzero isascii memchr memset strchr strerror strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
