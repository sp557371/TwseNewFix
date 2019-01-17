#============================================================================
# Makefile: Create XTRADE + other Library 2004/03/01
#
#============================================================================
ACLOCAL_AMFLAGS = -I m4

INCLUDES = -I$(includedir)/cc++2\
           -I/usr/local/include/cc++2 \
           -I../LibXS\
           -I../LibXS/Signon\
           -I../LibExt\
           -I../LibXsAux\
           -I../LibXsQFix\
           -I../LibXsSmAppia\
           -I../LibXsFixStk

DEFS = -DHAVE_CONFIG_H 

lib_LTLIBRARIES = libxsTwse.la

libxsTwse_la_LDFLAGS = -version-info 0:0:0

libxsTwse_la_LIBADD  = -L. -L$(libdir) -lccgnu2 $(LDL)\
                       -lxs -lExt\
                       -lxsAux -lxsQFix -lxsQFixExt -lxsSmAppia -lxsFixStk

libxsTwse_la_SOURCES = TwseRes.cpp\
                       TwseExcCode.cpp\
                       TwseType.cpp\
                       TwseIniFile.cpp\
                       TwseFixField.cpp\
                       TwseFsOrd.cpp\
                       TwseFsOrdFlow.cpp\
                       TwseFsRptFlow.cpp\
                       TwseFstFact.cpp\
                       TwseFstUnit.cpp\
                       TwseFstAdpt.cpp\
                       TwseFstAdptAux.cpp\
                       TwseFstAdptOrd.cpp\
                       TwseFstAdptRpt.cpp\
                       TwseFsOrdCenterVA.cpp\
                       TwseSesApaSmp.cpp\
                       TwseDrProc.cpp
