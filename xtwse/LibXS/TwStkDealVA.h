//
// TwStk �� [������Ӹ�ƪ�] �z�L TViewArea ��ܮ�, ������classes
//
// ����: $Id: TwStkDealVA.h,v 1.2 2004/09/14 02:54:35 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkDealVAH
#define TwStkDealVAH
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
#include "TwStkDeal.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
enum TStkDealFiledNo {
         sdfn_MSeqNo, //�����Ǹ�

         sdfn_Time,
         sdfn_Pri,
         sdfn_Qty,
         sdfn_BSeqNo, //��ӧǸ�

         sdfn_End,
};
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TStkDealVACr : public TVACreater
{
   TWSTK_class TFieldMaker {
   public:
      //���ѵ� TStkDeals �ϥ�
      typedef TStkDeals                TEvContainer;
      typedef TEvContainer::iterator   iterator;
      typedef size_t                   field_n_type;

      static TFieldDesp    FieldDesps[sdfn_End];

      inline static K_mf(field_n_type)      GetFieldCount ()                { return numofele(FieldDesps); }
      inline static K_mf(const TFieldDesp&) GetFieldDesp  (field_n_type n)  { return FieldDesps[n];        }

      K_mf(std::string)  Get  (           const iterator& iter, field_n_type fieldNo) const;
      K_mf(void)         Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string&);
   }; // class TFieldMaker
public:
   typedef TTViewArea<TStkDeals,TFieldMaker>    TViewArea;

   K_mf(TaViewArea)  CreateNewVA  ()   { return TaViewArea(new TViewArea(0)); }
};
//---------------------------------------------------------------------------
template <class TVAMaster, class TDetailMaker>
class TStkDealVACrDetail : public TStkDealVACr
{
   typedef TStkDealVACr                   inherited;
   typedef typename inherited::TViewArea  TVADetail;
public:
   typedef TVADetailLinker<TVAMaster, TVADetail, TDetailMaker>    TViewArea;
   K_mf(TaViewArea)  CreateNewVA  ()   { return TaViewArea(new TViewArea()); }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

