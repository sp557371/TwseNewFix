/**
   @file SysLogMgr.h

log format:
Date Time[apname][ap-id][class-id][runtime-obj-id][msg-id][msg-seq-no]Message\n
ap-id   =HostID
class-id=AAAA.BBBB.CCCC(16進位表示)
         AAAA = Module ID(libns,etmpgw,etmpex...)
         BBBB = Class Group(Kernel,Mes,Ses...)
         CCCC = Sequence No.
msg-id  =A.B
         A:Level: 0=Log System Message.
                  1=Critical Message.
                  2=
         B:Message ID.某個class自訂的[訊息ID]必須唯一,且與Level無關.
runtime-obj-id: dword(hex)
msg-seq-no:     dword(hex)
         [runtime-obj-id]+[msg-seq-no]為唯一.

範例:
2005/04/03 08:05:00[etmpgw][2][0000.0000.0000][0][0.0][0]etmpgw creating.
2005/04/03 08:05:00[etmpgw][2][0001.0002.0003][1][0.0][0]Ses creating.
2005/04/03 13:40:00[etmpgw][2][0001.0002.0003][1][0.1][1]destroying.
2005/04/03 17:00:00[etmpgw][2][0000.0000.0000][0][0.1][1]destroying.

當ap重新執行時,所有的runtime-obj-id都會重新計算.


監看程式可選擇某個(或某類)class自動監看,例如:
監看: class-id = 0001.0001.0001
      當發現有某個相符的class-id產生出新的instance時,會有一個runtime-obj-id.
      然後監看程式就會建立一個對應的runtime-obj儲存此obj的狀態及訊息變化,
      直到此obj死亡為止.
監看程式會維護數個資料表:
   (1) deque<objrec> :包含[全部物件]的[全部訊息列表].
   (2) map<class-id, deque<runtime-obj-id>> 某個class所產生的物件列表.
   (3) 每個訊息([runtime-obj-id] + [msg-seq-no] 為 key)都可以產生:
       另一個訊息處理註記表.(例如:緊急訊息已處理,並記錄處理過程...)
       記錄在 logFileName.re 例如: "2005/04/03/etmpgw.log.re"
       格式如下:
       Date Time[runtime-obj-id][msg-seq-no]Message.
       Date Time[runtime-obj-id][msg-seq-no][END]     表示此訊息的狀況已解除.
*/
//---------------------------------------------------------------------------
#ifndef SysLogMgrH
#define SysLogMgrH
//---------------------------------------------------------------------------
#include "KFile.h"
#include "MLog_IDs.h"
#include <stdarg.h>  //for: va_list
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
LIBNS_class TSysMgr;
LIBNS_class TSysLogMgr;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMsgLog
{
   TSysLogMgr*    LogMgr_;
   TClassID       ClassID_;
   dword          RuntimeObjID_;
   dword          MsgSeqNo_;

   K_mf(void) Write (EMsgLevel, dword msgID, const char* msgFmt, va_list argptr);
public:
           K_ctor  TMsgLog  (const TClassID&, const char* className, TSysLogMgr* = NULL);
   virtual K_dtor ~TMsgLog  ();

   void  Write  (const TMsgID& msgid, const char* defaultMsg, ...);
   void  Write  (EMsgLevel, const TDefaultMsg*, ...);

   K_mf(void) WriteFileErr  (EMsgLevel, const TDefaultMsg&, const TFile&);
   K_mf(void) WriteFileName (const TDefaultMsg&, const TFile&);
};
//---------------------------------------------------------------------------
LIBNS_class TSysLogMgr
{
   LIBNS_class TLogFile : public ::Kway::TFile {
      typedef ::Kway::TFile   inherited;
   public:
      K_ctor  TLogFile (const std::string& logFileName);
      K_dtor ~TLogFile ();
   };
   TLogFile       LogFile_;
   std::string    ApName_;
   dword          ApID_;
   dword          LastRuntimeObjID_;
   TMsgLog        MLog_;
   TKDate         LastMsgDate_;

public:
   K_ctor  TSysLogMgr  (const std::string& logFileName, const std::string& apName, dword apID);
   K_dtor ~TSysLogMgr  ();

   inline K_mf(dword)  FetchRuntimeObjID  ()  { return ++LastRuntimeObjID_; }

   /** 在 InitSysSingleton() 之後,
       您一定要找機會呼叫 GetSysMgrObjs(),
       否則將不會記錄AP結束的訊息.
   */
   static K_mf(TSysLogMgr&) InitSysSingleton (const std::string& logFileName, const std::string& apName, dword apID);
   static K_mf(TSysLogMgr*) GetSysSingleton  ();

   K_mf(void)  Write  (const char*, int len);

   inline K_mf(const ::Kway::TFile&) GetLogFile () const  { return LogFile_; }
   inline K_mf(dword)                GetApID    () const  { return ApID_;    }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

