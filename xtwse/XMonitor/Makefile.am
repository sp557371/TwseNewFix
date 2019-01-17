#============================================================================
# Makefile: Create XMonitor + other Library 2009/09/11
#
#============================================================================
bin_PROGRAMS    = XMonitor

DEFS = -DHAVE_CONFIG_H

AM_CPPFLAGS = -I$(includedir)/cc++2\
              -I../LibXS\
              -I../LibXS/Signon\
              -I../LibXS/Signon/Policy\
              -I../LibExt\
              -I../LibStkExt

XMonitor_LDFLAGS = -L$(libdir) -ldl -lnsl -lpthread -lstdc++ -lxml2\
                   -lxs -lxsSignon -lxsSignonSvrDB -lxsTwStk -lExt

XMonitor_SOURCES =  main02.cpp\
                    main01.cpp\
                    ProcMon.cpp
#============================================================
