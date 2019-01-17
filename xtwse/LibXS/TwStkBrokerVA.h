//
// TBrokerID 透過 TViewArea 顯示時, 欄位的顯示方式
//
// 版本: $Id: TwStkBrokerVA.h,v 1.2 2003/11/11 05:15:29 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkBrokerVAH
#define TwStkBrokerVAH
//---------------------------------------------------------------------------
#include "TwStkBroker.h"
#include "ViewAreaMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TBrokerFieldMaker_Base
{
public:
   typedef size_t                    field_n_type;

   static K_mf(const TFieldDesp&)    GetFieldDesp  (field_n_type);//僅提供一個欄位的顯示:[券商代號]
   static inline K_mf(field_n_type)  GetFieldCount ()    { return 1; }
};
//---------------------------------------------------------------------------
template <class TRecsDefine>
struct TBrokerFieldMaker : public TBrokerFieldMaker_Base
{
   typedef typename TRecsDefine::TEvRecs     TEvContainer;
   typedef typename TEvContainer::iterator   iterator;

   static inline K_mf(std::string)  Get  (const iterator& i, field_n_type)
                                             { return TRecsDefine::GetBrokerID(i).as_string(); }
   static inline K_mf(void)         Put  (TEvContainer&, iterator&, field_n_type, const std::string&)
                                             { }
};
//---------------------------------------------------------------------------
template <class TRecsDefine, class TDetailVACr, const char** detailDesp, size_t detailCount>
struct TBrokerVACreater : public TVACreater
{
   typedef typename TRecsDefine::TEvRecs     TEvRecs;
   typedef TBrokerFieldMaker<TRecsDefine>    TFieldMaker;
   typedef TTViewArea<TEvRecs,TFieldMaker>   TMasterVA;
   typedef typename TMasterVA::cursor_type   TMasterCursor;

   K_ctor            TBrokerVACreater      (TEvRecs& recs)     : Recs_(&recs) {}
   K_mf(TaViewArea)  CreateNewVA           ()                  { return TaViewArea(new TMasterVA(Recs_)); }
   K_mf(size_t)      GetDetails            (const char**& a)   { a = detailDesp;  return detailCount;     }
   K_mf(TVACreater*) GetDetailVACreater    (size_t detailID)   { return &DetailVACr_;                     }
   K_mf(TVACreater*) OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID)
                           { typedef typename TDetailVACr::TViewArea    TDetailVA;
                             const TMasterCursor*  cur = static_cast<TMasterVA&>(masterVA).GetCurRow();
                             if(cur==0  ||  cur->IsInvalid())
                                static_cast<TDetailVA*>(detailVA)->SetContainer(0);
                             else
                                DetailVACr_.SetContainer(*(cur->GetIterator()),
                                                         *(static_cast<TDetailVA*>(detailVA)),
                                                         detailID);
                             return DetailVACreaterNoChange; }
protected:
   TEvRecs*       Recs_;
   TDetailVACr    DetailVACr_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
