/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkSysOrd.h"
#include "Nsa_UtilComm.h"
#include "Nsa_DbgInfo.h"
#include "FixStkVer.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TFsSysOrds::TUpdaterImpl impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFsSysOrds::TUpdaterImpl::TUpdaterImpl (TFixUpdFrom uf, TFsSysOrds& recs, const TFsSysOrds::iterator& i)
   : inherited(recs, i)
   , UpdFrom_ (uf)
   , SysOrds_ (&recs)
   , State_   (i==recs.end() ? isNewOrd : isIterOK)
{
}
//---------------------------------------------------------------------------

K_dtor TFsSysOrds::TUpdaterImpl::~TUpdaterImpl ()
{
//   if(Ord_.get())
//      SysOrds_->OnOrdAfEndUpdate(*this);
}
//---------------------------------------------------------------------------

K_mf(bool) TFsSysOrds::TUpdaterImpl::is_iterator_eq (const Kstl::ev_iterator& i) const
{
   return (State_==isNewOrd ? false : get_iterator()==i);
}
//---------------------------------------------------------------------------

K_mf(bool) TFsSysOrds::TUpdaterImpl::bf_end_update (bool forFlush)
{
   if (Ord_.get() == 0) {
      //資料沒有任何異動,不需要更新
      if (!forFlush)
         cancel_update();
      return false;
   }
   
   if (State_ == isNewOrd) {
      Iter_  = SysOrds_->insert(TFsSysOrds::value_type(GetSysOrdID(), Ord_)).first;
      State_ = isIterOK;
      if (!forFlush) {
         cancel_update();  // insert()新增, 不需要產生 OnContainerAfEndUpdate() 事件
         return false;
      }
   }
   else {
      const_cast<TFsSysOrd&>(*Iter_) = Ord_;
   }
   
   if (forFlush) {
//      SysOrds_->OnOrdAfEndUpdate(*this);
      Ord_.reset(0);
   }
   return true;
}
//---------------------------------------------------------------------------

K_mf(TFsSysOrds::TUpdaterImpl::reference) TFsSysOrds::TUpdaterImpl::get_modifyer ()
{
   if (Ord_.get() == 0) {
      Ord_.renew();
      if (State_ == isNewOrd) {
      }
      else {
         //複製委託到異動緩衝區
         if (Iter_->get())
            *Ord_ = *(BfOrd_ = *Iter_);
      }
   }
   return Ord_;
}
//---------------------------------------------------------------------------

K_mf(TFixSysOrdID) TFsSysOrds::TUpdaterImpl::GetSysOrdID () const
{
   if (State_ == isNewOrd) {
      return TFixSysOrdID(SysOrds_->empty() ? 1 : ((SysOrds_->end()-1).get_idx()+1));
   }
   return TFixSysOrdID(Iter_.get_idx());
}
//---------------------------------------------------------------------------

K_mf(const TFsStkOrder*) TFsSysOrds::TUpdaterImpl::GetConst () const
{
   if (Ord_.get())
      return Ord_.get();
   return GetBfConst();
}
//---------------------------------------------------------------------------

K_mf(const TFsStkOrder*) TFsSysOrds::TUpdaterImpl::GetBfConst () const
{
   return (State_==isIterOK ? Iter_->get() : 0);
}


/////////////////////////////////////////////////////////////////////////////
// class TFsSysOrds::updater impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFsSysOrds::updater::updater (TUpdaterImpl* uimpl)
   : inherited(uimpl)
{
}
//---------------------------------------------------------------------------

K_ctor TFsSysOrds::updater::updater (const this_type& a)
   : inherited(a)
{
}
//---------------------------------------------------------------------------

K_dtor TFsSysOrds::updater::~updater ()
{
   #if defined(_NsaVer_DbgInfo_)
   char buf[128];
   sprintf(buf, "[%d] %p", GetSysOrdID().get_orig(), this);

   TDbgInfoHnd diHnd(edi_TwseFix_TFsSysOrds_updater_dtor, NULL, true, std::string(buf));
   #endif
}
//---------------------------------------------------------------------------

K_mf(bool) TFsSysOrds::updater::FlushUpdate (TFixUpdFrom uf)
{
   if (is_invalid())
      return false;

   TFsSysOrds::TUpdaterImpl* Impl  = static_cast<TFsSysOrds::TUpdaterImpl*>(get_impl());
   TFixUpdFrom               uFrom = Impl->UpdFrom_;
   
   Impl->UpdFrom_ = uf;
   Impl->flush_update();
   Impl->UpdFrom_ = uFrom;
   
   return true;
}
//---------------------------------------------------------------------------

K_mf(TFsSysOrds::updater&) TFsSysOrds::updater::operator = (const this_type& a)
{
   inherited::operator = (a);
   return *this;
}


/////////////////////////////////////////////////////////////////////////////
// class TFsSysOrds impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFsSysOrds::TFsSysOrds ()
{
}
//---------------------------------------------------------------------------

static TFixUpdFrom  StaticUpdFromParam;
//---------------------------------------------------------------------------

K_mf(TFsSysOrds::updater) TFsSysOrds::begin_update (const iterator& iter, TFixUpdFrom uf)
{
   StaticUpdFromParam = uf;
   return updater(static_cast<TUpdaterImpl*>(Kstl::ev_container::begin_update(iter)));
}
//---------------------------------------------------------------------------

K_mf(TFsSysOrds::TUpdaterImpl*) TFsSysOrds::updater_factory (const Kstl::ev_iterator& evi)
{
   return new TUpdaterImpl(StaticUpdFromParam, *this, static_cast<const iterator&>(evi));
}
//---------------------------------------------------------------------------
