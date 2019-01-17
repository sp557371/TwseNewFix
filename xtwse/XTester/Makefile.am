#============================================================================
# Makefile: Create XTRADE + other Library 2004/03/01
#
#============================================================================
bin_PROGRAMS    = XTester

DEFS = -DHAVE_CONFIG_H

AM_CPPFLAGS = -I$(includedir)/cc++2\
              -I/usr/local/include/cc++2 \
              -I../LibXS\
              -I../LibXS/Signon\
              -I../LibXS/Signon/Policy\
              -I../LibExt\
              -I../LibStkExt\
              -I../LibTMPSes\
              -I../LibXsAux\
              -I../LibXsQFix\
              -I../LibXsSmAppia\
              -I../LibXsFixStk\
              -I../LibXsTwse

XTester_LDFLAGS = -L$(libdir) -ldl -lnsl -lpthread -lstdc++ -lxml2 \
                 -lxs -lxsSignon -lxsSignonSvrDB -lxsTwStk -lExt -lStkExt\
                 -lTMPSession\
                 -lxsAux -lxsQFix -lxsQFixExt -lxsSmAppia -lxsFixStk -lxsTwse

XTester_SOURCES = main02.cpp\
                  main01.cpp\
                  OrdSesMgr.cpp\
                  TserIniFile.cpp\
                  TserK02.cpp\
                  TserSesApaSmp.cpp
#============================================================
