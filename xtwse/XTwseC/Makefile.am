#============================================================================
# 如果要讓程式可以使用 2G 的記憶體
# 要在 LDFLAGS 中新增 -N
#============================================================================
bin_PROGRAMS    = XTwseC

DEFS = -DHAVE_CONFIG_H -D_XTwse_
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(includedir)/cc++2\
              -I/usr/local/include/cc++2 \
              -I../LibXS\
              -I../LibXS/Signon\
              -I../LibXS/Signon/Policy\
              -I../LibExt\
              -I../LibStkExt\
              -I../LibXsQFix\
              -I../LibXsAux\
              -I../LibXsSmAppia\
              -I../LibTMPSes\
              -I../LibXsFixStk\
              -I../LibXsTwse

XTwseC_LDFLAGS = -L$(libdir) -ldl -lxs\
                 -L/usr/lib\
                 -lxsTwStk -lExt -lStkExt\
                 -lTMPSession\
                 -lxsAux -lxsQFix -lxsQFixExt -lxsSmAppia -lxsFixStk -lxsTwse

XTwseC_SOURCES = main02.cpp\
                 main01.cpp\
                 OrdScheduler.cpp\
                 OrdSesMgr.cpp

#============================================================
