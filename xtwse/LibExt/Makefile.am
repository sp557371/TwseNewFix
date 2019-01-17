# -----------------------------------------
# Makefile.am : 
# create shared library liborm 2004/07/13  
# -----------------------------------------
#includedir=/kway/rd/xtwse/kwaylib/include
AM_CPPFLAGS=-I$(includedir)/cc++2\
            -I../LibXS\
            -I../LibExt
lib_LTLIBRARIES=libExt.la
# -------------------------------------------
# Create Library : libExt.la + Other Library
# Other  Library : libns.la libnsTwStk.la
# -------------------------------------------
libExt_la_LDFLAGS=-version-info 0:0:0 
libExt_la_LIBADD =-L. -L$(libdir) -lxs -lxsTwStk
libExt_la_SOURCES=PacketSes.cpp\
                  ExtDef.cpp\
                  StdRecorder.cpp\
                  SchedulerBase.cpp\
                  SenderBase.cpp\
                  ORBase.cpp\
                  stdSettingP.cpp\
                  MsgCenter.cpp\
                  ScheduleMgr.cpp\
                  ExcMgrBase.cpp\
                  ExcMgr.cpp\
                  EvSesMgr.cpp\
                  OpenApMsg.cpp\
                  SesMgrR.cpp\
                  TableLogger.cpp
