//
// TwStk �� �̷� [����ҩe�U�Ѹ�] ���覡, �w�q��ƪ� TTseOrdRecsDefine<>
//
// ����: $Id: TwStkOrdDefine.h,v 1.2 2003/11/11 05:15:29 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkOrdDefineH
#define TwStkOrdDefineH
//---------------------------------------------------------------------------
// ����ҩe�U�Ѹ���Ӫ�
//  ���-�����q-�����O(TSE/OTC/���d/TSE�s�Ѧ���/OTC�s�Ѧ���)
//    �d��-�Ǹ�(�d��:0---��9---)-�Ǹ�(0-999)-[���]
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
   //�̥k�ݪ���ƪ�,�e�U�Ǹ���T�X:
   struct index_maker_SeqNo3 : public Kstl::index_maker_base {
      typedef TTseOrdID::TSeq  index_type;
      inline K_mf(index_type)  operator()  (const TTseOrdID& a) const {
         return static_cast<index_type>(a.GetSeqNo() % (62*62*62));
      }
   };
   typedef Kstl::const_ary_base<TTseOrdID,RecT,index_maker_SeqNo3>   TRecs0;
   //class TRecs0 : public Kstl::const_ary_base<TTseOrdID, RecT, index_maker_SeqNo3> {};

   //���X�e�U�Ǹ��d��X(0---��z---):
   struct index_maker_SeqNoT : public Kstl::index_maker_base {
      typedef byte   index_type;
      inline K_mf(index_type)  operator()  (const TTseOrdID& a) const {
         return static_cast<index_type>( (a.GetSeqNo()/(62*62*62)) % 62);// %62 : �T�O�b��ƿ��~�ɵ{�����i���`�B�@
      }
   };
   //typedef Kstl::ary_base<TTseOrdID,TRecs0,index_maker_SeqNoT>    TRecs1;
   //class TRecs1 : public Kstl::ary_base<TTseOrdID,TRecs0,index_maker_SeqNoT> {};

   //�N TRecs1 �ŧi�����@�� ary2d:
   //typedef Kstl::ev_ary2d<TTseOrdID,TRecs1,TRecs0>  TEvSeqNos;
   //class TEvSeqNos : public Kstl::ev_ary2d<TTseOrdID,TRecs1,TRecs0> {};

   //typedef Kstl::ary2d_base<TTseOrdID,TRecs1,TRecs0>  TSeqRecs;
   //class TSeqRecs : public Kstl::ary2d_base<TTseOrdID,TRecs1,TRecs0> {};
   typedef Kstl::ary2dd_base<TTseOrdID,index_maker_SeqNoT,TRecs0>   TSeqRecs;

   //���X�d��('0'��'9'�B'A'��'Z'�B'a'��'z'):
   struct index_maker_TermID : public Kstl::index_maker_base {
      typedef byte   index_type;
      inline K_mf(index_type)  operator()  (const TTseOrdID& a) const {
         return static_cast<index_type>(a.GetTermSeq());
      }
   };
   //typedef Kstl::ary_base<TTseOrdID,TSeqRecs,index_maker_TermID>  TRecs;
   //class TRecs : public Kstl::ary_base<TTseOrdID,TSeqRecs,index_maker_TermID> {};

   //�̫Უ��ev_ary���w�q:
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

