# -----------------------------------------
# Makefile.am : 
# create shared library liborm 2007/10/16  
# -----------------------------------------
AM_CPPFLAGS=-I$(includedir)/cc++2/cc++\
            -I/usr/local/include/cc++2\
            -I/kway/Project/XTrade/LibNS\
            -I/kway/Project/XTrade/LibExt\
            -I/kway/Project/XTrade/FT/source/FTCenter\
            -I/kway/Project/XTrade/FT/source/bxSes\
            -I/kway/Project/XTrade/FT/source/bxLink\
            -I/kway/Project/XTrade/FT/source/bxFT\
            -I/kway/Project/XTrade/FT/source/Notify\
            -I/kway/Project/XTrade/FT/source/Module\
            -I/kway/Project/XTrade/FT/source/LibFTrm
lib_LTLIBRARIES=libFTrm.la
# -------------------------------------------
# Create Library : liborm.la + Other Library
# Other  Library : libns.la libnsTwStk.la
# -------------------------------------------
libFTrm_la_LDFLAGS=-version-info 0:0:0 -no-undefined
libFTrm_la_LIBADD =-L. -lns -L. -lnsTwStk -L. -lExt 
libFTrm_la_SOURCES=FTRCSes.cpp\
                   FTRSesMgr.cpp\
                   FTRModule.cpp\
                   FTRSSes.cpp
