#============================================================================
# Makefile.am : Create Shared library libbxSession  2004/07/08
#
#============================================================================
#cppdir=/kway/dev/kwaylib/include
#                  -I$(cppdir)/cc++2\
#                  -I/development/kwaylib/include/cc++2\
INCLUDES        = -I../LibXS\
                  -I../LibExt
AM_CPPFLAGS     = -I$(includedir)/cc++2\
                  -I/usr/local/include/cc++2 \
                  -I../LibXS\
                  -I../LibExt
DEFS            = -DHAVE_CONFIG_H -D__Log__
lib_LTLIBRARIES = libTMPSession.la
#lib_LTLIBRARIES = libTMPSession.la
#============================================================================
# Create Library : libbxsession.la + Other Library
# Other  Library : libns.la libnsTwStk.la liborm.la
# Note:
# MesTSEClient.h : include in.h, path=/usr/include/netinet/in.h, 
#                  standard directory variable=oldincludedir, 
#                  GCC don't check this variable, useed by makefile command 
#	                  MesTSEServer.cpp\
#	                  MesTSEServerBase.cpp\
#============================================================================
libTMPSession_la_LDFLAGS = -version-info 0:0:0
libTMPSession_la_LIBADD  = -L. -L$(libdir) -lxs -lxsTwStk -L. -lExt
libTMPSession_la_SOURCES = bxDef.cpp\
                          bxSes.cpp\
                          TsePkSes.cpp\
                          TseOrdSes.cpp\
                          TseMatSes.cpp\
                          bxSesMgr.cpp\
                          bxLink.cpp\
                          bxOrder.cpp\
                          bxRpt.cpp\
			  bxLinkWorker.cpp\
			  bxLinkSes.cpp\
			  bxOrderWorker.cpp\
			  bxOrderSes.cpp\
                          bxExtOrdSes.cpp\
			  bxRecorder.cpp\
			  bxRptWorker.cpp\
			  bxRptSes.cpp\
			  PMachSes.cpp\
			  DRSes.cpp\
	                  MesTSEClient.cpp\
	                  EvDevTseTcp.cpp\
			  bxTestSes.cpp\
			  ResetErrCnt.cpp\
                          bxSender.cpp

