//
// TwStk 的 [委託流程]
//
// 版本: $Id: TwStkOrdFlow.h,v 1.5 2006/04/04 03:32:46 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkOrdFlowH
#define TwStkOrdFlowH
/////////////////////////////////////////////////////////////////////////////
#include "TwStkOrdCenter.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
//---------------------------------------------------------------------------
TWSTK_class  TTrProfile; // TwStkTrProfile.h
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TOrdFlow : public TKeyObject {
public:
   explicit K_ctor  TOrdFlow  (const std::string& logPath, TOrdCenter&);

   //Types.
   typedef Kway::Tw::Stk::TOrdCmdID    TOrdCmdID;  // 委託要求代號(0=新委託,1=改量...)
   typedef TOrdCenter::TOrdHnd         TOrdHnd;
   typedef Kway::Tw::Stk::TTrProfile   TTrProfile;

   //委託要求物件--------------基底 TReq-----------------------
   TWSTK_class TReqBase {
   public:
#ifdef _MSC_VER
      inline K_ctor TReqBase() {}
#endif
      inline  K_mf(bool)        IsFinish     () const  { return FlowID_.IsAllFinish(); }
      inline  K_mf(TOrdFlow*)   GetOrdFlow   () const  { return OrdFlow_;    }
      inline  K_mf(TOrdFlowID)  GetOrdFlowID () const  { return FlowID_;     }
      inline  K_mf(TOrdCmdID)   GetOrdCmdID  () const  { return OCmd_;       }
      inline  K_mf(TTrProfile&) GetTrProfile () const  { return *TrProfile_; }
      inline  K_mf(TOrdHnd&)    GetOrdHnd    ()        { return OrdHnd_;     }
      inline  K_mf(std::string) GetLastMsg   () const  { return LastMsg_;    }
      inline  K_mf(bool)        IsJamStk     () const  { return IsJamStk_;   }
      inline  K_mf(void)        SetIsJamStk  (bool v)  { IsJamStk_ = v;      }

      //當在 TTrProfile 解構時,必須重設正在執行的Reqs->TrProfile_
      virtual K_mf(void) ClrTrProfile ();

      //當某個委託流程步處理告一段落(不一定是該Step結束), 呼叫Update()更新訊息.
      virtual K_mf(void) Update (const std::string&, EOrdFlowID);
   protected:
              K_ctor  TReqBase  (TOrdCmdID, TTrProfile*, TSysOrdID);
      virtual K_dtor ~TReqBase  ();

      TOrdCmdID     OCmd_;
      TSysOrdID     SysOrdID_;
      std::string   LastMsg_;
      TOrdFlowID    FlowID_;
      bool          Updating_;
      TTrProfile*   TrProfile_;

      friend class TOrdFlow;  //當開始進入委託流程時,TOrdFlow必須設定相關的欄位
      TOrdHnd       OrdHnd_;
      TOrdFlow*     OrdFlow_;
      bool          IsJamStk_;   //塞單股票, 預設為false, 交給 TFlowStep 設定及處理.
   };
   typedef wise_ptr<TReqBase>    TReqPtr;
   typedef pod_vector<TReqPtr>   TReqPtrs;
   typedef TReqPtr::ref_obj      TReqRefBase;
   //委託要求物件--------------新委託(含強迫) TReqNew --------------------
   // TReqXXX 必須使用 new 的方式建構, 不可使用[區域變數]的方式產生!
   // 所以請使用 CreateReqXXX(...) 的 member function 來取得委託要求物件!
   TWSTK_class TReqNewBase : public TReqRefBase {
      typedef TReqRefBase   inherited;
   protected:
      K_ctor  TReqNewBase  (TOrdCmdID, TTrProfile&, TSysOrdID);
      K_ctor  TReqNewBase  (TTrProfile&, TSysOrdID);//=0新增,else強迫
      friend class TOrdFlow;
   public:
      //data members
      TOrdFields_ForNew    Ord_;
      TScForces            ScForces_; //風控管制強迫旗標(全新的委託也可以先填入某些強迫旗標,表示不必檢查某些項目)
      dword                ScCond_;   //2006.02.10.風控條件,由風控查驗時自行定義其意義,預設為0.
   };
   typedef wise_ref<TReqNewBase> TReqNew;
   static inline K_mf(TReqNew)   CreateReqNew  (TTrProfile& tpf, TSysOrdID id=0)
                                                   { return TReqNew(new TReqNewBase(tpf,id)); }
   //委託要求物件--------------舊委託的處理 TReqOld --------------------
   TWSTK_class TReqOld : public TReqRefBase {
      typedef TReqRefBase   inherited;
   protected:
      K_ctor  TReqOld  (TOrdCmdID, TTrProfile&, TSysOrdID);
   public:
      //data members
      TOrdSource  OrdSource_;
   };
   //委託要求物件--------------查詢 TReqQry --------------------
   TWSTK_class TReqQryBase : public TReqOld {
      typedef TReqOld   inherited;
   protected:
      K_ctor  TReqQryBase  (TTrProfile&, TSysOrdID);
      friend class TOrdFlow;
   };
   typedef wise_ref<TReqQryBase> TReqQry;
   static inline K_mf(TReqQry)   CreateReqQry  (TTrProfile& tpf, TSysOrdID id)
                                                   { return TReqQry(new TReqQryBase(tpf,id)); }
   //委託要求物件--------------查詢 TReqChgQty --------------------
   // cooper {
   //    交易所這邊必須要保留原始狀態,如果改量 ReqQty 為 0 時,還是改量
   // } cooper
   TWSTK_class TReqChgQtyBase : public TReqOld {
      typedef TReqOld   inherited;
      TStkQty ReqQty_;  // >0改後數量, <0刪減數量, ==0刪單
   protected:
      K_ctor  TReqChgQtyBase  (TTrProfile&, TSysOrdID, TStkQty);
      K_ctor  TReqChgQtyBase  (TTrProfile&, TSysOrdID); // cooper
      friend class TOrdFlow;
   public:
      inline K_mf(TStkQty)  GetReqQty   () const   { return ReqQty_; }
   };
   typedef wise_ref<TReqChgQtyBase> TReqChgQty;
   static inline K_mf(TReqChgQty)  CreateReqChgQty  (TTrProfile& tpf, TSysOrdID id, TStkQty qty)
                                                       { return TReqChgQty(new TReqChgQtyBase(tpf,id,qty)); }
   //委託要求物件--------------刪單 TReqDel --------------------
   TWSTK_class TReqDelBase : public TReqChgQtyBase {
      typedef TReqChgQtyBase   inherited;
   protected:
      K_ctor  TReqDelBase  (TTrProfile&, TSysOrdID);
      friend class TOrdFlow;
   };
   typedef wise_ref<TReqDelBase> TReqDel;
   static inline K_mf(TReqDel)  CreateReqDel  (TTrProfile& tpf, TSysOrdID id)
                                                   { return TReqDel(new TReqDelBase(tpf,id)); }
   //委託要求物件--------------改價 TReqChgPri --------------------
   TWSTK_class TReqChgPriBase : public TReqDelBase {
      typedef TReqDelBase  inherited;
      TStkPri  ReqPri_; //改後價格.
      TReqNew  ReqNew_; //刪單成功後將要送出的新委託.

   protected:
      friend class TOrdFlow;
      K_ctor     TReqChgPriBase (TTrProfile&, TSysOrdID, TStkPri);
      K_mf(bool) SetReqNew      (TOrdFlow*);//OrdFlow在DoReq()成功後會先呼叫此函式.
      K_mf(void) ClrTrProfile   ();
   public:
      inline K_mf(TReqNew) GetReqNew () const   { return ReqNew_; }
      inline K_mf(TStkPri) GetReqPri () const   { return ReqPri_; }
      virtual K_mf(void)   Update    (const std::string&, EOrdFlowID);
   };
   typedef wise_ref<TReqChgPriBase> TReqChgPri;
   static inline K_mf(TReqChgPri)  CreateReqChgPri  (TTrProfile& tpf, TSysOrdID id, TStkPri pri)
                                                   { return TReqChgPri(new TReqChgPriBase(tpf,id,pri)); }
   //委託要求物件--------------[*試算] TReqTriMul --------------------
   //TWSTK_class TReqTriMul : public TReqNew {
   //};
   //委託要求物件--------------[/試算] TReqTriDiv --------------------
   //TWSTK_class TReqTriDiv : public TReqNew {
   //};
   //================ 委託流程註冊 =============
   TWSTK_class TFlowStep {
      virtual K_mf(bool)  OnOrdReq  (TReqPtr) = 0;//傳回false時,表示中斷委託流程!
      friend class TOrdFlow;
   };

   // cooper { key 從 0 開始
   K_mf(bool) AddFlowStep (TOrdCmdID cid, TFlowStep& fs, size_t key);
   // } cooper

   inline K_mf(bool)  AddFlowStep  (TOrdCmdID cid, TFlowStep& fs)  { return FlowSteps_[cid].add(&fs); }
   inline K_mf(bool)  DelFlowStep  (TOrdCmdID cid, TFlowStep& fs)  { return FlowSteps_[cid].del(&fs); }
          K_mf(int)   DelFlowStep  (TFlowStep&);

   //================ 執行委託要求 =============
   K_mf(bool)  DoReq  (TReqNew);
   K_mf(bool)  DoReq  (TReqQry);
   K_mf(bool)  DoReq  (TReqDel);
   K_mf(bool)  DoReq  (TReqChgQty);
   K_mf(bool)  DoReq  (TReqChgPri);
   //K_mf(bool)  DoReq  (TReqTriMul);
   //K_mf(bool)  DoReq  (TReqTriDiv);

   inline K_mf(TOrdCenter&) GetOrdCenter() const   { return *OrdCenter_; }
private:
   friend class TReqBase;
   K_mf(bool)  DoFlowSteps  (TReqBase&, size_t stepn);
   K_mf(bool)  DoReqOld     (TReqOld&);
   K_mf(bool)  DoReqNew     (TReqNew, bool doFlow);

   typedef Kstl::pod_vector<TFlowStep*>   TFlowSteps;
   typedef TFlowSteps::iterator           TFlowStepi;
   TFlowSteps     FlowSteps_[cid_End];
   TOrdCenter*    OrdCenter_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

