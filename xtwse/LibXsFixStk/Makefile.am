#============================================================================
# Makefile: Create XTRADE + other Library 2004/03/01
#
#============================================================================
#ACLOCAL_AMFLAGS = -I m4

INCLUDES = -I$(includedir)/cc++2\
           -I/usr/local/include/cc++2 \
           -I../LibXS\
           -I../LibXS/Signon\
           -I../LibXsAux\
           -I../LibXsQFix

DEFS = -DHAVE_CONFIG_H 

lib_LTLIBRARIES = libxsFixStk.la

libxsFixStk_la_LDFLAGS = -version-info 0:0:0

libxsFixStk_la_LIBADD  = -L. -L$(libdir) -lccgnu2 $(LDL)\
                         -lxs -lxsAux -lxsQFix -lxsQFixExt

libxsFixStk_la_SOURCES = FixStk.cpp\
                         FixStkOrdTrack.cpp\
                         FixStkOrder.cpp\
                         FixStkPer.cpp\
                         FixStkIntf.cpp\
                         FixStkCoids.cpp\
                         FixStkSysOrd.cpp\
                         FixStkOrdCenter.cpp\
                         FixStkOrdCenter_Aux.cpp\
                         FixStkOrdFlow.cpp\
                         FixStkOrdFlowReq.cpp\
                         FixStkRptTrack.cpp\
                         FixStkRptCenter.cpp\
                         FixStkOrdCenterVA.cpp\
                         FixStkRptFlow.cpp\
                         FixStkRptFlowReq.cpp\
                         FixStkTradeMgr.cpp
