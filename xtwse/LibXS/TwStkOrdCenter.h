//
// TwStk �� [�e�U��ƳB�z����]
//
// ����: $Id: TwStkOrdCenter.h,v 1.9 2005/06/04 01:26:53 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkOrdCenterH
#define TwStkOrdCenterH
//---------------------------------------------------------------------------
#include "recorder.h"
#include "inn_file.h"
//---------------------------------------------------------------------------
#include "TwStkSysOrd.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
using namespace Kstl;
//---------------------------------------------------------------------------
/* TOrdCenter �e�U��Ƥ���!
      0.�t�d�޲z�t�Ωe�U�Ѹ�ƪ�
      1.�t�d�e�U���ʮɸ�ƪ��@�P�Υ��T��
      2.�t�d[�e�U.����]�^������s:
         �z�u�n�N [�e�U�^��] ����ƶ�J TOrdCenter::TRpt::TOrd
                  [����^��] ����ƶ�J TOrdCenter::TRpt::TDeal
         �M��I�s RptOrd() �� RptDeal(), �z�i�ϥζǦ^�� TOrdHnd �~���L��쪺��s
      3.�t�d���@[����ҩe�U�Ѹ�]�P[�t�ΧǸ�]������
*/
TWSTK_class TOrdCenter;
/////////////////////////////////////////////////////////////////////////////
struct TOnNewTseOrdKey {
   // ���s��[�e�U�Ѹ�]������SysOrdID��,�|�� OrdCenter ���ͦ��ƥ�.
   struct TArg {
      inline K_ctor  TArg  ( TOrdCenter&       OrdCenter
                           , const TTseOrdKey& TseOrdKey
                           , TSysOrdID         SysOrdID
                           , const TStkOrder&  StkOrd)
               : OrdCenter_(OrdCenter)
               , TseOrdKey_(TseOrdKey)
               , SysOrdID_ (SysOrdID)
               , StkOrd_   (StkOrd)
               { }

      TOrdCenter&       OrdCenter_;
      const TTseOrdKey& TseOrdKey_;
      TSysOrdID         SysOrdID_;
      const TStkOrder&  StkOrd_;
   };
private:
   virtual K_mf(ev_result)  OnNewTseOrdKey  (const TArg&) = 0;

