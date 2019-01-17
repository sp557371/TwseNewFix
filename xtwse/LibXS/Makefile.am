#
# Makefile.am for ns projects
# $Id: Makefile.am,v 1.23 2005/08/26 01:49:29 fonwin Exp $
#

INCLUDES        = -I$(includedir)/cc++2 \
                  -I/usr/local/include/cc++2 \
                  -I/usr/include/cc++2 \
                  -I../MS \
                  -I./VC/libxsIPC
                  
lib_LTLIBRARIES = libxs.la \
                  libxsSignon.la \
                  libxsSignonSvrDB.la \
                  libsneSignonAdm2.la \
                  libxsTwStk.la \
                  libsneTwStk.la \
                  libxsfmerg.la 
#                  libxsIPC.la \
#                  libxsGroupIPC.la

#sneTwStk:定義自行使用 dlopen() 載入的擴充模組, 不需要安裝到 /usr/--- 的目錄下
#  原本用 noinst_LTLIBRARIES = libsneTwStk.la 的方式,
#  可是如此一來將不會產生 .so 的檔案,
#  ** 為了方便,在此先直接採用 libtool 安裝的方式 **

libxs_la_LDFLAGS = -version-info 0:0:0
libxs_la_LIBADD  = -L$(libdir) -lccgnu2 -lccext2 $(LDL)
libxs_la_CXXFLAGS = -lxml2
#        -L↑指定 libccgnu2 的路徑,一定要加,否則會找不到
libxs_la_SOURCES = AlphaSeq.cpp ev_container.cpp fixed_num.cpp\
        inn_file.cpp KFile.cpp KTime.cpp ModuleLoader.cpp\
        TextReader.cpp Bool3.cpp\
        VACrControl.cpp ViewArea.cpp ViewAreaMgr.cpp\
        ViewCursor.cpp ExtFuncs.cpp MemPool.cpp EncDecStr.cpp\
        SyncEv.cpp SyncEv_UNIX.cpp SyncEv_AppRun.cpp nsinit.cpp\
        EvDev.cpp InpHandler.cpp TimerThread.cpp TokenStr.cpp\
        ServerList.cpp Signon/SignonSvr.cpp Signon/PkSignonWorkers.cpp\
        Mes.cpp MesRegs.cpp MesBaseSocket.cpp MesUDP.cpp\
        MesIPX.cpp MesMulticast.cpp MesTCPClient.cpp\
        MesTCPServer.cpp MesTCPServerOnce.cpp\
        MesTCPServerBase.cpp SocketPortBase.cpp\
        MesFile.cpp MesSerial.cpp MesSerialImpl.cpp\
        MesModem.cpp\
        EvDevSocket.cpp EvDevTcp.cpp MesTcpCliSetting.cpp\
        Ses.cpp SesPk.cpp SesServer.cpp SesSvrVA.cpp\
        SesClient.cpp Signon/SignonClient.cpp\
        SesCliPkFT.cpp SesSvrPkFT.cpp\
        SesBufferLine.cpp SesExtab.cpp\
        HiLoTrans.cpp\
        SesMgr.cpp SesMgr_Aux.cpp SysMgrObjs.cpp\
        SysLogMgr.cpp MLog_IDs.cpp SysErrMsg.cpp\
        kpersistent.cpp DlLib.cpp IdxFile.cpp IdxFileCW.cpp KIniFile.cpp\
        LineBuffer.cpp MesIPC.cpp SesIPC.cpp \
        Signon/Policy/PolicyBase.cpp \
        Signon/Policy/UPRemoteMgr.cpp sha1.cpp\
        cxa_atexit.cpp cxa_dll_fini.cpp\
        tinyxml/tinystr.cpp tinyxml/tinyxml.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinyxmlparser.cpp
# HP上可能需要加上 cxa_*.cpp 因為要解決[static objects 解構的問題]
# 而且, 要先: export CPPFLAGS=-fuse-cxa-atexit
# 然後才執行: ./configure --prefix=/home/fonwin --enable-shared=yes --enable-static=no

