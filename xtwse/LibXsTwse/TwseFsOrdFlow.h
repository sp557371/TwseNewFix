/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFsOrdFlow_h_)
#define _TwseFsOrdFlow_h_
//---------------------------------------------------------------------------
// Tw.Stk
#include "TwStkOrdCenter.h"
// Fix.Stk
#include "FixStkOrdFlow.h"
//---------------------------------------------------------------------------
// 2010.03.08 取消台股委託書存檔的機制,程式啟動時去 TFsSysOrds 掃過一遍並載入
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TVACreater;

/////////////////////////////////////////////////////////////////////////////
// class TBrkOrdID
/////////////////////////////////////////////////////////////////////////////
class TBrkOrdID
{
   typedef Tw::Stk::TBrokerID  TBrokerID;
   typedef Tw::Stk::TTseOrdID  TTseOrdID;

   TBrokerID  BrokerID_;
   TTseOrdID  TseOrdID_;

public:
   inline K_ctor TBrkOrdID () {}
   
   //inline K_ctor TBrkOrdID (const TBrokerID& brkID, const TTseOrdID& tseOrdID)
   //   : BrokerID_(brkID)
   //   , TseOrdID_(tseOrdID)
   //{}
   inline K_ctor TBrkOrdID (const std::string& brkID, const std::string& tseOrdID)
      : BrokerID_(brkID)
      , TseOrdID_(tseOrdID)
   {}

   K_mf(void) assign (const char*);
   
   K_mf(std::string) as_string () const;
   
   K_mf(bool) operator < (const TBrkOrdID& a) const;

   inline K_mf(void) assign          (const std::string& str)  { assign(str.c_str());   }
   inline K_mf(void) assign_BrokerID (const std::string& str)  { BrokerID_.assign(str); }
   inline K_mf(void) assign_TseOrdID (const std::string& str)  { TseOrdID_.assign(str); }
};


/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow
/////////////////////////////////////////////////////////////////////////////
class TTwseFsOrdFlow : public FIX::Stk::TFsOrdFlow
{
   // TFsOrdFlow
   typedef FIX::Stk::TFsOrdFlow    inherited;
   typedef inherited               TFsOrdFlow;
   typedef TFsOrdFlow::TOrdHnd     TFixOrdHnd;
   // FIX
   typedef FIX::Header             FixHeader;
   typedef FIX::Message            FixMessage;
   typedef FIX::TFixStdString1     TFixStdString;
   typedef FIX::TFixPri2           TFixPri;
   typedef FIX::TFixOrdType        TFixOrdType;
   typedef FIX::TFixTargetSubID    TFixTargetSubID;
   typedef FIX::TFixSenderSubID    TFixSenderSubID;
   typedef FIX::TFixAccount        TFixAccount;
   typedef FIX::TFixOrderID        TFixOrderID;
   typedef FIX::TFixSymbol         TFixSymbol;
   typedef FIX::TFixClOrdID        TFixClOrdID;
   typedef FIX::TFixOrigClOrdID    TFixOrigClOrdID;
   typedef FIX::TFixMsgType2       TFixMsgType2;
   // FIX.Stk
   typedef FIX::Stk::EFixOrdCmdID    EFixOrdCmdID;
   typedef FIX::Stk::EOfOrdField     EFixOfOrdField;
   typedef FIX::Stk::EFindOrdResult  EFixFindOrdResult;
   typedef FIX::Stk::SOfOrdField   SFixOfOrdField;
   typedef FIX::Stk::SRfBaseField  SFixRfBaseField;
   typedef FIX::Stk::SRfErField    SFixRfErField;
   typedef FIX::Stk::SRfOcrField   SFixRfOcrField;
   typedef FIX::Stk::TFixSysOrdID  TFixSysOrdID;
   typedef FIX::Stk::TFixRefOrdID  TFixRefOrdID;
   typedef FIX::Stk::TFsStkOrder   TFsStkOrder;
   typedef FIX::Stk::TFsSysOrdRef  TFsSysOrdRef;
   typedef FIX::Stk::TFsSysOrds    TFsSysOrds;
   typedef FIX::Stk::TRptFlow      TFixRptFlow;
   typedef FIX::Stk::SCoid         SFixCoid;
   typedef FIX::Stk::SOrdTrack     SFixOrdTrack;
   //
   typedef FIX::Stk::TOfReqBase      TFixOfReqBase;
   typedef FIX::Stk::TOfReqNosBase   TFixOfReqNosBase;
   typedef FIX::Stk::TOfReqOldBase   TFixOfReqOldBase;
   typedef FIX::Stk::TOfReqOcrBase   TFixOfReqOcrBase;
   typedef FIX::Stk::TOfReqOcrrBase  TFixOfReqOcrrBase;
   typedef FIX::Stk::TOfReqOsrBase   TFixOfReqOsrBase;
   typedef FIX::Stk::TOfReqAnoBase   TFixOfReqAnoBase;
   typedef FIX::Stk::TOfReqAno       TFixOfReqAno;
   //
   typedef FIX::Stk::TRfReqReject    TFixRfReqReject;
   // Tw.Stk
   typedef Tw::Stk::TSysOrdID      TSysOrdID;
   typedef Tw::Stk::TTrxSegment    TTrxSegment;
   typedef Tw::Stk::TMarket        TMarket;
   typedef Tw::Stk::TStkNo         TStkNo;
   typedef Tw::Stk::TBSCode        TBSCode;
   typedef Tw::Stk::TStkPri        TStkPri;
   typedef Tw::Stk::TIvacNo        TIvacNo;
   typedef Tw::Stk::TTseOrdKey     TTseOrdKey;
   typedef Tw::Stk::TOrdCenter     TOrdCenter;