   friend inline K_mf(ev_result)  CallEv  ( TOnNewTseOrdKey* pThis
                                          , const TArg&      arg)
        { return pThis->OnNewTseOrdKey(arg); }
};
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TOrdCenter : private TSysOrdsEvHandler
{
public:
   //�غc
   explicit K_ctor  TOrdCenter  (const std::string& logPath, const THostID&);
   virtual  K_dtor ~TOrdCenter  ();

   //Types.
   typedef Kway::Tw::Stk::TSysOrd         TSysOrd; // TwStkSysOrd.h
   typedef Kway::Tw::Stk::TSysOrds        TSysOrds;
   typedef TSysOrds::updater              TOrdHnd; // �e�UHandle����-�t�d�B�z[�@���e�U]�������u�@
   typedef TBrokerRecsDefine<TTseOrds>    TBrokerOrdsDefine;   // TwStkBroker.h
   typedef TBrokerOrdsDefine::TEvRecs     TBrokerOrds;

   // [�e�U/����]�^���B�z���� (�����w�g�s�s�F[�e�U�Ѹ�]���e�U,�~�i�i��^���@�~)
   struct TRpt {
      struct TBase { //[�e�U/����]�@�Ϊ�����
         TTseOrdKey  TseOrdKey_; //BrokerID_, Market_, TseOrdID_
         TIvacNo     IvacNo_;
         TBSCode     BSCode_;
         TStkNo      StkNo_;
         TStkPri     Pri_;
         TOType      OType_;
         TKTime      TseTime_;
         TTrxSegment TrxSegment_;//2008/01/11

         inline K_ctor TBase() : TrxSegment_(ts_Early) {
         }
         K_mf(bool)  IsEqual  (const TStkOrder&) const;
      };
      struct TOrd : public TBase {//�e�U�^��
         TStkQty              BfQty_;  //���t����
         TStkQty              AfQty_;  //���t����
         TSaleNo              SaleNo_;
         TSfGroup             SfGroup_;
         TOrdCmdID            OCmd_;
      };
      struct TDeal : public TBase {//����^��
         TStkQty              Qty_;
         TStkDeal::TBSeqNo    BSeqNo_;
         TStkDeal::TMSeqNo    MSeqNo_;
         TKTimeMS             TimeMS_;
      };
   };
   K_mf(TOrdHnd)  RptOrd   (const TRpt::TOrd&,  TOrdFlowID = TOrdFlowID(0,of_Finish),
                                                TSysOrdID = TSysOrdID());
   K_mf(TOrdHnd)  RptDeal  (const TRpt::TDeal&, TSysOrdID = TSysOrdID(), bool autoFlushUpdate = true);

   //�ΦU�ؤ��P���[�I���o�e�U��ƪ�
   // 1.�������涶��(TSysOrdID + TOrd)
   inline K_mf(TSysOrds&)     GetSysOrds  ()       { return SysOrds_;    }
   // 2.����ҩe�U�Ǹ�(TBrokerID + TTseOrdID + TOrd)
   inline K_mf(TBrokerOrds&)  GetBrokerOrds  ()    { return BrokerOrds_; }

   //��TseOrdKey���X�@���e�U,�ϥΫe�Х��P�_ OrdHnd �O�_����: hnd.is_invalid()
   //�Y uf==uf_RptDeal, �h�z�����b TOrdHnd �����e, �s�W�@���������,
   //                   �_�h�|�y�� OnOrdAfEndUpdate() �ƥ�, �~�P GetLastDeal().
   //                   �p�G�z�S��k�O�Ҧb TOrdHnd ���e, �s�W�@���������,
   //                   �h�Ч��L�� TUpdFrom(�Ҧp: uf_Client, uf_OrdFlow...)
   //                   ��,�p�G�z�ä��@�w�|����(�u�O���ǧP�_�B�z),�h���ӥ���GetOrd(),
   //                      ���T�w�n���ʮ�,�A�ϥ�GetOrdHnd(),�Ϊ����ϥ�RptDeal().
   K_mf(TOrdHnd)  GetOrdHnd  (const TTseOrdKey&, TStkQty dealQty, TUpdFrom uf, TSysOrdID);
   K_mf(TOrdHnd)  GetOrdHnd  (TSysOrdID, TUpdFrom);

   K_mf(bool)  GetOrd  (const TSysOrdID&,  TSysOrds::iterator&);
   K_mf(bool)  GetOrd  (const TTseOrdKey&, TSysOrds::iterator&, TStkQty dealQty);

   K_mf(bool)  AddOnNewTseOrdKey  (TOnNewTseOrdKey* a)   { return OnNewTseOrdKeyEvs_.add(a); }
   K_mf(bool)  DelOnNewTseOrdKey  (TOnNewTseOrdKey* a)   { return OnNewTseOrdKeyEvs_.del(a); }

   inline K_mf(THostID)  GetHostID  () const { return HostID_; }

   //�x�s��inn���j�p.
   static const int   SysOrdRecSz = sizeof(TStkOrder_Rec);

protected:
   K_mf(void)  SetOrdMap  (TSysOrdID, const TStkOrder*);

private:
   K_mf(TEvResult)  OnOrdAfEndUpdate  (TSysOrds::TUpdaterImpl&);

   THostID     HostID_;

   typedef rr_inn_file<uint64,word> inn_type;
   inn_type    SysOrdsInn_;

   class TSysOrdsRecorder : public recorder<TSysOrds,inn_type> {
      K_mf(bool)    into           (inn_type&, TSysOrdID, const TStkOrder*);
      K_mf(bool)    into_new_room  (inn_type&, const TSysOrds::const_iterator&);
      K_mf(bool)    into_room      (inn_type&, const TSysOrds::iterator&);
      K_mf(rr_key)  get_room_key   (inn_type&, const TSysOrds::iterator&);
   public:
      inline K_ctor  TSysOrdsRecorder  (inn_type* inn)  : recorder<TSysOrds,inn_type>(inn) {}
   };
   TSysOrdsRecorder   SysOrdsRecorder_;

   TSysOrds       SysOrds_;
   TBrokerOrds    BrokerOrds_;
   TFile          DealsFile_;

   typedef Kstl::ev_handlers<TOnNewTseOrdKey>   TOnNewTseOrdKeyEvs;
   TOnNewTseOrdKeyEvs   OnNewTseOrdKeyEvs_;
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
struct TOrdCenter_StkDeal_FileRec {
      TSysOrdID               SysOrdID_;
      TStkDeals::key_type     Key_;
      TStkDeals::mapped_type  Data_;
};
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
