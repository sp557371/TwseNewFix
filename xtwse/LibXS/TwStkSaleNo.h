//
// TwStk 的 [營業員代號]
//
// 版本: $Id: TwStkSaleNo.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkSaleNoH
#define TwStkSaleNoH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "TwStkBroker.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
/////////////////////////////////////////////////////////////////////////////
//
// 營業員代號 (分公司View)
//
typedef Kstl::char_ary<char,4,' '>     TSaleNo;
//---------------------------------------------------------------------------
//
// 營業員Key = SaleKey = BrokerID + SaleNo (證券市場View)
//
TWSTK_class TSaleKey
{
   typedef TSaleKey     this_type;
   Tw::Stk::TBrokerID   BrokerID_;
   TSaleNo              SaleNo_;
public:
   inline K_ctor  TSaleKey  ()  { }
   inline K_ctor  TSaleKey  (TBrokerID brokerID, TSaleNo saleNo)
                              : BrokerID_(brokerID), SaleNo_(saleNo)
                              { }

   inline K_mf(TBrokerID)  GetBrokerID  () const  { return BrokerID_;               }
   inline K_mf(TBrokerNo)  GetBrokerNo  () const  { return BrokerID_.GetBrokerNo(); }
   inline K_mf(TBranchNo)  GetBranchNo  () const  { return BrokerID_.GetBranchNo(); }
   inline K_mf(TSaleNo)    GetSaleNo    () const  { return SaleNo_;                 }

   inline K_mf(void)  SetSaleNo  (TSaleNo a)                { SaleNo_ = a; }
   inline K_mf(void)  Set        (TBrokerID b, TSaleNo a)   { BrokerID_ = b;  SaleNo_ = a; }
          K_mf(void)  assign     (const std::string&);
   K_mf(std::string)  as_string  () const;

   inline K_mf(bool)  operator<  (const this_type& a) const { return( BrokerID_ == a.BrokerID_
                                                                     ? (SaleNo_   < a.SaleNo_)
                                                                     : (BrokerID_ < a.BrokerID_) ); }
};
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
