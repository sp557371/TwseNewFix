//
// TwStk 的 [委託資料處理中心]
//
// 版本: $Id: TwStkOrdCenter.h,v 1.9 2005/06/04 01:26:53 fonwin Exp $
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
/* TOrdCenter 委託資料中心!
      0.負責管理系統委託書資料表
      1.負責委託異動時資料的一致及正確性
      2.負責[委託.成交]回報的更新:
         您只要將 [委託回報] 的資料填入 TOrdCenter::TRpt::TOrd
                  [成交回報] 的資料填入 TOrdCenter::TRpt::TDeal
         然後呼叫 RptOrd() 或 RptDeal(), 您可使用傳回值 TOrdHnd 繼續其他欄位的更新
      3.負責維護[交易所委託書號]與[系統序號]的對應
*/
TWSTK_class TOrdCenter;
/////////////////////////////////////////////////////////////////////////////
struct TOnNewTseOrdKey {
   // 當有新的[委託書號]對應到SysOrdID時,會由 OrdCenter 產生此事件.
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
   //建構
   explicit K_ctor  TOrdCenter  (const std::string& logPath, const THostID&);
   virtual  K_dtor ~TOrdCenter  ();

   //Types.
   typedef Kway::Tw::Stk::TSysOrd         TSysOrd; // TwStkSysOrd.h
   typedef Kway::Tw::Stk::TSysOrds        TSysOrds;
   typedef TSysOrds::updater              TOrdHnd; // 委託Handle物件-負責處理[一筆委託]的相關工作
   typedef TBrokerRecsDefine<TTseOrds>    TBrokerOrdsDefine;   // TwStkBroker.h
   typedef TBrokerOrdsDefine::TEvRecs     TBrokerOrds;

   // [委託/成交]回報處理物件 (必須已經編製了[委託書號]的委託,才可進行回報作業)
   struct TRpt {
      struct TBase { //[委託/成交]共用的部分
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
      struct TOrd : public TBase {//委託回報
         TStkQty              BfQty_;  //不含成交
         TStkQty              AfQty_;  //不含成交
         TSaleNo              SaleNo_;
         TSfGroup             SfGroup_;
         TOrdCmdID            OCmd_;
      };
      struct TDeal : public TBase {//成交回報
         TStkQty              Qty_;
         TStkDeal::TBSeqNo    BSeqNo_;
         TStkDeal::TMSeqNo    MSeqNo_;
         TKTimeMS             TimeMS_;
      };
   };
   K_mf(TOrdHnd)  RptOrd   (const TRpt::TOrd&,  TOrdFlowID = TOrdFlowID(0,of_Finish),
                                                TSysOrdID = TSysOrdID());
   K_mf(TOrdHnd)  RptDeal  (const TRpt::TDeal&, TSysOrdID = TSysOrdID(), bool autoFlushUpdate = true);

   //用各種不同的觀點取得委託資料表
   // 1.內部收單順序(TSysOrdID + TOrd)
   inline K_mf(TSysOrds&)     GetSysOrds  ()       { return SysOrds_;    }
   // 2.交易所委託序號(TBrokerID + TTseOrdID + TOrd)
   inline K_mf(TBrokerOrds&)  GetBrokerOrds  ()    { return BrokerOrds_; }

   //用TseOrdKey取出一筆委託,使用前請先判斷 OrdHnd 是否有效: hnd.is_invalid()
   //若 uf==uf_RptDeal, 則您必須在 TOrdHnd 死掉前, 新增一筆成交明細,
   //                   否則會造成 OnOrdAfEndUpdate() 事件, 誤判 GetLastDeal().
   //                   如果您沒辦法保證在 TOrdHnd 死前, 新增一筆成交明細,
   //                   則請改其他的 TUpdFrom(例如: uf_Client, uf_OrdFlow...)
   //                   或,如果您並不一定會異動(只是做些判斷處理),則應該先用GetOrd(),
   //                      等確定要異動時,再使用GetOrdHnd(),或直接使用RptDeal().
   K_mf(TOrdHnd)  GetOrdHnd  (const TTseOrdKey&, TStkQty dealQty, TUpdFrom uf, TSysOrdID);
   K_mf(TOrdHnd)  GetOrdHnd  (TSysOrdID, TUpdFrom);

   K_mf(bool)  GetOrd  (const TSysOrdID&,  TSysOrds::iterator&);
   K_mf(bool)  GetOrd  (const TTseOrdKey&, TSysOrds::iterator&, TStkQty dealQty);

   K_mf(bool)  AddOnNewTseOrdKey  (TOnNewTseOrdKey* a)   { return OnNewTseOrdKeyEvs_.add(a); }
   K_mf(bool)  DelOnNewTseOrdKey  (TOnNewTseOrdKey* a)   { return OnNewTseOrdKeyEvs_.del(a); }

   inline K_mf(THostID)  GetHostID  () const { return HostID_; }

   //儲存到inn的大小.
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
