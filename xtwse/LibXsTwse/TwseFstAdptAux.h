/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFstAdptAux_h_)
#define _TwseFstAdptAux_h_
//---------------------------------------------------------------------------
#include "TwseFstAdpt.h"
#include "TwStkTrProfile.h"
#include "TwStkOrdCenter.h"
#include "TwseFixField.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TOrdImpl
/////////////////////////////////////////////////////////////////////////////
class TTwseFstAdpt::TOrdImpl : public Tw::Stk::TTrProfile
{
   // Tw.Stk
   typedef Tw::Stk::TTrProfile         inherited;
   typedef Tw::Stk::TSysOrdID          TSysOrdID;
   typedef Tw::Stk::TOrdFields_ForNew  TOrdFields_ForNew;
   typedef Tw::Stk::TStkOrder_Rec      TStkOrder_Rec;
   // Fix.Stk
   typedef FIX::Stk::TFixRefOrdID      TFixRefOrdID;

   friend class TOrdStepImpl;

   TTwseFstAdpt*  Adpt_;
   TOrdFlow*      OrdFlow_;
   TOrdCenter*    OrdCenter_;
   TSysOrds*      SysOrds_;
   TRptImpl*      RptImpl_;
   TFixTradeLog*  FixTradeLog_;
   TFixOfReqPtr   FixReq_[FIX::Stk::fcid_Total];

public:
   K_ctor TOrdImpl (TTwseFstAdpt*);

   K_mf(bool) DoReqNew     (TFixOfReqPtr);
   K_mf(bool) DoReqDel     (TFixOfReqPtr);
   K_mf(bool) DoReqChgQty  (TFixOfReqPtr);
   K_mf(bool) DoReqQry     (TFixOfReqPtr);
   K_mf(bool) DoReqAutoNew (TFixOfReqPtr);

   // 驗證委託狀態
   // 不存在: 自動新增,並填入相關欄位
   //   存在: 更新相關欄位
   K_mf(TSysOrdID) VerifyOrder (TFixOfReqPtr, const TFixRefOrdID&);

   //==============================================
   // 以下是不透過
   K_mf(bool) ReReqNew    (TOrdCenter::TOrdHnd&);
   K_mf(bool) ReReqDel    (TOrdCenter::TOrdHnd&, const SFixOrdTrack&);
   K_mf(bool) ReReqChgQty (TOrdCenter::TOrdHnd&, const SFixOrdTrack&);
   K_mf(bool) ReReqQry    (const TSysOrdID&);

   inline K_mf(void) SetRptImpl (TRptImpl* i)  { RptImpl_ = i; }
   
   inline K_mf(TFixOfReqPtr*) GetFixReq (EFixOrdCmdID foid)  { return &(FixReq_[foid]); }

protected:
   //==============================================
   // overwrite TTrProfile virtual function
   // 所有的回報在 OnOrdAfEndUpdate 處理,所以這邊不需要處理回報
   K_mf(bool) OnOrdReqUpdate (TOrdFlow::TReqPtr);

   //==============================================
   // TOrdImpl function
   K_mf(void) InitOrdForNew (const TFixSysOrdID&, const TFixStkOrder&, TOrdFields_ForNew&);
   K_mf(void) InitOrdForRec (                     const TFixStkOrder&, TStkOrder_Rec&);
};


/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TOrdStepImpl
/////////////////////////////////////////////////////////////////////////////
class TTwseFstAdpt::TOrdStepImpl : public TOrdFlow::TFlowStep
{
   typedef FIX::Stk::SOfOrdField  SFixOfOrdField;
   
   TTwseFstAdpt*  Adpt_;
   TOrdImpl*      OrdImpl_;

public:
   K_ctor  TOrdStepImpl (TTwseFstAdpt*);
   K_dtor ~TOrdStepImpl ();

private:
   //==============================================
   // overwrite TFlowStep virtual function
   K_mf(bool) OnOrdReq (TOrdFlow::TReqPtr);
};


/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TRptImpl
/////////////////////////////////////////////////////////////////////////////
class TTwseFstAdpt::TRptImpl : public Tw::Stk::TSysOrdsEvHandler
{
   typedef Tw::Stk::TSysOrdsEvHandler  inherited;
   
   // Tw.Stk
   typedef Tw::Stk::TSysOrdsEvHandler  TSysOrdsEvHandler;
   typedef Tw::Stk::EOrdFlowID         EOrdFlowID;
   typedef Tw::Stk::EOrdCmdID          EOrdCmdID;
   typedef Tw::Stk::TStkQty            TStkQty;
   typedef Tw::Stk::TStkOrder          TStkOrder;
   typedef Tw::Stk::TStkDeals          TStkDeals;

   enum {
      TseCode_WrntMktMkr = 255,
   };

public:
   struct SRptInfo
   {
      EOrdFlowID        FlowID_;
      EOrdCmdID         CmdID_;
      const TStkOrder*  StkOrd_;
      TFixSysOrdID      FixSysOrdID_;
   };
   
   TTwseFstAdpt*  Adpt_;
   TSysOrds*      SysOrds_;
   TFixRptFlow*   FixRptFlow_;
   TFixTradeLog*  FixTradeLog_;
   const int&     FixVer_;

public:
   K_ctor  TRptImpl (TTwseFstAdpt*);
   K_dtor ~TRptImpl ();

   K_mf(bool) Execute (SRptInfo&);

protected:
   //==============================================
   // overwrite TSysOrdsEvHandler virtual function
   K_mf(TSysOrdsEvHandler::TEvResult) OnOrdAfEndUpdate (TSysOrds::TUpdaterImpl&);

   // 我的設計是不使用 OnOrdRecover, 所以這邊不可以宣告喔
   //K_mf(TSysOrdsEvHandler::TEvResult) OnOrdRecover     (TSysOrdID, const TStkOrder*);

   //==============================================
   // TRptImpl function
   K_mf(void) Exec_Finish                  (SRptInfo&);
   K_mf(void) Exec_Finish_New              (SRptInfo&);
   K_mf(void) Exec_Finish_Del              (SRptInfo&);
   K_mf(void) Exec_Finish_ChgQty           (SRptInfo&);
   K_mf(void) Exec_Finish_Del_ChgQty       (SRptInfo&, EFixOrdCmdID);
   K_mf(void) Exec_Finish_Qry              (SRptInfo&);
   K_mf(void) Exec_AbortOrdClear           (SRptInfo&);
   K_mf(void) Exec_AbortOrdNoChange        (SRptInfo&);
   K_mf(void) Exec_AbortOrdNoChange_Del    (SRptInfo&);
   K_mf(void) Exec_AbortOrdNoChange_ChgQty (SRptInfo&);
   K_mf(void) Exec_AbortOrdNoChange_Qry    (SRptInfo&);
   K_mf(void) Exec_Deal                    (const TStkOrder*, const TStkDeals::const_iterator&, TSysOrds::TUpdaterImpl&);

   K_mf(void) Set_FixBaseField (const TStkOrder*, SFixRfBaseField&, bool hasErr);
   K_mf(void) Set_FixErField   (const TStkOrder*, SFixRfErField&);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFstAdptAux_h_)