   enum EFieldLen
   {
      efl_Account = 7,
      efl_OrderID = 5,
      
      efl_BrokerID    = 4,

      efl_ClOrdID_Max = 12,
      efl_ClOrdID_Min =  1,

      efl_OrigClOrdID_Max = 12,
      efl_OrigClOrdID_Min =  1,

      efl_Symbol_Min  = 4,
      efl_Symbol_Max  = 6,
   };

private:
   class TRsOrdID;
   
   // 委託內容比對
   class TOrdFdChk;
   
   TOrdCenter*  TwOrdCenter_;
   TRsOrdID*    RsOrdID_;

public:
   K_ctor  TTwseFsOrdFlow (FIX::Stk::TTradeInfo*);
   K_dtor ~TTwseFsOrdFlow ();

protected:
   //======================================================
   // overwrite FIX::Stk::TOrdFlow virtual function
   K_mf(void)              VirInsNewOrd  (const TFixSysOrdID&,   const TFsStkOrder&);
   K_mf(EFixFindOrdResult) VirFindOldOrd (TFixOfReqOldBase&, TFixSysOrdID&);

   // 下刪改查作業前會先準備一些基本欄位
   K_mf(void) VirOnGetOrdFiled (const FixMessage&, SFixOfOrdField&);

   // 產生新委託時, 需要填入 StkOrd 資料的事件
   K_mf(void) VirInitNewOrdField (const FixMessage&, const SFixOfOrdField&, TFsStkOrder&);

   // NewOrderSingle 的事件
   K_mf(bool) OnProcReqNos (TFixOfReqNosBase*);
   
   //====================================
   // OrderCancelRequest 的事件
   K_mf(bool) VirProcOcr (TFixOfReqOcrBase*);

   inline K_mf(void) VirBfProcOcr (TFixOfReqOcrBase* refOcr)
      { InitOldOrdField(refOcr); }
 
   //====================================
   // OrderCancelReplaceRequest 的事件
   K_mf(bool) VirProcOcrr (TFixOfReqOcrrBase*);
   
   inline K_mf(void) VirBfProcOcrr (TFixOfReqOcrrBase* refOcrr)
      { InitOldOrdField(refOcrr); }

   //====================================
   // OrderStatusRequest 的事件
   K_mf(bool) VirProcOsr (TFixOfReqOsrBase*);

   inline K_mf(void) VirBfProcOsr (TFixOfReqOsrBase* refOsr)
      { InitOldOrdField(refOsr); }

   //====================================
   // 當委託找不到時的處理機制
   K_mf(bool) VirProcOrderNotFound (TFixOfReqOldBase*);

	//======================================================
	// TTwseFsOrdFlow function
   
   // 載入台股委託書號資訊
   K_mf(void) Load_RsOrdID ();

private:
   // 檢查 - 盤別
   K_mf(bool) CheckApCode (const TFixTargetSubID&);
   K_mf(bool) CheckApCode (const TFixSysOrdID&, EFixOrdCmdID, const TFixTargetSubID&, TFixRfReqReject&);
   
   // 檢查 - 券商代號長度
   K_mf(bool) CheckBrokerID (const TFixSenderSubID&);
   K_mf(bool) CheckBrokerID (const TFixSysOrdID&, EFixOrdCmdID, const TFixSenderSubID&, TFixRfReqReject&);
   
   K_mf(bool) CheckClOrdID     (const TFixSysOrdID&, EFixOrdCmdID, const TFixClOrdID&,     TFixRfReqReject&);
   K_mf(bool) CheckOrigClOrdID (const TFixSysOrdID&, EFixOrdCmdID, const TFixOrigClOrdID&, TFixRfReqReject&);
   
   // 檢查 - 40.OrdType
   K_mf(bool) CheckOrdType (const TFixSysOrdID&, EFixOrdCmdID, const TFixOrdType&, const TTrxSegment&, TFixRfReqReject&);

   K_mf(bool) CheckAccount  (const TFixSysOrdID&, EFixOrdCmdID,                         const TFixAccount&,   TFixRfReqReject&);
   K_mf(bool) CheckOrderID  (const TFixSysOrdID&, EFixOrdCmdID,                         const TFixOrderID&,   TFixRfReqReject&);
   K_mf(bool) CheckSymbol   (const TFixSysOrdID&, EFixOrdCmdID,                         const TFixSymbol&,    TFixRfReqReject&);
   K_mf(bool) CheckPrice    (const TFixSysOrdID&, EFixOrdCmdID, TFixOrdType::orig_type, const TFixStdString&, TFixRfReqReject&);
   K_mf(bool) CheckQty      (const TFixSysOrdID&, EFixOrdCmdID,                         const TFixStdString&, TFixRfReqReject&);

   // 檢查 - TwseWarrantLP是否合法
   K_mf(bool) CheckWarrantLP (TFixOfReqBase*, const TFixTargetSubID&, TFixRfReqReject&);

   // [刪/改/查] 需要填入 FixStkOrd 的資料
   K_mf(void) InitOldOrdField (TFixOfReqBase*);
};

/////////////////////////////////////////////////////////////////////////////
} // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFsOrdFlow_h_)
