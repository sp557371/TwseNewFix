/////////////////////////////////////////////////////////////////////////////
#if !defined(_ApaSesSmpFixUtil_h_)
#define _ApaSesSmpFixUtil_h_
//---------------------------------------------------------------------------
#include "ev_map.h"
#include "TimerThread.h"
#include "ApaSesSmpFix.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace FIX;

/////////////////////////////////////////////////////////////////////////////
// class TApaFixSeqNoFile
/////////////////////////////////////////////////////////////////////////////
K_class TApaFixSeqNoFile
{
public:
   enum EKind
   {
      ekd_Rt,
      ekd_Rcv,
      ekd_RcvTimer,
      ekd_RcvEnd,
   };

private:
   // 2009/09/21 14:06:09.260 0000001 A Rt
   // 2009/09/21 14:06:09.260 0000002 A Rcv
   TFile  File_;
   bool   IsOverwrite_;
   bool   IsAutoFlush_;

public:
   K_ctor TApaFixSeqNoFile (const char* filePath, bool isAutoFlush);

   K_mf(void)    Append       (const TSmpInt&, const TFixMsgType2&, EKind);
   K_mf(TSmpInt) GetLastSeqNo ();
};

/////////////////////////////////////////////////////////////////////////////
// class TApaFixRecover
/////////////////////////////////////////////////////////////////////////////
K_class TApaFixRecover
{
   typedef TApaFixRecover     this_type;
   typedef TTimer<this_type>  TTimerType;

   struct SRsSeqNo
   {
      TFixMsgType2  MsgType_;
      TMessageRef   MsgRef_;
      bool          IsRt_;
   };
   typedef ev_map<TSmpInt, SRsSeqNo>   TRsSeqNo;
   typedef TRsSeqNo::iterator          TRsSeqNoI;
   typedef std::pair<TRsSeqNoI, bool>  TRsSeqNoIns;

   TApaSesSmpFix*     SesFix_;
   TFileWriterHnd&    LogFileHnd_;
   TSmpString         ApaConnSesID_;
   TSmpString         FixTargetCompID_;
   TSmpInt            LastSeqNo_;
   TApaFixSeqNoFile*  SeqNoFile_;
   TRsSeqNo           RsSeqNo_;
   TTimerType         Timer_;

public:
   K_ctor  TApaFixRecover (TApaSesSmpFix*, const TSmpString& apaConnSesID, const TSmpString& fixTargetCompID);
   K_dtor ~TApaFixRecover ();

   K_mf(void) Init        ();
   K_mf(void) CheckFirst  (const TSmpInt&);
   K_mf(bool) CheckRt     (const TSmpInt&, const TFixMsgType2&, TMessageRef);
   K_mf(bool) CheckRcv    (const TSmpInt&, const TFixMsgType2&, TMessageRef);
   K_mf(void) CheckRcvEnd (const TSmpInt&);

private:
   K_mf(void) LineUp  (const TSmpInt&, const TFixMsgType2&, TMessageRef, bool, const char*);
   K_mf(void) OnTimer (TTimerBase*);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_ApaSesSmpFixUtil_h_)
