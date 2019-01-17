//
// TStkBase 透過 TViewArea 顯示時, 可以顯示的欄位:
//    class TStkBaseFieldMakerBase;
// TStkBase定義在TwStk.h
//
// 版本: $Id: TwStkBaseVA.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkBaseVAH
#define TwStkBaseVAH
//---------------------------------------------------------------------------
#include "ViewArea.h"
#include "TwStk.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
enum TStkBaseFieldNo {
         sbfn_Name,
         sbfn_Market,
         sbfn_Kind,
         sbfn_PriRef,
         sbfn_PriUpLmt,
         sbfn_PriDnLmt,
         sbfn_PriClose,
         sbfn_PriOdd,

         sbfn_End,
};
//---------------------------------------------------------------------------
TWSTK_gd(TFieldDesp)  StkBaseFieldDesp[sbfn_End];
//---------------------------------------------------------------------------
TWSTK_class  TStkBaseFieldMakerBase
{
public:
   typedef size_t    field_n_type;

inline K_mf(field_n_type)      GetFieldCount () const               { return numofele(StkBaseFieldDesp); }
inline K_mf(const TFieldDesp&) GetFieldDesp  (field_n_type n) const { return StkBaseFieldDesp[n];        }

       K_mf(std::string)  Get  (const TStkBase&, field_n_type) const;
       K_mf(void)         Put  (      TStkBase&, field_n_type, const std::string&);
};
//---------------------------------------------------------------------------
/*
TWSTK_class  TStkBaseFieldMaker : public TStkBaseFieldMakerBase
{
   typedef TStkBaseFieldMakerBase   inherited;
public:
   //提供給 TStkBases 使用
   typedef TStkBases                TEvContainer;
   typedef TEvContainer::iterator   iterator;

inline K_mf(field_n_type)      GetFieldCount () const      { return inherited::GetFieldCount()+1; }
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

