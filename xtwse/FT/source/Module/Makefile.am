#============================================================================
# 2006/10/12
# Makefile.am : create Shared library libFTExt Module (by GNU's libtool)
# Ref: www.gnu.org
#============================================================================
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS     = -I$(includedir)/cc++2 -I../../../LibXS\
		  -I../../../LibExt\
		  -I../base\
		  -I../FTCenter\
		  -I../bxSes\
		  -I../bxLink\
		  -I../bxFT
lib_LTLIBRARIES = libFTExt.la
#============================================================================
# Create Library : libFTExt.la + Other Library
# Other  Library : libns.la
#============================================================================
libFTExt_la_LDFLAGS = -version-info 0:0:0  
libFTExt_la_LIBADD  = -L. -L$(libdir) -lxs -lExt
libFTExt_la_SOURCES = DspMgr.cpp\
	       	      FTSysSetting.cpp\
                      SetsVAC.cpp\
                      FTExtDef.cpp
