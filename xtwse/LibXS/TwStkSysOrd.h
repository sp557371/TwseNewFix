//
// TwStk �� [�e�U�Ѹ�ƪ�]
//
// ����: $Id: TwStkSysOrd.h,v 1.9 2006/04/04 03:30:55 fonwin Exp $
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
      static const size_t  reserve_elements = 1024*10;   //�C��O���魫�s���t,�ݭn�w�d���Ŷ�
   };
} } }
//---------------------------------------------------------------------------
namespace Kstl {
   // gcc ���u��� extern template ��b�P ev_ary<> �ۦP�� namespace �̭�!
   // vc  �� TSysOrds �O DLL export, �� TSysOrds �~�Ӧ� ev_ary<>, �ҥH ev_ary<> �]�n�O DLL export
   //     �ӥB�����n�b�w�q TSysOrds ���e, �N���i��!
   // �ҥH���U�o��N������b�o��, �� TSysOrd, pod_allocator_TSysOrd �S�������w�q�n(�ҥH������b�W��)
   // �̫�, �N�ܦ��{�b�o����ˤF :(
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
//typedef fixed_num<dword>    TSysOrdID;   //�C�ӥ�����x,�̦h4�����e�U! ���b TwStkOrder.h
//---------------------------------------------------------------------------
TWSTK_class TSysOrdsEvHandler;    //��e�U��Ʀ� [����(�t����)�B�s�W] �ɪ��ƥ�
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
   K_mf(this_type&) operator= (const this_type&); //TSysOrds ���A�Ϊ��� assign()
   K_mf(this_type&) assign    (const this_type&); //�o�T�� members �S�� implement
   K_ctor           TSysOrds  (const this_type&); //�ӥB�O��b private
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
      TStkDeali   LastDeal_;//��UpdFrom_==uf_RptDeal�ɤ~����,�_�h�L�w�q!
      enum {
         isNewOrd,
         isIterOK,
      }  State_;

      friend class updater;
      inline K_mf(bool)  IsNewOrd  () const  { return State_==isNewOrd; }
      inline K_mf(bool)  IsIterOK  () const  { return State_==isIterOK; }
   public:
      K_ctor           TUpdaterImpl    (TUpdFrom, TSysOrds&, const TSysOrds::iterator&);
      K_dtor          ~TUpdaterImpl    ();   //�q�� TSysOrds ���� OnOrdAfEndUpdate() �ƥ�
      K_mf(bool)       is_iterator_eq  (const ev_iterator& i) const;
      K_mf(bool)       bf_end_update   (bool forFlush);
      K_mf(reference)  get_modifyer    ();
      K_mf(const TStkOrder*) GetConst  () const;
      K_mf(const TStkOrder*) GetBfConst() const;

      inline K_mf(TUpdFrom)   GetUpdFrom  () const  { return UpdFrom_;  }
      inline K_mf(TSysOrds&)  GetSysOrds  () const  { return *SysOrds_; }
      //�Y���s�W�e�U GetBfOrd().get()==0
      inline K_mf(const TSysOrd&)    GetBfOrd    () const  { return BfOrd_;    }
      inline K_mf(const TSysOrd&)    GetAfOrd    () const  { return Ord_;      }
      //�� GetUpdFrom()==uf_RptDeal, GetLastDeal()�~���N�q
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
      // updater���Ѥ@���B�~���A��:
      // AddDeal()�Ǧ^false��ܭ��ƪ�����^��,�ΨS���������e�U
      K_mf(bool)  AddDeal  (TStkDeal::TMSeqNo,  //�����Ǹ�
                            TStkDeal::TBSeqNo,  //��ӧǸ�
                            TKTime,             //���X�ɶ�
                            TStkPri,            //�����
                            TStkQty);           //����Ѽ�(�̤p������)
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
   //��930406:�ζǦ^�ȨM�w�O�_�~��q��,�Ψ������U
   //         �T��b�ƥ�q���ɨϥ� DelEvHandler() �Ө������U
   virtual K_mf(TEvResult)  OnOrdAfEndUpdate  (TSysOrds::TUpdaterImpl&) = 0;
   //�Y OnOrdRecover() �Ǧ^ er_DelEvHandler
   //�h���|�A���� OnOrdRecover() �� OnOrdAfEndUpdate()
   virtual K_mf(TEvResult)  OnOrdRecover      (TSysOrdID, const TStkOrder*);
   TSysOrdID   NextRecoverID_;
};
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TTseOrds
{  //�H[��Ӥ����q]���[�I�Ӭݩe�U���
   typedef TTseOrds  this_type;
public:
   //�����q[����ҩe�U�Ѹ�]����:
   // Types.
   enum TOrdTabID
   {
      oti_Tse,          //����W���@��(��ѩe�U+����B�w���e�U+����B�s�ѩe�U)
      oti_Otc,          //    �W�d
      oti_OtcR,         //���鿳�d�e�U+����
      oti_End
      //oti_TseYDayDeal,  //�Q��W������ YDay = Yesterday = T-1�� (T-1��e�U,T�馨��^��)
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
