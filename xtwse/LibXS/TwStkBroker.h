//
// TwStk: 券商代號的 classes:
//    TBrokerNo, TBranchNo, TBrokerID
//
// 版本: $Id: TwStkBroker.h,v 1.5 2005/06/04 01:22:26 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkBrokerH
#define TwStkBrokerH
//---------------------------------------------------------------------------
#include "libnsTwStk.h"
#include "AlphaSeq.h"
#include "ary_recs.h"
#include "ev_ary2d.h"
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <algorithm>
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
//
// 券商代號 = BrokerID = BrokerNo + BranchNo
//
TWSTK_class TBrokerNo
{
   typedef TBrokerNo this_type;
   typedef word      brokerno_type;
   brokerno_type     BrokerNo_;  // 000 - 999
   static const brokerno_type    BrokerEnd = 1000;
public:
   inline explicit K_ctor  TBrokerNo  (unsigned brokerSeqNo = 0) : BrokerNo_(static_cast<brokerno_type>(brokerSeqNo % BrokerEnd)) { }
   inline explicit K_ctor  TBrokerNo  (const char* a)            : BrokerNo_(static_cast<brokerno_type>(atoi(a) % BrokerEnd))     { }

   inline K_mf(brokerno_type)  GetSeqNo  () const        { return BrokerNo_;                  }
   inline K_mf(void)           Assign    (unsigned n)    { BrokerNo_ = static_cast<brokerno_type>(n % BrokerEnd); }
   inline K_mf(void)           Assign    (const char* a) { Assign(atoi(a));                   }
   inline K_mf(void)           Swap      (TBrokerNo&  b) { std::swap(BrokerNo_, b.BrokerNo_); }
   K_mf(std::string)           as_string () const;

   inline K_mf(bool)  operator==  (const this_type a) const  { return(BrokerNo_ == a.BrokerNo_); }
   inline K_mf(bool)  operator!=  (const this_type a) const  { return(BrokerNo_ != a.BrokerNo_); }
};
//---------------------------------------------------------------------------
TWSTK_class TBranchNo
{
   typedef TBranchNo this_type;
   typedef byte      branchno_type;
   branchno_type     BranchSeq_;
public:
   static const branchno_type    BranchEnd = 63;   //0-61 = 0-9,A-Z,a-z 、 62 = *

   inline          K_ctor  TBranchNo  ()                          : BranchSeq_(0)                           { }
   inline explicit K_ctor  TBranchNo  (branchno_type branchSeqNo) : BranchSeq_(branchSeqNo)                 { }
   inline explicit K_ctor  TBranchNo  (char branchCode)           : BranchSeq_(branchCode=='*' ? (char)(BranchEnd-1)
                                                                             : Kway::Alpha2Seq[static_cast<byte>(branchCode)]) { }

   inline K_mf(branchno_type)  GetSeqNo      () const   { return BranchSeq_;                    }
   inline K_mf(char)           GetBranchCode () const   { return BranchSeq_ >= (BranchEnd-1) ? '*'
                                                               : Kway::Seq2Alpha[BranchSeq_];   }
   inline K_mf(void)           Swap    (TBranchNo& b)   { std::swap(BranchSeq_, b.BranchSeq_);  }
   inline K_mf(bool)           IsMark  () const   { return(BranchSeq_ >= (BranchEnd-1)); }

   inline K_mf(bool)  operator==  (const this_type a) const  { return(BranchSeq_ == a.BranchSeq_); }
   inline K_mf(bool)  operator!=  (const this_type a) const  { return(BranchSeq_ != a.BranchSeq_); }
};
//---------------------------------------------------------------------------
TWSTK_class TBrokerID
{
   struct TBrokerStrBuf {  //在 assign() 時會用到
      char BrokerNo[3];
      char eos;
   };
   typedef TBrokerID    this_type;
public:
   typedef word        brokeridx_type; // 1000 * 63(0-9,A-Z,a-z,*,共63個) = 63000
   static const byte   BranchSize = TBranchNo::BranchEnd;

   inline K_ctor  TBrokerID  ()  : BrokerIdx_(0)  { }
   inline K_ctor  TBrokerID  (TBrokerNo brokerNo, TBranchNo branchNo)
                              : BrokerIdx_( static_cast<brokeridx_type>( brokerNo.GetSeqNo()*BranchSize
                                                                       + branchNo.GetSeqNo() ) )
                              { }
   inline explicit K_ctor  TBrokerID  (brokeridx_type idx)  : BrokerIdx_(idx) { }
   inline explicit K_ctor  TBrokerID  (const std::string& str)   { assign(str); }

   inline K_mf(bool)       IsMark       () const  { return(BrokerIdx_==BrokerMark); }
   inline K_mf(TBrokerNo)  GetBrokerNo  () const  { return TBrokerNo(static_cast<word>(BrokerIdx_ / BranchSize)); }
   inline K_mf(TBranchNo)  GetBranchNo  () const  { return TBranchNo(static_cast<byte>(BrokerIdx_ % BranchSize)); }
   inline K_mf(char)       GetBranchCode() const  { return GetBranchNo().GetBranchCode(); }
   K_mf(const char*)       assign       (const std::string&);//傳回:BrokerID之後的index,方便接下來欄位(TIvacKey)解譯用
   K_mf(std::string)       as_string    (bool isLongStr = false) const;//if(isLongStr) format = "XXX-X"

   inline K_mf(bool)  operator<   (const this_type a) const  { return(BrokerIdx_ <  a.BrokerIdx_); }
   inline K_mf(bool)  operator==  (const this_type a) const  { return(BrokerIdx_ == a.BrokerIdx_); }
   inline K_mf(bool)  operator!=  (const this_type a) const  { return(BrokerIdx_ != a.BrokerIdx_); }

private:
   brokeridx_type      BrokerIdx_;
   static const brokeridx_type   BrokerMark = (brokeridx_type)(-1);
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
template <class RecT>
class TBrokerRecsDefine  //兩層式的陣列:[BrokerNo][BranchNo]
{
public:
   //最右端的資料表:分公司碼:
   struct index_maker_BranchNo : public Kstl::index_maker_base {
      typedef byte   index_type;
      inline K_mf(index_type)  operator()  (const TBrokerID a) const  { return a.GetBranchNo().GetSeqNo(); }
   };
   typedef Kstl::const_ary_base<TBrokerID,RecT,index_maker_BranchNo>   TRecs0;
   //class TRecs0 : public Kstl::const_ary_base<TBrokerID, RecT, index_maker_SeqNo3> {};

   //左端的資料表:券商代號:
   struct index_maker_BrokerNo : public Kstl::index_maker_base {
      typedef word   index_type;
      inline K_mf(index_type)  operator()  (const TBrokerID a) const  { return a.GetBrokerNo().GetSeqNo(); }
   };
   typedef Kstl::ary_base<TBrokerID,TRecs0,index_maker_BrokerNo>    TRecs1;
   //class TRecs1 : public Kstl::ary_base<TBrokerID,TRecs0,index_maker_SeqNoT> {};

   //將 TRecs1 宣告成為一個 ary2d:
   //typedef Kstl::ev_ary2d<TBrokerID,TRecs1,TRecs0>  TEvRecs;
   //class TEvBrokers: public Kstl::ev_ary2d<TBrokerID,TRecs1,TRecs0> {};
   typedef Kstl::ev_ary2dd<TBrokerID,index_maker_BrokerNo,TRecs0>  TEvRecs;

   static TBrokerID  GetBrokerID  (const typename TEvRecs::iterator& i)
   {
      return TBrokerID( static_cast<TBrokerID::brokeridx_type>(
                                       i.get_lefti().get_idx() * TBrokerID::BranchSize
                                     + i.get_righti().get_idx() ) );
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

