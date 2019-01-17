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
      // �o�{���Ъ�[���ӥN��+�e�U�Ѹ�]
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
         { "�e�U�Ѹ�", 12, ta_xLeft_yCenter, ta_xLeft_yCenter, tfs_Fixed, },
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
         { "Fix�t�ΧǸ�", 12, ta_xLeft_yCenter, ta_xLeft_yCenter, tfs_Fixed, },
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

      VaMgr_.Add(std::string("1.���"), *RsVacFull_);
      VaMgr_.Add(std::string("2.�s��"), *RsVacOdd_);
      VaMgr_.Add(std::string("3.�L��"), *RsVacFixed_);
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

   // �o���٨S�P�_�O fcid_ChgQty, fcid_ChgPri, fcid_ChgPriQty
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
   // ���q���
   if (TrxSeg_ == Tw::Stk::ts_Full) {
      if (IsChg_) {
         // ��q: ���ˬd[���H�b��]
         // ==> ���O����ҭn�D��������ǰe FixCli �����,�ҥH�令[�n�ˬd]
         //Conf_.ChkAccount_ = false;
      }
   }
   //==============================================
   // �L��w��
   else if (TrxSeg_ == Tw::Stk::ts_Fix) {
      // �����ˬd[����]
      Conf_.ChkPrice_ = false;
      
      if (IsChg_) {
         // ��q: �ˬd[�Ҩ�N��],[���H�b��],[�ƶq],[����O],[���H�U�����O���O]
      }
   }
   //==============================================
   // �s��
   else if (TrxSeg_ == ts_Odd) {
      if (IsDel_) {
         // �R��: �O�ݩe�U�ѽs��, �]���O�㵧�R��, �ҥH���ˮֻ���, �ҥH Tandem �^�з|�O���\��
         //       ���ӥi�H�����ˮ֩e�U���e
         // ==> ���O����ҭn�D��������ǰe FixCli �����,�ҥH�令[�n�ˬd]
         //return true;
      }
      else if (IsChg_) {
         // ��q
         // ==> �ˬd[�Ѳ��N��],[�R��O],[���H�b��],[�ƶq],[����]
      }
   }

   return ChkField();
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::TOrdFdChk::ChkField ()
{
   //==============================================
   // ��� - 55.Symbol
   if (FixOrd_.Symbol_ != FixOrdField_.Symbol_) {
      sprintf(ErrField_, "Symbol[%s][%s]", FixOrd_     .Symbol_.as_string().c_str(),
                                           FixOrdField_.Symbol_.as_string().c_str());
      return false;
   }
   //==============================================
   // ��� - 54.Side
   if (FixOrd_.Side_ != FixOrdField_.Side_) {
      sprintf(ErrField_, "Side[%s][%s]", FixOrd_     .Side_.as_string().c_str(),
                                         FixOrdField_.Side_.as_string().c_str());
      return false;
   }
   //==============================================
   // ��� - 10001.TwseOrdType
   const STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<const STwseFsOrdReseChar*>(FixOrd_.GetReseChar());
   if (twseOrdRChar->TwseOrdType_ != FixOrdField_.TwseOrdType_) {
      sprintf(ErrField_, "TwseOrdType[%s][%s]", twseOrdRChar->TwseOrdType_.as_string().c_str(),
                                                FixOrdField_ .TwseOrdType_.as_string().c_str());
      return false;
   }

   //===========================================
   // ��� - Price
   if (Conf_.ChkPrice_) {
      TFixPri pri(FixOrdField_.Price_.as_string());

      if (FixOrd_.Price_ != pri) {
         sprintf(ErrField_, "Price[%s][%s]", FixOrd_.Price_.as_string().c_str(),
                                             pri           .as_string().c_str());
         return false;
      }
   }
   
   //===========================================
   // ��� - Account
   // ==> �b�r���x�s�̭�, ���i��O 0000094 or 94, �o�ˤ��|�����D
   //     �ҥH�j�a���ন TIvacNo ���ӴN�S���D�F
   // ==> [�R��][�d��]�èS���� Tag, �ҥH�ݧP�_�O�_ empty
   // ==> FixOrd.Account_ �C���b Tandem �^���ɳ��|�Q����, �ҥH�ϥ� FixOrd.OrigAccount_ �ӧP�_
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
   sprintf(ErrDesc_, "fsid[%d], �e�U�Ѹ�[%s / %s], ���[%s]",
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
   
   // �o���٨S�P�_�O fcid_ChgQty, fcid_ChgPri, fcid_ChgPriQty
   if (foid == FIX::Stk::fcid_ChgQty ||
       foid == FIX::Stk::fcid_ChgPri ||
       foid == FIX::Stk::fcid_ChgPriQty)
   {
      isChg = true;
   }

   errField[0] = '\0';

   // ���
   if (trxSeg == ts_Full) {
      if (isChg) {
         // ��q
         // ���n��쬰�Ҩ�ӥN���B�e�U�ѽs���B�Ҩ�N���B�ƶq�B����B����O�B���H�U�����O���O
         // ==> ���ˮ�[���H�b��]
         chkAccount = false;
      }
   }
   // �L��w��
   else if (trxSeg == ts_Fix) {
      chkPrice = false;
      
      if (isChg) {
         // ��q
         // ���n��쬰�Ҩ�ӥN���B�e�U�ѽs���B�Ҩ�N���B���H�b���B�ƶq�B����O�B���H�U�����O���O
         // ==> ���ˮ�[����]
         chkAccount = true;
      }
   }
   // �s��
   else if (trxSeg == ts_Odd) {
      if (foid == FIX::Stk::fcid_Del) {
         // �R��
         // ==> �s�ѧR��O�ݩe�U�ѽs��, �]���O�㵧�R��, �ҥH���ˮֻ���, �ҥH Tandem �^�з|�O���\��
         // ==> ���ӥi�H�����ˮ֩e�U���e
         return true;
      }
      else if (isChg) {
         // ��q�|�H BRKNO+�e�U�ѽs���hŪ����l���, �|��Ѳ��N��, �R��O, ���H�b��, �ƶq, ����@�ˮ�
         chkAccount = true;
      }
   }

   do {
      //===========================================
      // ��� - Symbol
      if (fixStkOrd->Symbol_ != fixOrdFiled.Symbol_) {
         sprintf(errField, "Symbol[%s][%s]", fixOrdFiled.Symbol_.as_string().c_str(),
                                             fixStkOrd->Symbol_ .as_string().c_str());
         break;
      }
      //===========================================
      // ��� - Side
      if (fixStkOrd->Side_ != fixOrdFiled.Side_) {
         sprintf(errField, "Side[%s][%s]", fixOrdFiled.Side_.as_string().c_str(),
                                           fixStkOrd->Side_ .as_string().c_str());
         break;
      }
      //===========================================
      // ��� - Price
      if (chkPrice) {
         TFixPri pri(fixOrdFiled.Price_.as_string());

         if (fixStkOrd->Price_ != pri) {
            sprintf(errField, "Price[%s][%s]", pri              .as_string().c_str(),
                                               fixStkOrd->Price_.as_string().c_str());
            break;
         }
      }
      //===========================================
      // ��� - Account
      // �b�r���x�s�̭�, ���i��O 0000094 or 94, �o�ˤ��|�����D
      // �ҥH�j�a���ন TIvacNo ���ӴN�S���D�F
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
   // �T������
   if (errField[0] == '\0')
      return true;

   return false;
}
*/
