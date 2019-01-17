//
// TwStk 的 [委託書資料表]
//
// 版本: $Id: TwStkSysOrd.h,v 1.9 2006/04/04 03:30:55 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkSysOrdH
#define TwStkSysOrdH
//---------------------------------------------------------------------------
#include "ev_ary.h"
#include "wise_ptr.h"
#include "TimerThread.h"
//---------------------------------------------------------------------------
#include "TwStkOrder.h"
#include "TwStkOrdDefine.h"
#include "SysMgrObjs.h"
//---------------------------------------------------------------------------
namespace Kway { namespace Tw { namespace Stk {
   typedef wise_ptr<TStkOrder>   TSysOrd;

   struct pod_allocator_TSysOrd : public Kstl::pod_allocator_base
   {
      static const size_t  reserve_elements = 1024*10;   //每當記憶體重新分配,需要預留的空間
   };
} } }
//---------------------------------------------------------------------------
namespace Kstl {
   // gcc 說只能把 extern template 放在與 ev_ary<> 相同的 namespace 裡面!
   // vc  說 TSysOrds 是 DLL export, 而 TSysOrds 繼承自 ev_ary<>, 所以 ev_ary<> 也要是 DLL export
   //     而且必須要在定義 TSysOrds 之前, 就先告知!
   // 所以底下這行就必須放在這兒, 而 TSysOrd, pod_allocator_TSysOrd 又必須先定義好(所以必須放在上面)
   // 最後, 就變成現在這個醜樣了 :(
   TWSTK_EXTERN_TEMPLATE_CLASS   ev_ary< Kway::Tw::Stk::TSysOrdID, Kway::Tw::Stk::TSysOrd
                                       , index_maker<Kway::Tw::Stk::TSysOrdID>
                                       , Kstl::pod_vector<Kway::Tw::Stk::TSysOrd, Kway::Tw::Stk::pod_allocator_TSysOrd>
                                       >;
}
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
using namespace Kstl;
/////////////////////////////////////////////////////////////////////////////
//typedef fixed_num<dword>    TSysOrdID;   //每個交易平台,最多4億筆委託! 改放在 TwStkOrder.h
//---------------------------------------------------------------------------
TWSTK_class TSysOrdsEvHandler;    //當委託資料有 [易動(含成交)、新增] 時的事件
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TUpdFrom;
   enum TUpdFrom_t
#else
   enum TUpdFrom
