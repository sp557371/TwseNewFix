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
   // 指定 SpecRule
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

   // 檢查 Tw.Stk.IvacNo 是否和 FIX.Stk.Account 一致
   // 在字串儲存裡面, 有可能是 0000094 or 94, 這樣比對會有問題
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

   // 檢查 Tw.Stk.IvacNo 是否和 FIX.Stk.Account 一致
   // 在字串儲存裡面, 有可能是 0000094 or 94, 這樣比對會有問題
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
// 移除 10003 的傳送
//   // 先取得這筆回報是不是 TwseWarrantLP = Y
//   FixMessage&         fixMsg          = refErOrd.GetFixMsg   ();
//   SFixRfBaseField&    fixRfBaseFd     = refErOrd.GetBaseField();
//   STwseFsOrdReseChar* twseRChar       = reinterpret_cast<STwseFsOrdReseChar*>(fixRfBaseFd.GetReseChar());
//   bool                isTwseWarrantLP = (twseRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use);

   // 先移動到第一筆,依序往下尋找,尋找尚未回報的部份
   if (rshOt.FindMap2() == NULL || rshOt.MoveFirst2() == false)
      return false;
   
   do {
      const SFixOrdTrack& cdOt = rshOt.GetDat2(false);

      if (cdOt.MsgType_ == fixMsgType && cdOt.FlowID_.IsFinish() == false) {
//         const STwseOrdTrackReseChar* twseOtRChar = reinterpret_cast<const STwseOrdTrackReseChar*>(cdOt.GetRChar());
//               char                   ch          = twseOtRChar->TwseWarrantLP_.get_orig();
//
//         // 此筆回報是 TwseWarrantLP
//         if (isTwseWarrantLP) {
//            if (ch == TwseWarrantLP_Use) {
//               // Client端委託時的電文有 10003=Y, 所以需要填入
//               fixMsg.setField(TwseWarrantLP(TwseWarrantLP_Use));
//               return true;
//            }
//         }
//         // 此筆回報不是 TwseWarrantLP
//         else if (ch != TwseWarrantLP_Use) {
//            if (ch == TwseWarrantLP_NotUse) {
//               // Client端委託時的電文有 10003=N, 所以需要填入
//               fixMsg.setField(TwseWarrantLP(TwseWarrantLP_NotUse));
//            }
//            else {
//               // Client端委託時的電文沒有 10003=Y, 所以不需要填入
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
