//
// TwStk 的 [交易所委託書號]
//
// 版本: $Id: TwStkTseOrdKey.h,v 1.2 2004/12/23 07:16:50 fonwin Exp $
//
// 2010/05/21 委託書號 = xxxxx, x = 0..9, A..Z, a..z
//
//---------------------------------------------------------------------------
#ifndef TwStkTseOrdKeyH
#define TwStkTseOrdKeyH
//---------------------------------------------------------------------------
#include "TwStk.h"
#include "TwStkBroker.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
K_class TTseOrdID //交易所委託書號
{
   typedef TTseOrdID    this_type;
public:
   typedef Kstl::char_ary1<char,' '>  TTerm;
   typedef dword                      TSeq;

   explicit inline K_ctor  TTseOrdID  (char term=0, TSeq seq=0)  : TermID_(term), SeqNo_(seq)  {}
   explicit inline K_ctor  TTseOrdID  (const std::string& str)    { assign(str); }

   inline K_mf(bool)  operator==  (const this_type& a) const    { return(TermID_==a.TermID_ && SeqNo_==a.SeqNo_); }
   inline K_mf(bool)  operator!=  (const this_type& a) const    { return(TermID_!=a.TermID_ || SeqNo_!=a.SeqNo_); }
   inline K_mf(bool)  operator <  (const this_type& a) const
                                           { if(TermID_ == a.TermID_)
                                                return(SeqNo_ < a.SeqNo_);
                                             return(TermID_ < a.TermID_); }
   inline K_mf(byte)  GetTermSeq () const  { return static_cast<byte>(GetAlpha2Seq(TermID_[0]) % 62); }
                                                                           // ↑確保資料的正確(% 62):
   inline K_mf(TTerm) GetTermID  () const  { return TermID_; }
   inline K_mf(TSeq)  GetSeqNo   () const  { return SeqNo_;  }
   inline K_mf(bool)  IsInvalid  () const  { return TermID_.empty(); }

   K_mf(std::string)  as_string  (bool noDash=false) const;
   K_mf(void)         assign     (const std::string&);

private:
   TTerm    TermID_;
   TSeq     SeqNo_;
};
//---------------------------------------------------------------------------
struct TTseOrdKey
{
   TBrokerID   BrokerID_;
   TMarket     Market_;
   TTseOrdID   TseOrdID_;

   K_ctor  TTseOrdKey  ()  {}
   K_ctor  TTseOrdKey  (TBrokerID bid, TMarket m, TTseOrdID tid)
                           : BrokerID_(bid), Market_(m), TseOrdID_(tid)
                           { }

   K_mf(TBrokerID)  GetBrokerID  () const  { return BrokerID_; }
   K_mf(TMarket)    GetMarket    () const  { return Market_;   }
   K_mf(TTseOrdID)  GetTseOrdID  () const  { return TseOrdID_; }

   inline K_mf(bool)  operator<  (const TTseOrdKey& a) const
      { return ( BrokerID_ == a.BrokerID_
               ? (Market_  == a.Market_
                  ? TseOrdID_ < a.TseOrdID_
                  : Market_   < a.Market_)
               : BrokerID_ < a.BrokerID_ ); }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
