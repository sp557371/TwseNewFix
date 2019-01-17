/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFsOrdFlowAux.h"
#include "tplns/Nsa_EvMap_2k_VA.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TRsOrdID impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFsOrdFlow::TRsOrdID::TRsOrdID (const char* logPath)
   : RsVacFull_  (NULL)
   , RsVacOdd_   (NULL)
   , RsVacFixed_ (NULL)
{
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsOrdFlow::TRsOrdID::Insert (const TTrxSegment& trxSegment, const TBrkOrdID& brkOrdID, const TFixSysOrdID& fsid)
{
   TRsBrkOrdID* rsBrkOrdID = NULL;
   
   switch (trxSegment) {
      case Tw::Stk::ts_Full:  rsBrkOrdID = &RsFull_;   break;
      case Tw::Stk::ts_Fix:   rsBrkOrdID = &RsFixed_;  break;
      case Tw::Stk::ts_Odd:   rsBrkOrdID = &RsOdd_;    break;
   }

   if (!rsBrkOrdID)
      return;
      
   TRsBrkOrdID::mapped_type                dat1;
   TRsFixSoid ::mapped_type                dat2;
   std::pair<TRsBrkOrdID::iterator, bool>  ins1;
   TRsFixSoid*                             rsFixSoid;

   ins1      = rsBrkOrdID->insert(TRsBrkOrdID::value_type(brkOrdID, dat1));
   rsFixSoid = ins1.first->second.GetContainer(rsFixSoid);

   rsFixSoid->insert(TRsFixSoid::value_type(fsid, dat2));
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::TRsOrdID::CheckInsert (const TTrxSegment&  trxSeg, const TBrkOrdID& brkOrdId,
                                                  const TFixSysOrdID& fsid)
{
   TRsBrkOrdID* rsBrkOrdID = NULL;
   
   switch (trxSeg) {
      case Tw::Stk::ts_Full:  rsBrkOrdID = &RsFull_;   break;
      case Tw::Stk::ts_Fix:   rsBrkOrdID = &RsFixed_;  break;
      case Tw::Stk::ts_Odd:   rsBrkOrdID = &RsOdd_;    break;
   }

   if (!rsBrkOrdID)
      return false;
      
   TRsBrkOrdID::mapped_type               dat1;
   std::pair<TRsBrkOrdID::iterator, bool> ins1;
   TRsFixSoid*                            rsFixSoid;

   ins1      = rsBrkOrdID->insert(TRsBrkOrdID::value_type(brkOrdId, dat1));
   rsFixSoid = ins1.first->second.GetContainer(rsFixSoid);

   if (!rsFixSoid->empty()) {
      // 發現重覆的[卷商代號+委託書號]
      return false;
   }

   TRsFixSoid::mapped_type dat2;

   rsFixSoid->insert(TRsFixSoid::value_type(fsid, dat2));
   return true;
}
//---------------------------------------------------------------------------

K_mf(TTwseFsOrdFlow::TRsOrdID::TRsFixSoid*) TTwseFsOrdFlow::TRsOrdID::FindRsFixSoid (const TTrxSegment& trxSegment,
                                                                                     const TBrkOrdID&   brkOrdID)
{
   TRsBrkOrdID* rsBrkOrdID = NULL;
   
   switch (trxSegment) {
      case Tw::Stk::ts_Full:  rsBrkOrdID = &RsFull_;   break;
      case Tw::Stk::ts_Fix:   rsBrkOrdID = &RsFixed_;  break;
      case Tw::Stk::ts_Odd:   rsBrkOrdID = &RsOdd_;    break;
   }

   if (!rsBrkOrdID)
      return NULL;

   TRsBrkOrdID::iterator iter1 = rsBrkOrdID->find(brkOrdID);
   if (iter1 == rsBrkOrdID->end())
      return NULL;

   TRsFixSoid* rsFixSoid = iter1->second.GetContainer(rsFixSoid);
   return rsFixSoid;
}

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TRsOrdID::TFieldMaker1
/////////////////////////////////////////////////////////////////////////////
class TTwseFsOrdFlow::TRsOrdID::TFieldMaker1
{
   typedef TRsBrkOrdID          container;
   typedef container::iterator  iterator;
   enum {
      IField_BrkOrdID,
      IField_Total
   };

public:
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "委託書號", 12, ta_xLeft_yCenter, ta_xLeft_yCenter, tfs_Fixed, },
      };
      return FdDsp[n];
   }

   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
      case IField_BrkOrdID:  return i->first.as_string();
      }
      return std::string();
   }

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container&, iterator&, size_t, const std::string&) {}
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TRsOrdID::TFieldMaker2
/////////////////////////////////////////////////////////////////////////////
class TTwseFsOrdFlow::TRsOrdID::TFieldMaker2
{
   typedef TRsFixSoid           container;
   typedef container::iterator  iterator;
   enum {
      IField_FixSysOrdID,
      IField_Total
   };

public:
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "Fix系統序號", 12, ta_xLeft_yCenter, ta_xLeft_yCenter, tfs_Fixed, },
      };
      return FdDsp[n];
   }

   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
      case IField_FixSysOrdID:  return i->first.as_string();
      }
      return std::string();
   }

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container&, iterator&, size_t, const std::string&) {}
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TRsOrdID::TRsVac impl
/////////////////////////////////////////////////////////////////////////////
class TTwseFsOrdFlow::TRsOrdID::TRsVac: public TEvMap2kVA<TRsBrkOrdID, TFieldMaker1, TFieldMaker2>
{
   typedef TEvMap2kVA<TRsBrkOrdID, TFieldMaker1, TFieldMaker2>  inherited;
      
public:
   inline K_ctor TRsVac (TRsBrkOrdID* rs)
      : inherited(rs)
   {}
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TRsOrdID impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TTwseFsOrdFlow::TRsOrdID::~TRsOrdID ()
{
   if (RsVacOdd_) {
      VaMgr_.Del(*RsVacOdd_);
      delete RsVacOdd_;
   }
   if (RsVacFixed_) {
      VaMgr_.Del(*RsVacFixed_);
      delete RsVacFixed_;
   }
   if (RsVacFull_) {
      VaMgr_.Del(*RsVacFull_);
      delete RsVacFull_;
   }
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TTwseFsOrdFlow::TRsOrdID::GetVaMgr ()
{
   if (!RsVacFull_) {
      RsVacFull_  = new TRsVac(&RsFull_);
      RsVacOdd_   = new TRsVac(&RsOdd_);
      RsVacFixed_ = new TRsVac(&RsFixed_);

      VaMgr_.Add(std::string("1.整股"), *RsVacFull_);
      VaMgr_.Add(std::string("2.零股"), *RsVacOdd_);
      VaMgr_.Add(std::string("3.盤後"), *RsVacFixed_);
   }
      
   return VaMgr_;
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TOrdFdChk impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFsOrdFlow::TOrdFdChk::TOrdFdChk (const TTrxSegment& trxSeg, EFixOrdCmdID          foid,
                                             const TFsStkOrder& fixOrd, const SFixOfOrdField& fixOrdField)
   : TrxSeg_     ((ETrxSegment)trxSeg)
   , FixOrd_     (fixOrd)
   , FixOrdField_(fixOrdField)
   , IsChg_      (false)
   , IsDel_      (false)
   , IsQry_      (false)
{
   ErrField_[0] = '\0';
   ErrDesc_ [0] = '\0';

   // 這邊還沒判斷是 fcid_ChgQty, fcid_ChgPri, fcid_ChgPriQty
   switch (foid) {
      case FIX::Stk::fcid_ChgQty:
      case FIX::Stk::fcid_ChgPri:
      case FIX::Stk::fcid_ChgPriQty:  IsChg_ = true;  break;
      case FIX::Stk::fcid_Del:        IsDel_ = true;  break;
      case FIX::Stk::fcid_Qry:        IsQry_ = true;  break;
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::TOrdFdChk::DoChk ()
{
   //==============================================
   // 普通交易
   if (TrxSeg_ == Tw::Stk::ts_Full) {
      if (IsChg_) {
         // 減量: 不檢查[投資人帳號]
         // ==> 但是交易所要求必須完整傳送 FixCli 的資料,所以改成[要檢查]
         //Conf_.ChkAccount_ = false;
      }
   }
   //==============================================
   // 盤後定價
   else if (TrxSeg_ == Tw::Stk::ts_Fix) {
      // 都不檢查[價格]
      Conf_.ChkPrice_ = false;
      
      if (IsChg_) {
         // 減量: 檢查[證券代號],[投資人帳號],[數量],[交易別],[投資人下單類別註記]
      }
   }
   //==============================================
   // 零股
   else if (TrxSeg_ == ts_Odd) {
      if (IsDel_) {
         // 刪單: 是看委託書編號, 因為是整筆刪除, 所以不檢核價格, 所以 Tandem 回覆會是成功的
         //       應該可以不用檢核委託內容
         // ==> 但是交易所要求必須完整傳送 FixCli 的資料,所以改成[要檢查]
         //return true;
      }
      else if (IsChg_) {
         // 改量
         // ==> 檢查[股票代號],[買賣別],[投資人帳號],[數量],[價格]
      }
   }

   return ChkField();
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::TOrdFdChk::ChkField ()
{
   //==============================================
   // 比對 - 55.Symbol
   if (FixOrd_.Symbol_ != FixOrdField_.Symbol_) {
      sprintf(ErrField_, "Symbol[%s][%s]", FixOrd_     .Symbol_.as_string().c_str(),
                                           FixOrdField_.Symbol_.as_string().c_str());
      return false;
   }
   //==============================================
   // 比對 - 54.Side
   if (FixOrd_.Side_ != FixOrdField_.Side_) {
      sprintf(ErrField_, "Side[%s][%s]", FixOrd_     .Side_.as_string().c_str(),
                                         FixOrdField_.Side_.as_string().c_str());
      return false;
   }
   //==============================================
   // 比對 - 10001.TwseOrdType
   const STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<const STwseFsOrdReseChar*>(FixOrd_.GetReseChar());
   if (twseOrdRChar->TwseOrdType_ != FixOrdField_.TwseOrdType_) {
      sprintf(ErrField_, "TwseOrdType[%s][%s]", twseOrdRChar->TwseOrdType_.as_string().c_str(),
                                                FixOrdField_ .TwseOrdType_.as_string().c_str());
      return false;
   }

   //===========================================
   // 比對 - Price
   if (Conf_.ChkPrice_) {
      TFixPri pri(FixOrdField_.Price_.as_string());

      if (FixOrd_.Price_ != pri) {
         sprintf(ErrField_, "Price[%s][%s]", FixOrd_.Price_.as_string().c_str(),
                                             pri           .as_string().c_str());
         return false;
      }
   }
   
   //===========================================
   // 比對 - Account
   // ==> 在字串儲存裡面, 有可能是 0000094 or 94, 這樣比對會有問題
   //     所以大家都轉成 TIvacNo 應該就沒問題了
   // ==> [刪單][查詢]並沒有此 Tag, 所以需判斷是否 empty
   // ==> FixOrd.Account_ 每次在 Tandem 回報時都會被異動, 所以使用 FixOrd.OrigAccount_ 來判斷
   if (Conf_.ChkAccount_ && FixOrdField_.Account_.empty() == false) {
      std::string newAcc = FixOrdField_.Account_    .as_string();
      std::string oldAcc = FixOrd_     .OrigAccount_.as_string();
      TIvacNo     newIvacNo(newAcc);
      TIvacNo     oldIvacNo(oldAcc);

      if (newIvacNo != oldIvacNo) {
         sprintf(ErrField_, "Account[%s][%s]", oldAcc.c_str(), newAcc.c_str());
         return false;
      }
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(const char*) TTwseFsOrdFlow::TOrdFdChk::GetErrDesc (const TFixSysOrdID& fsid, const TBrkOrdID& brkOrdId)
{
   sprintf(ErrDesc_, "fsid[%d], 委託書號[%s / %s], 欄位[%s]",
         fsid                .get_orig(),
         TTrxSegment(TrxSeg_).as_string(true).c_str(),
         brkOrdId            .as_string()    .c_str(),
         ErrField_);

   return ErrDesc_;
}
//---------------------------------------------------------------------------








/*
K_mf(bool) TTwseFsOrdFlow::CheckOrdData (const TTrxSegment& trxSeg,    EFixOrdCmdID          foid,
                                         const TFsStkOrder* fixStkOrd, const SFixOfOrdField& fixOrdFiled,
                                         char*              errField)
{
   bool chkPrice   = true;
   bool chkAccount = false;
   bool isChg      = false;
   
   // 這邊還沒判斷是 fcid_ChgQty, fcid_ChgPri, fcid_ChgPriQty
   if (foid == FIX::Stk::fcid_ChgQty ||
       foid == FIX::Stk::fcid_ChgPri ||
       foid == FIX::Stk::fcid_ChgPriQty)
   {
      isChg = true;
   }

   errField[0] = '\0';

   // 整股
   if (trxSeg == ts_Full) {
      if (isChg) {
         // 減量
         // 必要欄位為證券商代號、委託書編號、證券代號、數量、價格、交易別、投資人下單類別註記
         // ==> 不檢核[投資人帳號]
         chkAccount = false;
      }
   }
   // 盤後定價
   else if (trxSeg == ts_Fix) {
      chkPrice = false;
      
      if (isChg) {
         // 減量
         // 必要欄位為證券商代號、委託書編號、證券代號、投資人帳號、數量、交易別、投資人下單類別註記
         // ==> 不檢核[價格]
         chkAccount = true;
      }
   }
   // 零股
   else if (trxSeg == ts_Odd) {
      if (foid == FIX::Stk::fcid_Del) {
         // 刪單
         // ==> 零股刪單是看委託書編號, 因為是整筆刪除, 所以不檢核價格, 所以 Tandem 回覆會是成功的
         // ==> 應該可以不用檢核委託內容
         return true;
      }
      else if (isChg) {
         // 改量會以 BRKNO+委託書編號去讀取原始資料, 會對股票代號, 買賣別, 投資人帳號, 數量, 價格作檢核
         chkAccount = true;
      }
   }

   do {
      //===========================================
      // 比對 - Symbol
      if (fixStkOrd->Symbol_ != fixOrdFiled.Symbol_) {
         sprintf(errField, "Symbol[%s][%s]", fixOrdFiled.Symbol_.as_string().c_str(),
                                             fixStkOrd->Symbol_ .as_string().c_str());
         break;
      }
      //===========================================
      // 比對 - Side
      if (fixStkOrd->Side_ != fixOrdFiled.Side_) {
         sprintf(errField, "Side[%s][%s]", fixOrdFiled.Side_.as_string().c_str(),
                                           fixStkOrd->Side_ .as_string().c_str());
         break;
      }
      //===========================================
      // 比對 - Price
      if (chkPrice) {
         TFixPri pri(fixOrdFiled.Price_.as_string());

         if (fixStkOrd->Price_ != pri) {
            sprintf(errField, "Price[%s][%s]", pri              .as_string().c_str(),
                                               fixStkOrd->Price_.as_string().c_str());
            break;
         }
      }
      //===========================================
      // 比對 - Account
      // 在字串儲存裡面, 有可能是 0000094 or 94, 這樣比對會有問題
      // 所以大家都轉成 TIvacNo 應該就沒問題了
      if (chkAccount && fixOrdFiled.Account_.empty() == false) {
         std::string newAcc = fixOrdFiled.Account_.as_string();
         std::string oldAcc = fixStkOrd->Account_ .as_string();
         TIvacNo     newIvacNo(newAcc);
         TIvacNo     oldIvacNo(oldAcc);

         if (newIvacNo != oldIvacNo) {
            sprintf(errField, "Account[%s][%s]", newAcc.c_str(), oldAcc.c_str());
            break;
         }
      }

   } while (false);

   //==============================================
   // 訊息揭示
   if (errField[0] == '\0')
      return true;

   return false;
}
*/
