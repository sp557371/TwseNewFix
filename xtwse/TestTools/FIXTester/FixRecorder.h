//---------------------------------------------------------------------------
#ifndef FixRecorderH
#define FixRecorderH
//---------------------------------------------------------------------------
#include "FixDef.h"
#include "KFile.h"
#include "FixEvHand.h"
#include "StdRecorder.h"
namespace Kway {
namespace Tw   {
namespace Fix  {
namespace Stk  {
/////////////////////////////////////////////////////////////////////////////
K_class TStkFixRecorder : private TFixMsgEv
{
   struct SInt {
      int  Num_;
      K_ctor SInt() : Num_(0) {}
   };

   typedef TStkFixRecorder    this_type;
   //typedef Kstl::ev_map<std::string, SInt>     TTag11Map;
   //typedef Kway::TStdRecorder<TTag11Map, SInt> TTag11Recorder;
   std::string _OrgPath;
   TFile       _Loger;
   TFile       _RecvIdx;
   TFile       _SendIdx;
   int         _LastRecvSeqNum;
   int         _LastContSeqNum;
   int         _LastSendSeqNum;
   std::string _FixLogPath;
   std::string _TradeID;
   std::string _LogPath;
   std::string _KwayCompID;
   std::string _TargetCompID;
   int         _Counter;
   //TTag11Map*        Map_;
   //TTag11Recorder*   Recorder_;


   struct SIdxRec
   {
      Kway::TFile::size_type    _Pos;
      long                      _Len;
   };
    	K_mf(void) FillSeqGap      (int aSeqBegin, int aSeqEnd);
    	K_mf(void) FirstMessage    (TStkSesFix*, const TFixMsg&, TFixEv*);
        // event handle
        K_mf(void) OnLogon         (TStkSesFix*, const TFixMsg&);
        K_mf(void) OnLogout        (TStkSesFix*, const TFixMsg&);
        K_mf(void) OnSeqReset      (TStkSesFix*, const TFixMsg&);
        K_mf(void) OnResendReq     (TStkSesFix*, const TFixMsg&);
        K_mf(void) OnSessReject    (TStkSesFix*, const TFixMsg&);
        // end
        K_mf(void) WriteLogIdx     (TStkSesFix*, const SFixStr&, int aMsgSeqNum, Kway::TFile&);
        K_mf(void) WriteAdminMsg   (TStkSesFix*, const SFixStr&);

        K_mf(void) OpenOrderInfo();
        K_mf(void) CloseProfile(TStkSesFix*);

        K_mf(void) WriteLogIdx   (TStkSesFix*, int aMsgSeqNum, Kway::TFile&);
public:
        K_ctor TStkFixRecorder  (const std::string& path);
        K_dtor ~TStkFixRecorder ();
        K_mf(bool) LinkTo       (TStkSesFix*);
        K_mf(bool) Unlink       (TStkSesFix*);
        K_mf(SIdxRec) WriteLog     (TStkSesFix*, const char*);
        K_mf(void)    WriteTitle   (TStkSesFix*, const char*);
        K_mf(void)    WriteErrLog  (TStkSesFix*, const char* remark, const char*);
        K_mf(void)    WriteSendLog (TStkSesFix* aSes, const SFixStr& aFix, int aMsgSeqNum, bool isResend=false);
        K_mf(void)    WriteRecvLog (TStkSesFix* aSes, const SFixStr& aFix, int aMsgSeqNum);

   K_mf(void) CheckContinuousMessage (TStkSesFix*, SFixStr&, TFixEv*);

   K_mf(int)  FetchSeqNum   (TStkSesFix*);
   K_mf(bool) NeedReqResend (TStkSesFix*);

   //K_mf(int)         GetTag11();

   K_mf(bool)        IsLogonOK       ();
   K_mf(std::string) GetTradeID      ()  { return _TradeID;      }
   K_mf(std::string) GetKwayCompID   ()  { return _KwayCompID;   }
   K_mf(std::string) GetTargetCompID ()  { return _TargetCompID; }
   K_mf(std::string) GetLogPath      ()  { return _LogPath;      }
   K_mf(void)	     SetLastRecvSeqNum(TStkSesFix*, int num);
   K_mf(void)	     SetLastContSeqNum(TStkSesFix*, int num);
   K_mf(void)	     SetLastSendSeqNum(TStkSesFix*, int num);
   K_mf(int)	     GetLastRecvSeqNum() { return _LastRecvSeqNum; }
   K_mf(int)	     GetLastContSeqNum() { return _LastContSeqNum; }
   K_mf(int)         GetLastSendSeqNum() { return _LastSendSeqNum; }
   K_mf(bool)        OpenProfile      (TStkSesFix*);
   K_mf(bool)        OpenNewProfile(TStkSesFix* aSes);
   K_mf(void) SetKwayCompID   (const std::string& s)  { _KwayCompID   = s; }
   K_mf(void) SetTargetCompID (const std::string& s)  { _TargetCompID = s; }
};
/////////////////////////////////////////////////////////////////////////////
}; // Stk
}; // Fix
}; // Tw
}; // Kway
#endif
