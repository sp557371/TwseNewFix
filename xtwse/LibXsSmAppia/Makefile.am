#============================================================================
# Makefile: Create XTRADE + other Library 2004/03/01
#
#============================================================================
ACLOCAL_AMFLAGS = -I m4

INCLUDES = -I$(includedir)/cc++2\
           -I/usr/local/include/cc++2 \
           -I../LibXS\
           -I../LibXS/Signon\
           -I../LibXS/tinyxml\
           -I../LibXsAux\
           -I../LibXsQFix

DEFS = -DHAVE_CONFIG_H 

lib_LTLIBRARIES = libxsSmAppia.la

libxsSmAppia_la_LDFLAGS = -version-info 0:0:0

libxsSmAppia_la_LIBADD  = -L$(libdir) -lccgnu2 $(LDL)\
                          -L../LibXS\
                          -L../LibXsAux\
                          -lxs -lxsAux -lxsQFix -lxsQFixExt

libxsSmAppia_la_SOURCES = ApaSmpType.cpp\
                          ApaSmpMsg.cpp\
                          ApaSesConf.cpp\
                          ApaSesSmp.cpp\
                          ApaSesSmpFix.cpp\
                          ApaSesSmpFixSole.cpp\
                          ApaSesSmpFixMult.cpp\
                          ApaSesSmpFixUtil.cpp
