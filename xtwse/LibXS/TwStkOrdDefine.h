//
// TwStk 的 依照 [交易所委託書號] 的方式, 定義資料表 TTseOrdRecsDefine<>
//
// 版本: $Id: TwStkOrdDefine.h,v 1.2 2003/11/11 05:15:29 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkOrdDefineH
#define TwStkOrdDefineH
//---------------------------------------------------------------------------
// 交易所委託書號對照表
//  券商-分公司-市場別(TSE/OTC/興櫃/TSE零股成交/OTC零股成交)
//    櫃號-序號(千位:0---～9---)-序號(0-999)-[資料]
//
#include "ev_ary2d.h"
#include "ary_recs.h"
//---------------------------------------------------------------------------
#include "TwStkTseOrdKey.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
template <class RecT>
class TTseOrdRecsDefine
{
public:
   //最右端的資料表,委託序號後三碼:
   struct index_maker_SeqNo3 : public Kstl::index_maker_base {
      typedef TTseOrdID::TSeq  index_type;
      inline K_mf(index_type)  operator()  (const TTseOrdID& a) const {
         return static_cast<index_type>(a.GetSeqNo() % (62*62*62));
      }
   };
   typedef Kstl::const_ary_base<TTseOrdID,RecT,index_maker_SeqNo3>   TRecs0;
   //class TRecs0 : public Kstl::const_ary_base<TTseOrdID, RecT, index_maker_SeqNo3> {};

   //結合委託序號千位碼(0---～z---):
   struct index_maker_SeqNoT : public Kstl::index_maker_base {
      typedef byte   index_type;
      inline K_mf(index_type)  operator()  (const TTseOrdID& a) const {
         return static_cast<index_type>( (a.GetSeqNo()/(62*62*62)) % 62);// %62 : 確保在資料錯誤時程式仍可正常運作
      }
   };
   //typedef Kstl::ary_base<TTseOrdID,TRecs0,index_maker_SeqNoT>    TRecs1;
   //class TRecs1 : public Kstl::ary_base<TTseOrdID,TRecs0,index_maker_SeqNoT> {};

   //將 TRecs1 宣告成為一個 ary2d:
   //typedef Kstl::ev_ary2d<TTseOrdID,TRecs1,TRecs0>  TEvSeqNos;
   //class TEvSeqNos : public Kstl::ev_ary2d<TTseOrdID,TRecs1,TRecs0> {};

   //typedef Kstl::ary2d_base<TTseOrdID,TRecs1,TRecs0>  TSeqRecs;
   //class TSeqRecs : public Kstl::ary2d_base<TTseOrdID,TRecs1,TRecs0> {};
   typedef Kstl::ary2dd_base<TTseOrdID,index_maker_SeqNoT,TRecs0>   TSeqRecs;

   //結合櫃號('0'～'9'、'A'～'Z'、'a'～'z'):
   struct index_maker_TermID : public Kstl::index_maker_base {
      typedef byte   index_type;
      inline K_mf(index_type)  operator()  (const TTseOrdID& a) const {
         return static_cast<index_type>(a.GetTermSeq());
      }
   };
   //typedef Kstl::ary_base<TTseOrdID,TSeqRecs,index_maker_TermID>  TRecs;
   //class TRecs : public Kstl::ary_base<TTseOrdID,TSeqRecs,index_maker_TermID> {};

   //最後產生ev_ary的定義:
   //typedef Kstl::ev_ary2d<TTseOrdID,TRecs,TSeqRecs>   TEvRecs;
   //class TEvRecs : public Kstl::ev_ary2d<TTseOrdID,TRecs,TSeqRecs> {};
   typedef Kstl::ev_ary2dd<TTseOrdID,index_maker_TermID,TSeqRecs>  TEvRecs;

   static K_mf(TTseOrdID)  GetTseOrdID  (const typename TEvRecs::iterator& i) {
      typename TEvRecs::iterator::right_iterator   ri = i.get_righti();
      return TTseOrdID( GetSeq2Alpha(i.get_lefti().get_idx()),
         static_cast<TTseOrdID::TSeq>(ri.get_lefti().get_idx() * (62*62*62)  +  ri.get_righti().get_idx()) );
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

