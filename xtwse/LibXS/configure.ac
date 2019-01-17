#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([ns project], 0.1, fonwin@ms21.hinet.net)

# 一些輔助configure的檔案(install-sh…之類)存放的位置，
# 此行一定要放在AC_INIT()之後，否則產生出來的configure會找不到install-sh
AC_CONFIG_AUX_DIR(config)

AC_CONFIG_SRCDIR([AlphaSeq.cpp])
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# initial automake
AM_INIT_AUTOMAKE

# AC_CONFIG_HEADER(output:inputs)，inputs若有多個file時,可用「:」隔開?
AC_CONFIG_HEADER(nsconfig_unix.h)

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
# 禁止使用 static library, 因為 nslib 有許多共用物件變數,
# 若使用 static library 將會造成許多問題!
AC_DISABLE_STATIC

# 因為ns project會產生share library所以要檢查libtool
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([netipx/ipx.h])
AC_CHECK_HEADERS([alloca.h])

# FreeBSD
AC_CHECK_FUNCS([kqueue])
# Solaris
AC_CHECK_HEADERS([sys/devpoll.h])
# Linux
AC_CHECK_HEADERS([sys/epoll.h])

# Checks for typedefs, structures, and compiler characteristics.
# AC_HEADER_STDBOOL
# AC_C_CONST
# AC_C_INLINE
# AC_TYPE_MODE_T
# AC_TYPE_OFF_T
# AC_TYPE_SIZE_T
# AC_HEADER_TIME
# AC_STRUCT_TM
# AC_C_VOLATILE
# AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
# AC_FUNC_MALLOC
# AC_FUNC_MKTIME
# AC_FUNC_REALLOC
# AC_FUNC_SELECT_ARGTYPES
# AC_FUNC_VPRINTF
# AC_CHECK_FUNCS([ftruncate gettimeofday inet_ntoa localtime_r memchr memmove memset mkdir select socket strchr strerror strrchr strstr strtol strtoul])

# 因為 libns 需要用到 dlopen()..., 而有些OS會將此類函式放在 libdl.so, 所以要檢查是否有 libdl.so
if test -e /usr/lib/libdl.so  ||  test -e /usr/local/lib/libdl.so; then
   LDL="-ldl"
else
   LDL=""
fi
AC_SUBST(LDL)

# 檢查 thread 放在哪個 lib 裡面, 參考 OpenTop Library 的寫法
# Look for sched_yield.  Up to Solaris 2.6, it is in libposix4, since
# Solaris 7 the name librt is preferred.
NS_THREAD_LIB_OPTIONS=""
AC_CHECK_LIB([pthread],
	[pthread_create],
	[NS_THREAD_LIB_OPTIONS="-lpthread"],
 	[AC_CHECK_LIB(pthreads, pthread_create, NS_THREAD_LIB_OPTIONS="-lpthreads",
 	    [AC_CHECK_LIB(c_r, pthread_create, NS_THREAD_LIB_OPTIONS="-lc_r",
		[AC_CHECK_LIB(pthread, __pthread_attr_init_system, NS_THREAD_LIB_OPTIONS="-lpthread",
			[AC_CHECK_FUNC(pthread_create)]
			)]
 		    )]
 		)]
 	)
case "$target" in
*-*-freebsd*)
	NS_THREAD_LIB_OPTIONS="-pthread"
        ;;
esac

# hp/ux 的 sem_init() 必須 -lrt 
AC_CHECK_LIB([rt], [sem_init], [NS_THREAD_LIB_OPTIONS="$NS_THREAD_LIB_OPTIONS -lrt"], [])

AC_SUBST(NS_THREAD_LIB_OPTIONS)

# 產生 Makefile
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
