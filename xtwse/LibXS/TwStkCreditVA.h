//
// TwStk 的 [信用配額資料] 透過 TViewArea 顯示時, 欄位的顯示方式
//
// 版本: $Id: TwStkCreditVA.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkCreditVAH
#define TwStkCreditVAH
//---------------------------------------------------------------------------
#include "ViewArea.h"
//---------------------------------------------------------------------------
#include "libnsTwStk.h"
#include "TwStkCredit.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
enum TStkCreditFiledNo {//市場上信用交易的資訊,與證金公司無關
         scfn_CrStop,   //停資
         scfn_CrRate,   //成數

         scfn_DbStop,   //停券
         scfn_DbRate,   //成數

         scfn_End,
};
//---------------------------------------------------------------------------
TWSTK_gd(TFieldDesp)  StkCreditFieldDesp[scfn_End];
//---------------------------------------------------------------------------
TWSTK_class  TStkCreditFieldMakerBase
{
public:
   typedef size_t    field_n_type;

inline K_mf(field_n_type)      GetFieldCount () const               { return numofele(StkCreditFieldDesp); }
inline K_mf(const TFieldDesp&) GetFieldDesp  (field_n_type n) const { return StkCreditFieldDesp[n];        }

       K_mf(std::string)  Get  (const TStkCredit&, field_n_type) const;
       K_mf(void)         Put  (      TStkCredit&, field_n_type, const std::string&);
};
//---------------------------------------------------------------------------
/*
TWSTK_class  TStkCreditFieldMaker : public TStkCreditFieldMakerBase
{
   typedef TStkCreditFieldMakerBase   inherited;
public:
   //提供給 TStkCredits 使用
   typedef TStkCredits                TEvContainer;
   typedef TEvContainer::iterator   iterator;

inline K_mf(field_n_type)      GetFieldCount () const   { return numofele(StkCreditFieldDesp)+1; }
       K_mf(const TFieldDesp&) GetFieldDesp  (field_n_type n) const;

   K_mf(std::string)  Get  (           const iterator& iter, field_n_type fieldNo) const;
   K_mf(void)         Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string&);
};
*/
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
