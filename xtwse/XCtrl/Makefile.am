#============================================================================
# Makefile: Create XCtrlSes 2009/11/11
#
#============================================================================
ACLOCAL_AMFLAGS = -I m4

INCLUDES = -I$(includedir)/cc++2\
           -I/usr/local/include/cc++2 \
           -I../LibXS\
           -I../LibXS/Signon\
           -I../LibXS/Signon/Policy\
           -I../LibExt\
           -I../LibStkExt\
           -I../LibXsAux\
           -I../LibXsQFix\
           -I../LibXsSmAppia\
           -I../LibXsTwse

DEFS = -DHAVE_CONFIG_H

lib_LTLIBRARIES = libxsXCtrlSes.la

libxsXCtrlSes_la_LDFLAGS = -version-info 0:0:0 

libxsXCtrlSes_la_LIBADD = -L$(libdir) -lnsl -lpthread -lstdc++ -lxml2\
                          -lxs -lxsSignon -lxsSignonSvrDB -lxsTwStk -lExt -lStkExt\
                          -lxsAux -lxsQFix -lxsQFixExt -lxsSmAppia -lxsTwse

libxsXCtrlSes_la_SOURCES = CtrlFieldType.cpp\
                           CtrlRsReqResu.cpp\
                           CtrlSesAppia.cpp\
                           CtrlSesTandem.cpp\
                           CtrlSesMgr.cpp\
                           CtrlSesIniter.cpp\
                           CtrlIniFile.cpp

#============================================================================
# Makefile: Create XCtrl 2009/11/01
#
#============================================================================
bin_PROGRAMS  = XCtrl

XCtrl_LDFLAGS = -L. -L$(libdir) -ldl -lpthread -lxml2\
                -lxs -lxsSignon -lxsSignonSvrDB -lxsTwStk -lExt -lStkExt\
                -lxsAux -lxsQFix -lxsQFixExt -lxsSmAppia -lxsTwse

XCtrl_SOURCES = main02.cpp\
                main01.cpp\
                THConf.cpp
