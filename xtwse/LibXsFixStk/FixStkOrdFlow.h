/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrdFlow_h_)
#define _FixStkOrdFlow_h_
//---------------------------------------------------------------------------
// libnsAux
#include "tpl/Nsa_EvObjCtn.h"
// FixStk
#include "FixStkOrdFlowReq.h"
#include "FixStkRptFlow.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

namespace FIX {
namespace Stk {
//---------------------------------------------------------------------------
class TTradeInfo;
class TTradeLog;
//---------------------------------------------------------------------------
#define FixStk_GetFixMsg(FixMsg, QfField, KwField)    \
   {                                                  \
      QfField QfField##Tmp;                           \
      if (FixMsg.findField(QfField##Tmp))             \
         KwField.assign(QfField##Tmp);                \
   }                                                  \
//---------------------------------------------------------------------------
// FixStk_FindTagVal
#define FixStk_FindTagVal(FixMsg, TagId, KwFd)        \
   if (FixMsg.findField(TagId, tagVal))               \
      KwFd.assign(tagVal);                            \
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// ECoidStatus
/////////////////////////////////////////////////////////////////////////////
enum ECoidStatus
{
   ecs_Empty,     // ClOrdID 空白
   ecs_Duplicate, // ClOrdID 重複
   ecs_Valid,     // ClOrdID 是有效的
};

/////////////////////////////////////////////////////////////////////////////
// EFindOrdResult
/////////////////////////////////////////////////////////////////////////////
// 尋找[舊委託]的結果
enum EFindOrdResult
{
   for_Success,
      for_OrderNotFound,   // 找不到原始委託
      for_ClOrdIDDup,      // ClOrdID 重覆
      for_OrderDup,        // 重覆的委託
      for_OrdContNotMatch, // 委託內容不符
};

/////////////////////////////////////////////////////////////////////////////
// class TOfSpecRule    (OrdFlow特殊的處理邏輯)
/////////////////////////////////////////////////////////////////////////////
enum EOfSpecRule
{
   // 一般的 Fix 是使用[剩餘量],證交所是使用[刪減量]
   osr_ChgQty_UseDec,

   // 停用[改價]判斷; Ex:證交所沒有改價功能
   osr_ChgPri_Disable,

   osr_Total,
};
typedef std::bitset<osr_Total>  TOfSpecRule;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdFlow
/////////////////////////////////////////////////////////////////////////////
K_class TFsOrdFlow
{
public:
   typedef TFsOrdCenter::TOrdHnd  TOrdHnd;

   /*
   enum EReqOldOrdResu
   {
      ror_Success,
         ror_OrderNotFound, // 找不到原始委託
         ror_ClOrdIDDup,    // ClOrdID 重覆
         ror_OrderDup,      // 重覆的委託
   };
   */

   //==============================================
   // class TFlowStep
   class TFlowStepWorker;

   class TFlowStep
   {
      friend class TFlowStepWorker;

      // 傳回false時,表示中斷委託流程!
      virtual K_mf(bool) OnFsOrdReq (TOfReqPtr) = 0;
   };
   
   //==============================================
   // 執行委託要求
   K_mf(bool) DoReqNos  (TOfReqNos);
   K_mf(bool) DoReqOcr  (TOfReqOcr);
   K_mf(bool) DoReqOcrr (TOfReqOcrr);
   K_mf(bool) DoReqOsr  (TOfReqOsr);
   K_mf(bool) DoReqAno  (TOfReqAno);
   K_mf(bool) DoReqRno  (TOfReqRno);

   //==============================================
   // TFsOrdFlow inline function
   //inline K_mf(bool) AddFlowStep (TFixOrdCmdID cid, TFlowStep* fs, EFlowPrio fp = efp_None)
   //   { return FlowSteps_[cid].Add((TFlowSteps::TKey)fp, fs); }
   inline K_mf(bool) AddFlowStep (TFixOrdCmdID cid, TFlowStep* fs)  { return FlowSteps_[cid].Add(fs); }
   inline K_mf(void) DelFlowStep (TFixOrdCmdID cid, TFlowStep* fs)  { return FlowSteps_[cid].Del(fs); }

   inline K_mf(void) SetRptFlow (TRptFlow* rf)  { RptFlow_ = rf; }
   
   inline K_mf(TOrdTrack*)    GetOrdTrack  () const  { return OrdTrack_;  }
   inline K_mf(TRptFlow*)     GetRptFlow   () const  { return RptFlow_;   }
   inline K_mf(TFsOrdCenter*) GetOrdCenter () const  { return OrdCenter_; }
   
   //==============================================
   // OrdFlow Req Create
   static inline K_mf(TOfReqNos)  CreateReqNos  (const TMessageRef& m)  { return TOfReqNos (new TOfReqNosBase (m)); }
   static inline K_mf(TOfReqOcr)  CreateReqOcr  (const TMessageRef& m)  { return TOfReqOcr (new TOfReqOcrBase (m)); }
   static inline K_mf(TOfReqOcrr) CreateReqOcrr (const TMessageRef& m)  { return TOfReqOcrr(new TOfReqOcrrBase(m)); }
   static inline K_mf(TOfReqOsr)  CreateReqOsr  (const TMessageRef& m)  { return TOfReqOsr (new TOfReqOsrBase (m)); }

   static inline K_mf(TOfReqAno) CreateReqAno (const TMessageRef& m, TFixRefOrdID roid, bool isValidOrd)
      { return TOfReqAno(new TOfReqAnoBase(m, roid, isValidOrd)); }

   static inline K_mf(TOfReqRno) CreateReqRno ()  { return TOfReqRno(new TOfReqRnoBase); }

protected:
   typedef TEvObjCtn<TFlowStep>  TFlowSteps;

   TTradeInfo*     TradeInfo_;
   TCoids*         Coids_;
   TFsOrdCenter*   OrdCenter_;
   TRptFlow*       RptFlow_;
   TOrdTrack*      OrdTrack_;
   TTradeLog*      TradeLog_;
   TFlowSteps      FlowSteps_[fcid_Total];
   TOfSpecRule     SpecRule_;
   // 抓 Fix 電文欄位設定
   TOfOrdFieldSet  OfSetNos_;
   TOfOrdFieldSet  OfSetOcr_;
   TOfOrdFieldSet  OfSetOcrr_;
   TOfOrdFieldSet  OfSetOsr_;
   TOfOrdFieldSet  OfSetAno_;

public:
   K_ctor TFsOrdFlow (TTradeInfo*);
   
   // 設定 fix 版本號
   K_mf(void) SetFixVer (int);

   inline K_mf(TFsOrdCenter*) GetOrdCenter ()  { return OrdCenter_; }

protected:
   //======================================================
   // TFsOrdFlow virtual function
   //======================================================
   // 取得舊的委託
   virtual K_mf(void)           VirInsNewOrd  (const TFixSysOrdID&, const TFsStkOrder&) {}
   virtual K_mf(EFindOrdResult) VirFindOldOrd (TOfReqOldBase&, TFixSysOrdID&);
   
   // 下刪改查作業前會先準備一些基本欄位
   virtual K_mf(void) VirOnGetOrdFiled (const Message&, SOfOrdField&) = 0;

   // 產生新委託時, 需要填入 StkOrd 資料的事件
   virtual K_mf(void) VirInitNewOrdField (const Message&, const SOfOrdField&, TFsStkOrder&) = 0;
   
   // NewOrderSingle 的事件
   virtual K_mf(bool) OnProcReqNos (TOfReqNosBase*) = 0;
   
   // OrderCancelRequest 的事件
   virtual K_mf(void) VirBfProcOcr (TOfReqOcrBase*) = 0;
   virtual K_mf(bool) VirProcOcr   (TOfReqOcrBase*) = 0;

   // OrderCancelReplaceRequest 的事件
   virtual K_mf(void) VirBfProcOcrr (TOfReqOcrrBase*) = 0;
   virtual K_mf(bool) VirProcOcrr   (TOfReqOcrrBase*) = 0;
   
   // OrderStatusRequest 的事件
   virtual K_mf(void) VirBfProcOsr (TOfReqOsrBase*) = 0;
   virtual K_mf(bool) VirProcOsr   (TOfReqOsrBase*) = 0;
   
   // 當委託找不到時的處理機制
   virtual K_mf(bool) VirProcOrderNotFound (TOfReqOldBase*) = 0;

   //======================================================
   // TFsOrdFlow function
   //======================================================
   // 尋找原始委託, 檢查 ClOrdID
   K_mf(EFindOrdResult) DoReqOld (TOfReqOldBase&, SOrdTrack&);
   
   // 發現 ClOrdID 重複的處理機制
   K_mf(void) ProcClOrdIDTheSame (EFixOrdCmdID, TFixSysOrdID);

   // 處理前置作業發現的錯誤
   //K_mf(void) ProcReqOldError (TOfReqOldBase&);
   K_mf(void) ProcReqError (TOfReqBase&, int rejCode);

   // 從 Fix 電文中取到定義欄位
   K_mf(void) GetOrdFiled (const TOfOrdFieldSet&, const Message&, SOfOrdField&);

   // 在產生新委託時, 需要填入 StkOrd 的資料
   K_mf(void) InitOrdField (const Message&, const SOfOrdField&, TFsStkOrder&);

   // 開始處理委託囉
   K_mf(bool) DoFlowSteps (TOfReqBase&);
};   

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrdFlow_h_)
