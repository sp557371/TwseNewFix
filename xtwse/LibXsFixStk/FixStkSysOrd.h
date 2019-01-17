/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkSysOrd_h_)
#define _FixStkSysOrd_h_
//---------------------------------------------------------------------------
#include <deque>
#include "ev_ary.h"
#include "wise_ptr.h"
#include "FixStkOrder.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace FIX  {
namespace Stk  {

/////////////////////////////////////////////////////////////////////////////
// 基本/輔助
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::wise_ptr<TFsStkOrder>  TFsSysOrd;
typedef TFsSysOrd::ref_obj           TFsSysOrdRef;
//---------------------------------------------------------------------------
//struct pod_allocator_TFsSysOrd : public Kstl::pod_allocator_base
//{
//   static const size_t  reserve_elements = 1024*10;   //每當記憶體重新分配,需要預留的空間
//};


/////////////////////////////////////////////////////////////////////////////
// class TFsSysOrds
/////////////////////////////////////////////////////////////////////////////
K_class TFsSysOrds : public Kstl::ev_ary<TFixSysOrdID, TFsSysOrd,
                                         Kstl::index_maker<TFixSysOrdID>,
                                         //Kstl::pod_vector <TFsSysOrd, pod_allocator_TFsSysOrd> >
                                         std:: deque<TFsSysOrd> >
{
   typedef Kstl::ev_ary<TFixSysOrdID, TFsSysOrd,
                        Kstl::index_maker<TFixSysOrdID>,
                        //Kstl::pod_vector <TFsSysOrd, pod_allocator_TFsSysOrd> >  inherited;
                        std:: deque<TFsSysOrd> >  inherited;

   typedef inherited::updater             inherited_updater;
   typedef inherited::updater::impl_type  updater_impl_base;
   typedef TFsSysOrds                     this_type;

   //==============================================
   // 以下屬性不適用, 所以沒有 implement
   K_mf(this_type&) operator= (const this_type&);
   K_mf(this_type&) assign    (const this_type&);
   K_ctor           TFsSysOrds (const this_type&);

public:
   //================================================================
   // class TUpdaterImpl
   //================================================================
   K_class updater;
   //--------------------
   K_class TUpdaterImpl : public updater_impl_base
   {
      typedef updater_impl_base  inherited;
//      using inherited::flush_update;

      TFixUpdFrom  UpdFrom_;
      TFsSysOrds*  SysOrds_;
      TFsSysOrd    BfOrd_;
      TFsSysOrd    Ord_;
      enum {
         isNewOrd,
         isIterOK,
      } State_;

      friend class updater;
//      inline K_mf(bool)  IsNewOrd  () const  { return State_==isNewOrd; }
//      inline K_mf(bool)  IsIterOK  () const  { return State_==isIterOK; }

   public:
      K_ctor  TUpdaterImpl (TFixUpdFrom, TFsSysOrds&, const TFsSysOrds::iterator&);
      K_dtor ~TUpdaterImpl ();

      //===========================================
      // overwrite updater_impl_base virtual function
      K_mf(bool)      is_iterator_eq (const Kstl::ev_iterator& i) const;
      K_mf(bool)      bf_end_update  (bool forFlush);
      K_mf(reference) get_modifyer   ();

      //===========================================
      // TUpdaterImpl function
      K_mf(TFixSysOrdID)       GetSysOrdID () const;
      K_mf(const TFsStkOrder*) GetConst    () const;
      K_mf(const TFsStkOrder*) GetBfConst  () const;

      //===========================================
      // TUpdaterImpl inline function
      inline K_mf(TFsSysOrds&)      GetSysOrds () const  { return *SysOrds_; }
      inline K_mf(TFixUpdFrom)      GetUpdFrom () const  { return UpdFrom_;  }
      inline K_mf(const TFsSysOrd&) GetBfOrd   () const  { return BfOrd_;    }
      inline K_mf(const TFsSysOrd&) GetAfOrd   () const  { return Ord_;      }
   };

   //==============================================
   // class updater
   K_class updater : public inherited_updater
   {
      typedef inherited_updater  inherited;
      typedef updater            this_type;
//      using inherited::flush_update;

   public:
      K_ctor  updater (TUpdaterImpl* = 0);
      K_ctor  updater (const this_type&);
      K_dtor ~updater ();

      K_mf(bool) FlushUpdate (TFixUpdFrom);

      K_mf(this_type&)  operator=  (const this_type&);

      inline K_mf(const TUpdaterImpl*) GetImpl     () const  { return static_cast<const TUpdaterImpl*>(get_impl()); }
      inline K_mf(const TFsStkOrder*)  GetBfConst  () const  { return GetImpl()->GetBfConst();                      }
      inline K_mf(const TFsStkOrder*)  GetConst    () const  { return GetImpl()->GetConst();                        }
      inline K_mf(TFsStkOrder&)        GetModify   ()        { return *(get_impl()->get_modifyer());                }
      inline K_mf(TFixSysOrdID)        GetSysOrdID () const  { return GetImpl()->GetSysOrdID();                     }
   };

public:
   K_ctor TFsSysOrds ();
   
   //==============================================
   // TSysOrds function
          K_mf(updater) begin_update (const iterator&, TFixUpdFrom = fuf_Unknown);
   inline K_mf(updater) begin_insert (TFixUpdFrom uf)  { return begin_update(end(), uf); }

private:
   //==============================================
   // overwrite Kstl::ev_ary virtual function
   K_mf(TUpdaterImpl*) updater_factory (const ev_iterator&);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkSysOrd_h_)
