#============================================================================
# 2004/02/03..07/13
# Makefile.am : create Shared library libnsFixSes (by GNU's libtool)
# Ref: www.gnu.org
#============================================================================
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS     = -I/usr/local/include/cc++2\
                  -I../LibXS\
                  -I../LibXS/Signon\
                  -I../LibXS/Signon/Policy\
                  -I../LibExt
lib_LTLIBRARIES = libStkExt.la
#============================================================================
# Create Library : libnsFixSes.la + Other Library
# Other  Library : libns.la libnsTwStk.la libnsSignon.la
#============================================================================
libStkExt_la_LDFLAGS = -version-info 0:0:0 
libStkExt_la_LIBADD  = -L. -L$(libdir) -lxs -lxsTwStk -lxsSignon  -lExt
libStkExt_la_SOURCES = OrdSesMgrBase.cpp\
                       SysInfo.cpp\
                       Flows.cpp
