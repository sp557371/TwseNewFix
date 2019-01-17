//
// TwStk 的 [投資人帳號] 的相關 classes
//
// 版本: $Id: TwStkIvacNo.h,v 1.7 2005/12/28 08:58:08 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkIvacNoH
#define TwStkIvacNoH
//---------------------------------------------------------------------------
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
// 投資人帳號 Invest Account No (分公司View)
//
TWSTK_class TIvacNo
{
   typedef TIvacNo   this_type;
   dword    IvacNo_; //[含]檢查碼的投資人帳號
public:
   inline K_ctor  TIvacNo  (const char* str)          { Assign(str); }
   inline K_ctor  TIvacNo  (const std::string& str)   { assign(str); }
   inline explicit
          K_ctor  TIvacNo  (dword n=0)    : IvacNo_(n) { }

   inline K_mf(dword)  ToInt_NoChk  () const  { return IvacNo_ / 10;    }
   inline K_mf(dword)  ToInt        () const  { return IvacNo_;         }
   inline K_mf(void)   Assign       (dword n) { IvacNo_ = n % 10000000; }//含檢查碼
          K_mf(void)   Assign       (const char*);//含檢查碼,檢查碼前有無[-]皆可
          K_mf(void)   assign       (const std::string& str)   { Assign(str.c_str()); }
          K_mf(void)   ValidChkCode (TBrokerID);
          K_mf(bool)   IsInvalid    (TBrokerID) const;

   K_mf(std::string)  as_string  (char spl = '-', bool longstr = false) const;
   //要顯示帳號 ex:"0000327" 須用 IvacNo.as_string(0,true);

   inline K_mf(bool)  operator<  (const this_type a) const  { return(IvacNo_ <  a.IvacNo_); }
   inline K_mf(bool)  operator== (const this_type a) const  { return(IvacNo_ == a.IvacNo_); }
   inline K_mf(bool)  operator!= (const this_type a) const  { return(IvacNo_ != a.IvacNo_); }
};
//---------------------------------------------------------------------------
//
// 投資人帳號 = IvacKey = BrokerID + IvacNo (證券市場View)
//
TWSTK_class TIvacKey
{
   typedef TIvacKey  this_type;

   TBrokerID   BrokerID_;
   TIvacNo     IvacNo_;
public:
   inline K_ctor  TIvacKey  ()  { }
   inline K_ctor  TIvacKey  (TBrokerID brokerID, TIvacNo ivacNo)
                              : BrokerID_(brokerID), IvacNo_(ivacNo)
                              { }

   inline K_mf(TBrokerID)  GetBrokerID  () const  { return BrokerID_;               }
   inline K_mf(TBrokerNo)  GetBrokerNo  () const  { return BrokerID_.GetBrokerNo(); }
   inline K_mf(TBranchNo)  GetBranchNo  () const  { return BrokerID_.GetBranchNo(); }
   inline K_mf(char)       GetBranchCode() const  { return BrokerID_.GetBranchNo().GetBranchCode(); }
   inline K_mf(TIvacNo)    GetIvacNo    () const  { return IvacNo_;                 }

   inline K_mf(void)  SetIvacNo  (TIvacNo a)                { IvacNo_ = a; }
   inline K_mf(void)  Set        (TBrokerID b, TIvacNo a)   { BrokerID_ = b;  IvacNo_ = a; }
          K_mf(void)  assign     (const std::string&);
   K_mf(std::string)  as_string  () const;

   inline K_mf(bool)  operator<  (const this_type& a) const { return( BrokerID_ == a.BrokerID_
                                                                     ? (IvacNo_   < a.IvacNo_)
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
