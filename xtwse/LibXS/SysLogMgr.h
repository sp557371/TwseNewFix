/**
   @file SysLogMgr.h

log format:
Date Time[apname][ap-id][class-id][runtime-obj-id][msg-id][msg-seq-no]Message\n
ap-id   =HostID
class-id=AAAA.BBBB.CCCC(16�i����)
         AAAA = Module ID(libns,etmpgw,etmpex...)
         BBBB = Class Group(Kernel,Mes,Ses...)
         CCCC = Sequence No.
msg-id  =A.B
         A:Level: 0=Log System Message.
                  1=Critical Message.
                  2=
         B:Message ID.�Y��class�ۭq��[�T��ID]�����ߤ@,�B�PLevel�L��.
runtime-obj-id: dword(hex)
msg-seq-no:     dword(hex)
         [runtime-obj-id]+[msg-seq-no]���ߤ@.

�d��:
2005/04/03 08:05:00[etmpgw][2][0000.0000.0000][0][0.0][0]etmpgw creating.
2005/04/03 08:05:00[etmpgw][2][0001.0002.0003][1][0.0][0]Ses creating.
2005/04/03 13:40:00[etmpgw][2][0001.0002.0003][1][0.1][1]destroying.
2005/04/03 17:00:00[etmpgw][2][0000.0000.0000][0][0.1][1]destroying.

��ap���s�����,�Ҧ���runtime-obj-id���|���s�p��.


�ʬݵ{���i��ܬY��(�άY��)class�۰ʺʬ�,�Ҧp:
�ʬ�: class-id = 0001.0001.0001
      ��o�{���Y�Ӭ۲Ū�class-id���ͥX�s��instance��,�|���@��runtime-obj-id.
      �M��ʬݵ{���N�|�إߤ@�ӹ�����runtime-obj�x�s��obj�����A�ΰT���ܤ�,
      ���즹obj���`����.
�ʬݵ{���|���@�ƭӸ�ƪ�:
   (1) deque<objrec> :�]�t[��������]��[�����T���C��].
   (2) map<class-id, deque<runtime-obj-id>> �Y��class�Ҳ��ͪ�����C��.
   (3) �C�ӰT��([runtime-obj-id] + [msg-seq-no] �� key)���i�H����:
       �t�@�ӰT���B�z���O��.(�Ҧp:���T���w�B�z,�ðO���B�z�L�{...)
       �O���b logFileName.re �Ҧp: "2005/04/03/etmpgw.log.re"
       �榡�p�U:
       Date Time[runtime-obj-id][msg-seq-no]Message.
       Date Time[runtime-obj-id][msg-seq-no][END]     ��ܦ��T�������p�w�Ѱ�.
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

   /** �b InitSysSingleton() ����,
       �z�@�w�n����|�I�s GetSysMgrObjs(),
       �_�h�N���|�O��AP�������T��.
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