#endif
{
         uf_RptOrd,
         uf_RptDeal,
         uf_Client,
         uf_OrdFlow,
         uf_ViewArea,
} K_EnumPacked;
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TSysOrds : public ev_ary<TSysOrdID,TSysOrd,
                                 index_maker<TSysOrdID>,
                                 Kstl::pod_vector<TSysOrd,pod_allocator_TSysOrd>
                                 >
                     , public TKeyObject
{
   typedef ev_ary<TSysOrdID,TSysOrd,index_maker<TSysOrdID>,Kstl::pod_vector<TSysOrd,pod_allocator_TSysOrd> >   inherited;
   typedef inherited::updater               inherited_updater;
   //typedef ev_ary<TSysOrdID,TSysOrd>      inherited;
   typedef inherited::updater::impl_type    updater_impl_base;
   typedef TSysOrds                         this_type;
   K_mf(this_type&) operator= (const this_type&); //TSysOrds 不適用直接 assign()
   K_mf(this_type&) assign    (const this_type&); //這三個 members 沒有 implement
   K_ctor           TSysOrds  (const this_type&); //而且是放在 private
public:
   inline K_ctor    TSysOrds  ()  : RecoverTimer_(this) {}
   TWSTK_class updater;
   TWSTK_class TUpdaterImpl : public updater_impl_base {
      typedef updater_impl_base           inherited;
      typedef TStkDeals::const_iterator   TStkDeali;
      using inherited::flush_update;
      TUpdFrom    UpdFrom_;
      TSysOrds*   SysOrds_;
      TSysOrd     BfOrd_;
      TSysOrd     Ord_;
      TStkDeali   LastDeal_;//當UpdFrom_==uf_RptDeal時才有效,否則無定義!
      enum {
         isNewOrd,
         isIterOK,
      }  State_;

      friend class updater;
      inline K_mf(bool)  IsNewOrd  () const  { return State_==isNewOrd; }
      inline K_mf(bool)  IsIterOK  () const  { return State_==isIterOK; }
   public:
      K_ctor           TUpdaterImpl    (TUpdFrom, TSysOrds&, const TSysOrds::iterator&);
      K_dtor          ~TUpdaterImpl    ();   //通知 TSysOrds 產生 OnOrdAfEndUpdate() 事件
      K_mf(bool)       is_iterator_eq  (const ev_iterator& i) const;
      K_mf(bool)       bf_end_update   (bool forFlush);
      K_mf(reference)  get_modifyer    ();
      K_mf(const TStkOrder*) GetConst  () const;
      K_mf(const TStkOrder*) GetBfConst() const;

      inline K_mf(TUpdFrom)   GetUpdFrom  () const  { return UpdFrom_;  }
      inline K_mf(TSysOrds&)  GetSysOrds  () const  { return *SysOrds_; }
      //若為新增委託 GetBfOrd().get()==0
      inline K_mf(const TSysOrd&)    GetBfOrd    () const  { return BfOrd_;    }
      inline K_mf(const TSysOrd&)    GetAfOrd    () const  { return Ord_;      }
      //當 GetUpdFrom()==uf_RptDeal, GetLastDeal()才有意義
      inline K_mf(const TStkDeali&)  GetLastDeal () const  { return LastDeal_; }
             K_mf(TSysOrdID)         GetSysOrdID () const;
   }; // class TUpdaterImpl

   TWSTK_class updater : public inherited_updater {
      typedef inherited_updater  inherited;
      typedef updater            this_type;
      using inherited::flush_update;
   public:
      K_ctor  updater  (TUpdaterImpl* = 0);
      K_ctor  updater  (const this_type&);

      inline K_mf(const TUpdaterImpl*) GetImpl    () const  { return static_cast<const TUpdaterImpl*>(get_impl()); }
      inline K_mf(const TStkOrder*)    GetBfConst () const  { return GetImpl()->GetBfConst();                      }
      inline K_mf(const TStkOrder*)    GetConst   () const  { return GetImpl()->GetConst();                        }
      inline K_mf(TStkOrder&)          GetModify  ()        { return *(get_impl()->get_modifyer());                }
      inline K_mf(TSysOrdID)           GetSysOrdID() const  { return GetImpl()->GetSysOrdID();                     }

      K_mf(this_type&)  operator=  (const this_type&);
      // updater提供一些額外的服務:
      // AddDeal()傳回false表示重複的成交回報,或沒有對應的委託
      K_mf(bool)  AddDeal  (TStkDeal::TMSeqNo,  //市場序號
                            TStkDeal::TBSeqNo,  //券商序號
                            TKTime,             //撮合時間
                            TStkPri,            //成交價
                            TStkQty);           //成交股數(最小交易單位)
      K_mf(bool)  FlushUpdate  (TUpdFrom);
   }; // class updater

          K_mf(updater)  begin_update  (const iterator&, TUpdFrom = uf_ViewArea);
   inline K_mf(updater)  begin_insert  (TUpdFrom uf)  { return begin_update(end(),uf); }

   K_mf(void)  AddEvHandler  (TSysOrdsEvHandler*);
   K_mf(void)  DelEvHandler  (TSysOrdsEvHandler*);
private:
   K_mf(TUpdaterImpl*)  updater_factory  (const ev_iterator&);

   friend class TUpdaterImpl;
   K_mf(void)  OnOrdAfEndUpdate  (TUpdaterImpl&);

   typedef Kstl::pod_vector<TSysOrdsEvHandler*>    TEvHandlers;
   typedef TEvHandlers::iterator                   TEvHandleri;
   TEvHandlers    evs_;
   TEvHandlers    Recovers_;

   K_mf(void)  TimerEv  (TTimerBase*);
   TTimer<this_type> RecoverTimer_;
};
//---------------------------------------------------------------------------
inline TSysOrdID  GetSysOrdID  (const TSysOrds::iterator& i)   { return TSysOrdID(i.get_idx()); }
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TSysOrdsEvHandler
{
public:
   enum TEvResult {
      er_DelEvHandler,
      er_Continue,
   };
private:
   friend class TSysOrds;
   //↓930406:用傳回值決定是否繼續通知,或取消註冊
   //         禁止在事件通知時使用 DelEvHandler() 來取消註冊
   virtual K_mf(TEvResult)  OnOrdAfEndUpdate  (TSysOrds::TUpdaterImpl&) = 0;
   //若 OnOrdRecover() 傳回 er_DelEvHandler
   //則不會再收到 OnOrdRecover() 及 OnOrdAfEndUpdate()
   virtual K_mf(TEvResult)  OnOrdRecover      (TSysOrdID, const TStkOrder*);
   TSysOrdID   NextRecoverID_;
};
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TTseOrds
{  //以[券商分公司]的觀點來看委託資料
   typedef TTseOrds  this_type;
public:
   //分公司[交易所委託書號]明細:
   // Types.
   enum TOrdTabID
   {
      oti_Tse,          //今日上市一般(整股委託+成交、定價委託+成交、零股委託)
      oti_Otc,          //    上櫃
      oti_OtcR,         //今日興櫃委託+成交
      oti_End
      //oti_TseYDayDeal,  //昨日上市成交 YDay = Yesterday = T-1日 (T-1日委託,T日成交回報)
      //oti_OtcYDayDeal,  //
   };
   typedef TTseOrdRecsDefine<TSysOrdID>               TOrdRecsDefine;   //TwStkOrdDefine.h
   typedef TOrdRecsDefine::TEvRecs                    TEvRecs;
   typedef wise_container<TOrdRecsDefine::TEvRecs>    TOrdRecs;
   typedef TOrdRecsDefine::TEvRecs::iterator          TOrdReci;
   typedef TOrdRecsDefine::TEvRecs::value_type        TOrdRecv;
   typedef TOrdRecsDefine::TEvRecs::updater           TOrdRecu;

   // Data members.
   mutable TOrdRecs     Recs_[oti_End];
   static const char*   DetailDesp[oti_End];

   // Member functions.
          K_mf(void)       swap      (this_type&);
   inline K_mf(TOrdRecs&)  GetRecs   (TMarket m, TStkQty dealQty) const  { return Recs_[GetTabID(m,dealQty)]; }
   static K_mf(TOrdTabID)  GetTabID  (TMarket,   TStkQty dealQty);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
