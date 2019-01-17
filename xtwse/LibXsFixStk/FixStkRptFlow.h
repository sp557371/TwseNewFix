/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkRptFlow_h_)
#define _FixStkRptFlow_h_
//---------------------------------------------------------------------------
// libns
#include "wise_ptr.h"
// libnsAux
#include "tpl/Nsa_EvObjCtn.h"
// QFix
#include "src/Code/Message.h"
// QFixExt
#include "QFixExt/QFixTagType.h"
// FixStk
#include "FixStkRes.h"
#include "FixStkOrder.h"
#include "FixStkOrdCenter.h"
#include "FixStkRptCenter.h"
#include "FixStkOrdTrack.h"
#include "FixStkRptFlowReq.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
   using namespace Nsa;

namespace FIX {
namespace Stk {
//---------------------------------------------------------------------------
class TOrdTrack;
class TRptTrack;
class TTradeInfo;
class TTradeLog;

/////////////////////////////////////////////////////////////////////////////
// macro
/////////////////////////////////////////////////////////////////////////////
#define FixStk_SetFixAll_Required(FixMsg, QfField, KwField)    \
   {                                                           \
      FixMsg.setField(QfField(KwField.as_fix()));              \
   }                                                           \
//---------------------------------------------------------------------------
#define FixStk_SetFixDef_Optional(FixMsg, QfField, KwField)    \
   {                                                           \
      if (KwField.is_null() == false)                          \
         FixMsg.setField(QfField(KwField.as_fix()));           \
   }                                                           \
//---------------------------------------------------------------------------
#define FixStk_SetFixStr_Optional(FixMsg, QfField, KwField)    \
   FixStk_SetFixDef_Optional(FixMsg, QfField, KwField)         \
//---------------------------------------------------------------------------
#define FixStk_SetFixQty_Optional(FixMsg, QfField, KwField)    \
   FixStk_SetFixDef_Optional(FixMsg, QfField, KwField)         \
//---------------------------------------------------------------------------
#define FixStk_SetFixPri_Optional(FixMsg, QfField, KwField)    \
   FixStk_SetFixDef_Optional(FixMsg, QfField, KwField)         \
//---------------------------------------------------------------------------
#define FixStk_SetFixStr_Optional2(FixMsg, FieldNum, KwStr)    \
   {                                                           \
      if (KwStr.is_null() == false)                            \
         FixMsg.setField(FieldNum, KwStr.as_fix());            \
   }                                                           \
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
// class TRfSpecRule
/////////////////////////////////////////////////////////////////////////////
enum ERfSpecRule
{
   // ExecutionReport: 在打成交 Fix 電文時, OrderQty = 0
   rsr_ErDeal_OrderQty_Zero,

   // ExecutionReport: 在打成交 Fix 電文時, LeavesQty = 0
   rsr_ErDeal_LeavesQty_Zero,

   rsr_Total,
};
typedef std::bitset<rsr_Total>  TRfSpecRule;
      
/////////////////////////////////////////////////////////////////////////////
// class TRptFlow
/////////////////////////////////////////////////////////////////////////////
K_class TRptFlow
{
public:
   typedef TFsOrdCenter::TOrdHnd  TOrdHnd;

   enum EFlowPrio
   {
      efp_None,
         efp_PreSet,
   };
   
   //==================================================================
   // class TFlowStep
   class TFlowStepWorker;
   //--------------------
   class TFlowStep
   {
      friend class TFlowStepWorker;

      // 傳回false時,表示中斷委託流程!
      virtual K_mf(bool) OnRptReq (TRfReqPtr) = 0;
   };

   //==================================================================
   // friend class
   friend class TRfReqBase;
   friend class TRfReqErBase;
   friend class TRfReqErOrdBase;
   friend class TRfReqErRejectBase;
   friend class TRfReqErDealBase;
   friend class TRfReqOcrBase;

   //==================================================================
   // 執行委託要求
   K_mf(bool) DoReqErOrd    (TRfReqErOrd);
   K_mf(bool) DoReqErReject (TRfReqErReject);
   K_mf(bool) DoReqErDeal   (TRfReqErDeal);
   K_mf(bool) DoReqOcr      (TRfReqOcr);
   K_mf(bool) DoReqReject   (TRfReqReject);

   //==================================================================
   // TRptFlow inline function
   inline K_mf(bool) AddFlowStep (TFixRptCmdID rid, TFlowStep* fs, EFlowPrio fp = efp_None)
      { return FlowSteps_[rid].Add((TFlowSteps::TKey)fp, fs); }

   inline K_mf(void) DelFlowStep (TFixRptCmdID rid, TFlowStep* fs)  { return FlowSteps_[rid].Del(fs); }

   //==============================================
   // RptFlow Req Create
   static inline K_mf(TRfReqErOrd) CreateReqErOrd (const TFixSysOrdID& soid, EFixOrdCmdID oid)
      { return TRfReqErOrd(new TRfReqErOrdBase(soid, oid)); }

   static inline K_mf(TRfReqErReject) CreateReqErReject (const TFixSysOrdID& soid, EFixOrdCmdID oid, int rejCode = wrc_None)
      { return TRfReqErReject(new TRfReqErRejectBase(soid, oid, rejCode)); }

   static inline K_mf(TRfReqErDeal) CreateReqErDeal (const TFixSysOrdID& soid)
      { return TRfReqErDeal(new TRfReqErDealBase(soid)); }

   static inline K_mf(TRfReqOcr) CreateReqOcr (const TFixSysOrdID& soid, EFixOrdCmdID oid, int rejCode = wrc_None)
      { return TRfReqOcr(new TRfReqOcrBase(soid, oid, rejCode)); }

   static inline K_mf(TRfReqReject) CreateReqReject (const TFixSysOrdID& soid, EFixOrdCmdID oid, int rejCode = wrc_None)
      { return TRfReqReject(new TRfReqRejectBase(soid, oid, rejCode)); }

protected:
   typedef TEvObjCtn<TFlowStep>  TFlowSteps;

   TTradeInfo*    TradeInfo_;
   TOrdTrack*     OrdTrack_;
   TFsOrdCenter*  OrdCenter_;
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   TRptTrack*     RptTrack_;
   #endif
   //**********************************************
   TRptCenter*    RptCenter_;
   TTradeLog*     TradeLog_;
   TFlowSteps     FlowSteps_[frid_Total];
   TRfSpecRule    SpecRule_;
   
public:
   K_ctor TRptFlow (TTradeInfo*);

protected:
   //==============================================
   // TRptFlow virtual function
   virtual K_mf(void) VirBfOnUpdate_Er  (TRfReqErBase&)  = 0;
   virtual K_mf(void) VirBfOnUpdate_Ocr (TRfReqOcrBase&) = 0;

   virtual K_mf(bool) VirErOrd_FindOrdTrack (TRfReqErOrdBase&, TOrdTrack::TRecsHnd& rshOt, const TFixMsgType2& fixMsgType)
      { return rshOt.FindUnfinished(fixMsgType); }

   //==============================================
   // TRptFlow function
   K_mf(bool) DoFlowSteps (TRfReqBase&);
   K_mf(bool) DoReqBase   (TRfReqBase&);
   K_mf(bool) DoReqErBase (TRfReqErBase&);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkRptFlow_h_)
