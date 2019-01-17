#============================================================================
# Makefile: Create XTRADE + other Library 2004/03/01
#
#============================================================================
bin_PROGRAMS    = FIXTester
DEFS   = -DHAVE_CONFIG_H
AM_CPPFLAGS     = -I$(includedir)/cc++2\
                  -I../../LibXS\
                  -I../../LibXS/Signon\
                  -I../../LibXS/Signon/Policy\
                  -I../../LibExt\
                  -I../../LibStkExt\
                  -I../../LibXsAux
FIXTester_LDFLAGS = -L$(libdir) -ldl -lnsl -lpthread -lstdc++\
                    -lxs -lxsSignon -lxsSignonSvrDB -lxsTwStk -lExt -lStkExt -lxsAux
FIXTester_SOURCES = main02.cpp\
                    main01.cpp\
                    FixDef.cpp\
                    FixSesMgr.cpp\
                    SesFix.cpp\
                    FixRecorder.cpp
                    
#============================================================