libxsSignon_la_LDFLAGS  = -version-info 0:0:0
libxsSignon_la_SOURCES  = Signon/Policy/UFGroups.cpp\
        Signon/Policy/UFTwStkRes.cpp\
        Signon/Policy/UPTwStkScItems.cpp\
        Signon/Policy/UPTwStkSaleNos.cpp\
        Signon/Policy/UPTwStkIvacNos.cpp\
        Signon/Policy/UPTwStkRights.cpp\
        Signon/Policy/UPPass.cpp\
        Signon/Policy/UPRemoteMgr.cpp

libxsSignonSvrDB_la_LDFLAGS = -version-info 0:0:0
libxsSignonSvrDB_la_LIBADD  = -L. -lxs   -L. -lxsSignon
#            -L↑指定 libxs、libxsSignon的路徑,
#  libtool會自動找到 .libs/libxs.la
#  第二個 -L. 也一定要加上,否則libtool會找到 .libs/.libs/libsnsSignon.la
#  這應該是 libtool 的 bug?
libxsSignonSvrDB_la_SOURCES = Signon/PmBase.cpp  Signon/PmBase_Impl.cpp\
            Signon/PmUFrom.cpp Signon/PmUFrom_Impl.cpp\
            Signon/PmUFromIPv4.cpp\
            Signon/PmUserDB.cpp\
            Signon/SignonSvrDB.cpp Signon/SignonSvr_DC.cpp\
            Signon/sneLoader.cpp\
            Signon/UserPassPolicy.cpp\
            Signon/UserRemoteMgrPolicy.cpp\
            cxa_dll_fini.cpp

libsneSignonAdm2_la_LDFLAGS  = -version-info 0:0:0
libsneSignonAdm2_la_LIBADD  = -L. -lxs   -L. -lxsSignonSvrDB  -L. -lxsSignon
libsneSignonAdm2_la_SOURCES  = Signon/SignonAdm2.cpp\
        Signon/SignonAdm2Mgr.cpp\
        Signon/SignonAdm2Policy.cpp\
        cxa_dll_fini.cpp

libxsTwStk_la_LDFLAGS  = -version-info 0:0:0
libxsTwStk_la_LIBADD  = -L. -lxs
libxsTwStk_la_SOURCES  = TwStk.cpp TwStkBaseVA.cpp\
        TwStkBroker.cpp TwStkBrokerVA.cpp\
        TwStkCreditVA.cpp\
        TwStkDeal.cpp TwStkDealVA.cpp\
        TwStkIvacNo.cpp TwStkSaleNo.cpp\
        TwStkOrdCenter.cpp TwStkOrdCenter_Aux.cpp\
        TwStkOrder.cpp TwStkOrdFlow.cpp\
        TwStkSysOrd.cpp TwStkTrProfile.cpp\
        TwStkTseOrdKey.cpp TwStkClOrdIDMgr.cpp

libsneTwStk_la_LDFLAGS  = -version-info 0:0:0
libsneTwStk_la_LIBADD  = -L. -lxs   -L. -lxsSignonSvrDB  -L. -lxsSignon\
        -L. -lxsTwStk
libsneTwStk_la_SOURCES  = Signon/sneTwStkStart.cpp\
        Signon/sneTwStkStart_Aux.cpp\
        Signon/UserTwStkRes.cpp\
        Signon/UserTwStkIvacNos.cpp\
        Signon/UserTwStkSaleNos.cpp\
        Signon/UserTwStkRights.cpp\
        Signon/UserTwStkScItems.cpp

libxsfmerg_la_LDFLAGS  = -version-info 0:0:0
libxsfmerg_la_LIBADD  = -L. -lxs
libxsfmerg_la_SOURCES  = nsfmerg.cpp nsfmerg_obj.cpp nsfmerg_mgr.cpp


#
# bin programs: pkftc
#
bin_PROGRAMS  = pkftc

pkftc_LDFLAGS = -lxs -ldl $(NS_THREAD_LIB_OPTIONS)
pkftc_SOURCES = pkftc.cpp pkftc_base.cpp
