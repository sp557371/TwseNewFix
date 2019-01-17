//
// TwStk 的 [成交明細資料表] 透過 TViewArea 顯示時, 的相關classes
//
// 版本: $Id: TwStkDealVA.h,v 1.2 2004/09/14 02:54:35 fonwin Exp $
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
         sdfn_MSeqNo, //市場序號

         sdfn_Time,
         sdfn_Pri,
         sdfn_Qty,
         sdfn_BSeqNo, //券商序號

         sdfn_End,
};
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TStkDealVACr : public TVACreater
{
   TWSTK_class TFieldMaker {
   public:
      //提供給 TStkDeals 使用
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

