#============================================================================
# Makefile: Create XTRADE + other Library 2004/03/01
#
#============================================================================
bin_PROGRAMS    = XFT
DEFS   = -DHAVE_CONFIG_H 

AM_CPPFLAGS     = -I$(includedir)/cc++2\
                  -I/usr/local/include/cc++2 \
                  -I../LibXS\
                  -I../LibXS/Signon\
                  -I../LibXS/Signon/Policy\
                  -I../LibExt\
		  -I./source/base\
		  -I./source/bxSes\
		  -I./source/bxLink\
		  -I./source/bxFT\
		  -I./source/FTCenter\
		  -I./source/Request\
		  -I./source/Notify\
		  -I./source/bxDR\
		  -I./source/LibFTrm\
		  -I./source/Module\
                  -I../LibTMPSes
XFT_LIBADDR    = -L$(libdir)
XFT_LDFLAGS    = -L. -L$(libdir) -lxs -ldl -lxsSignon -lxsSignonSvrDB\
                 -lxsTwStk -lExt -lFTExt -lpthread -lnsl
XFT_SOURCES    = \
		 ./source/bxSes/bxSes.cpp\
		 ./source/bxSes/bxSesMgr.cpp\
                 ./source/base/SesPacket.cpp\
		 ./source/base/SesLibBase.cpp\
		 ./source/base/BxLoger.cpp\
		 ./source/bxSes/PvcTable.cpp\
		 ./source/bxLink/bxLinkSes.cpp\
		 ./source/bxLink/bxLinkWorker.cpp\
		 ./source/bxFT/bxFTSes.cpp\
		 ./source/bxFT/bxFTWorker.cpp\
		 ./source/bxFT/FileCtrl.cpp\
		 ./source/FTCenter/FTJob.cpp\
		 ./source/FTCenter/FTJobDef.cpp\
		 ./source/FTCenter/FTSchedule.cpp\
		 ./source/FTCenter/FTCenter.cpp\
		 ./source/Notify/KwayNotify.cpp\
		 ./source/Notify/Notify3T.cpp\
		 ./source/Notify/SystexNotify.cpp\
		 ./source/Notify/NotifyMgr.cpp\
		 ./source/Request/TwseReq.cpp\
		 ./source/Request/ReqMgr.cpp\
		 ./source/bxDR/bxDR.cpp\
		 ./source/bxDR/bxDRSes.cpp\
		 ./source/bxDR/bxDRWorker.cpp\
		 ./source/Module/FTSysSetting.cpp\
                 ./../LibXS/MesTCPClientTSE.cpp\
                 ../LibTMPSes/MesTSEClient.cpp\
                 ../LibTMPSes/EvDevTseTcp.cpp\
		 FileLog.cpp main.cpp\
                 runmain.cpp
#============================================================
#Waitting Test: None
#                 ./source/Module/DspMgr.cpp\
#============================================================
