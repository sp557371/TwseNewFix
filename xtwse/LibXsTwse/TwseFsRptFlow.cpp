/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFsRptFlow.h"
#include "TwseFsOrd.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsRptFlow impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFsRptFlow::TTwseFsRptFlow (FIX::Stk::TTradeInfo* fti)
   : inherited(fti)
{
   //==============================================
   // ���w SpecRule
   const EFixRfSpecRule specRules[] = {
      FIX::Stk::rsr_ErDeal_OrderQty_Zero,
      FIX::Stk::rsr_ErDeal_LeavesQty_Zero,
   };

   for (int i=0; i<numofele(specRules); ++i)
      inherited::SpecRule_.set(specRules[i]);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsRptFlow::VirBfOnUpdate_Er (TFixRfReqErBase& fixRefEr)
{
         TFixStkOrder&    fixStkOrd = fixRefEr.GetOrdHnd().GetModify();
   const SFixRfBaseField& fixBaseFd = fixRefEr.GetBaseField();

   // �ˬd Tw.Stk.IvacNo �O�_�M FIX.Stk.Account �@�P
   // �b�r���x�s�̭�, ���i��O 0000094 or 94, �o�ˤ��|�����D
   if (fixBaseFd.Account_.is_null() == false) {
      TIvacNo newIvacNo(fixBaseFd.Account_.as_string());
      TIvacNo oldIvacNo(fixStkOrd.Account_.as_string());
   
      if (newIvacNo != oldIvacNo)
         fixStkOrd.Account_ = fixBaseFd.Account_;
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsRptFlow::VirBfOnUpdate_Ocr (TFixRfReqOcrBase& fixRefOcr)
{
         TFixStkOrder&    fixStkOrd = fixRefOcr.GetOrdHnd().GetModify();
   const SFixRfBaseField& fixBaseFd = fixRefOcr.GetBaseField();

   // �ˬd Tw.Stk.IvacNo �O�_�M FIX.Stk.Account �@�P
   // �b�r���x�s�̭�, ���i��O 0000094 or 94, �o�ˤ��|�����D
   if (fixBaseFd.Account_.is_null() == false) {
      TIvacNo newIvacNo(fixBaseFd.Account_.as_string());
      TIvacNo oldIvacNo(fixStkOrd.Account_.as_string());
   
      if (newIvacNo != oldIvacNo)
         fixStkOrd.Account_ = fixBaseFd.Account_;
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsRptFlow::VirErOrd_FindOrdTrack (TFixRfReqErOrdBase&     refErOrd,
                                                  TFixOrdTrack::TRecsHnd& rshOt, const TFixMsgType2& fixMsgType)
{
// ���� 10003 ���ǰe
//   // �����o�o���^���O���O TwseWarrantLP = Y
//   FixMessage&         fixMsg          = refErOrd.GetFixMsg   ();
//   SFixRfBaseField&    fixRfBaseFd     = refErOrd.GetBaseField();
//   STwseFsOrdReseChar* twseRChar       = reinterpret_cast<STwseFsOrdReseChar*>(fixRfBaseFd.GetReseChar());
//   bool                isTwseWarrantLP = (twseRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use);

   // �����ʨ�Ĥ@��,�̧ǩ��U�M��,�M��|���^��������
   if (rshOt.FindMap2() == NULL || rshOt.MoveFirst2() == false)
      return false;
   
   do {
      const SFixOrdTrack& cdOt = rshOt.GetDat2(false);

      if (cdOt.MsgType_ == fixMsgType && cdOt.FlowID_.IsFinish() == false) {
//         const STwseOrdTrackReseChar* twseOtRChar = reinterpret_cast<const STwseOrdTrackReseChar*>(cdOt.GetRChar());
//               char                   ch          = twseOtRChar->TwseWarrantLP_.get_orig();
//
//         // �����^���O TwseWarrantLP
//         if (isTwseWarrantLP) {
//            if (ch == TwseWarrantLP_Use) {
//               // Client�ݩe�U�ɪ��q�妳 10003=Y, �ҥH�ݭn��J
//               fixMsg.setField(TwseWarrantLP(TwseWarrantLP_Use));
//               return true;
//            }
//         }
//         // �����^�����O TwseWarrantLP
//         else if (ch != TwseWarrantLP_Use) {
//            if (ch == TwseWarrantLP_NotUse) {
//               // Client�ݩe�U�ɪ��q�妳 10003=N, �ҥH�ݭn��J
//               fixMsg.setField(TwseWarrantLP(TwseWarrantLP_NotUse));
//            }
//            else {
//               // Client�ݩe�U�ɪ��q��S�� 10003=Y, �ҥH���ݭn��J
//            }
//            
//            return true;
//         }
         return true;
      }

   } while (rshOt.MoveNext2());

   return false;
}
//---------------------------------------------------------------------------
