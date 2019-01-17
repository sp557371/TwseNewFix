/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrdTrack_h_)
#define _FixStkOrdTrack_h_
//---------------------------------------------------------------------------
#include "Nsa_Time.h"
#include "tplns/Nsa_EvCtn.h"
#include "tplns/Nsa_EvMap_2k.h"
#include "tplns/Nsa_EvMap_2k_Hnd.h"
#include "QFixExt/QFixTagDef.h"
#include "FixStk.h"
   
/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

namespace FIX {
namespace Stk {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// struct SOrdTrack
/////////////////////////////////////////////////////////////////////////////
struct SOrdTrack
{
   TCommInn::rr_key mutable  RoomKey_;

   TTimeStamp       RecvTime_;
   TFixOrdCmdID     OrdCmd_;
   TFixMsgSeqNum    MsgSeqNum_;     // 034
   TFixMsgType2     MsgType_;       // 035
   TFixClOrdID      ClOrdID_;       // 011
   TFixOrigClOrdID  OrigClOrdID_;   // 041
   TFixQty2         OrderQty_;      // 038
   TFixPri2         Price_;         // 044
   TFixOrdFlowID    FlowID_;
   TFixChar         ReseChar_[8];   // FixChar 保留區

   inline K_mf(void) clear ()
      {
         TCommInn::rr_key roomKey = RoomKey_;
      
         *this    = SOrdTrack(); 
         RoomKey_ = roomKey;
     }

   inline K_mf(void*)     GetDataBeg ()  { return &RecvTime_; }
   inline K_mf(TFixChar*) GetRChar   ()  { return ReseChar_;  }

   inline K_mf(const TFixChar*) GetRChar    () const  { return ReseChar_;  }
   inline K_mf(const void*)     GetDataBeg  () const  { return &RecvTime_; }
   inline K_mf(size_t)          GetDataSize () const  { return (sizeof(SOrdTrack) - sizeof(TCommInn::rr_key)); }

   //**********************************************
   #if defined(_FixTag_Twse_Ver2_)
   K_mf(void) operator = (const SOrdTrack& a)
   {
      memcpy(this, &a, sizeof(SOrdTrack)-sizeof(TCommInn::rr_key));
   }

   #else //----------------------------------------
      
   K_mf(void) operator = (const SOrdTrack& a)
   {
      RecvTime_    = a.RecvTime_;
      OrdCmd_      = a.OrdCmd_;
      MsgSeqNum_   = a.MsgSeqNum_;
      MsgType_     = a.MsgType_;
      ClOrdID_     = a.ClOrdID_;
      OrigClOrdID_ = a.OrigClOrdID_;
      OrderQty_    = a.OrderQty_;
      Price_       = a.Price_;
      FlowID_      = a.FlowID_;
   }
   #endif
   //**********************************************
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// class TOrdTrack
/////////////////////////////////////////////////////////////////////////////
K_class TOrdTrack
{
public:
   //==============================================
   // TRecsType
   typedef TEvMap2k<TFixSysOrdID, TFixSysSeqNo, SOrdTrack>  TRecsType;
   typedef TRecsType::TMap1            TRsSysOrdID;
   typedef TRecsType::TMapI1           TRsSysOrdIDI;
   typedef TRecsType::TMap2            TRsSeqNo;
   typedef TRecsType::TMapI2           TRsSeqNoI;
   typedef TRecsType::TMapU2           TRsSeqNoU;
   typedef std::pair<TRsSeqNoI, bool>  TRsSeqNoIns;

   //==============================================
   // TRecsHnd
   K_class TRecsHnd : public TEvMap2kHnd<TRsSysOrdID>
   {
      typedef TEvMap2kHnd<TRsSysOrdID>  inherited;
      
   public:
      enum ENewStatus
      {
         ens_Found,     // 找到尚未傳送回報的 New 委託
         ens_Sent,      // 已經有傳送回報的紀錄了
         ens_NotFound,  // 找不到 New 委託紀錄
      };

   public:
      inline K_ctor TRecsHnd (TOrdTrack* p, const TFixSysOrdID& key1)
         : inherited(&(p->RsSysOrdID_), key1)
      {}

      K_mf(ENewStatus) FindNewStatus  ();
      K_mf(bool)       FindFinished   (const TFixMsgType2&);
      K_mf(bool)       FindUnfinished (const TFixMsgType2&);
   };
   friend class TRecsHnd;

public:
   K_ctor  TOrdTrack (const std::string&);
   K_dtor ~TOrdTrack ();

   K_mf(TRsSeqNoU) AddTrack    (const TFixSysOrdID&, const SOrdTrack&);
   K_mf(TRsSeqNoU) GetNewTrack (const TFixSysOrdID&, const SOrdTrack&);
   
   K_mf(TRsSeqNo*) FindRsSeqNo (const TFixSysOrdID&);
   K_mf(TRsSeqNo*) GetRsSeqNo  (const TFixSysOrdID&);

private:
   class TRecsRecorder;

   TRsSysOrdID     RsSysOrdID_;
   TRecsRecorder*  RecsRecorder_;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrdTrack_h_)

