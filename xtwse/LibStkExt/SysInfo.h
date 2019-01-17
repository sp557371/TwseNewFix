//---------------------------------------------------------------------------
#ifndef SysInfoH
#define SysInfoH
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
#include "stdSettingP.h"
#include "ScheduleMgr.h"
#include "TwStkOrdCenter.h"
#include "StdRecorder.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kstl;
//---------------------------------------------------------------------------
struct SSysInfo{
   char_ary<char,40,' '>    Item_;
   char_ary<char,20,' '>    State_;
   char_ary<char,40,' '>    Memo_;
};
//---------------------------------------------------------------------------
typedef char_ary<char,3,' '>             TSysKey;
typedef Kstl::ev_map<TSysKey, SSysInfo>  TSysInfo;
typedef TStdRecorder<TSysInfo, SSysInfo> TSysInfoRecorder;
//---------------------------------------------------------------------------
enum eStateChg{
   eMarketPh,
   eTseLine,
   eOtcLine,
   eOtcRLine,
   eNormalOpen,
   eNormalClose,
   eFixedOpen,
   eFixedClose,
   eOddOpen,
   eOddClose,
   eForceOpen,
};
//---------------------------------------------------------------------------
K_class TSysEvHnd
{
public:
   virtual K_mf(void) Alert(eStateChg) = 0;
};
//---------------------------------------------------------------------------
struct TSysFieldMaker
{
   typedef TSysInfo                 TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;

   K_mf(const Kway::TFieldDesp&)  GetFieldDesp  (field_n_type n) const
          {
            static Kway::TFieldDesp f[] = {
                     { "項次",      6, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Fixed,  },
                     { "項目",     30, Kway::ta_xCenter_yCenter, Kway::ta_xLeft_yCenter  , Kway::tfs_Null,   },
                     { "狀態",     16, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Null,},
                     { "備註",     30, Kway::ta_xCenter_yCenter, Kway::ta_xLeft_yCenter,   Kway::tfs_CanEdit,}};
            return f[n];
          }
   inline K_mf(field_n_type)  GetFieldCount () const    { return 4; }

   K_mf(std::string)  Get  (const iterator& i, field_n_type fieldNo) const
         {
            char  buf[128];
            switch(fieldNo) {
            case 0:  return i->first.as_string();
            case 1:  return i->second.Item_.as_string();
            case 2:  return i->second.State_.as_string();
            case 3:  return i->second.Memo_.as_string();
            }
            return std::string();
         }
   K_mf(void)  Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str)
         {
            if(fieldNo ==0 )
               return;
            TEvContainer::updater   u = c.begin_update(iter);
            switch(fieldNo) {
            case 3:u->Memo_.assign(str);     break;
            default:break;
            }
         }
}; // class TSampleFieldMaker
//---------------------------------------------------------------------------
typedef TTViewArea<TSysInfo, TSysFieldMaker> TSysInfoVA;
//---------------------------------------------------------------------------
K_class TSysInfoPage :public TVACreater
{
private:
   typedef TSysInfo::iterator    TSysi;
   typedef TSysInfo::updater     TSysu;
   typedef TSysInfoPage          this_type;
   typedef TScheduleObj<this_type> TSchedObj;
   TSysInfo                      SysInfo_;
//   TSysInfoRecorder              Recorder_;
   bool				 UseMainPath_;
   bool				 UseSecPath_;
   bool                          RecLdckLdrk_;
   bool                          RecClOrdID_;
   Kway::TKDate                  Date_;
   TTrxSegment                   MarketPh_;
   TTrxSegment                   TrMarketPh_;
   TSchedObj*                    SchNormalOpen_;
   TSchedObj*                    SchNormalClose_;
   TSchedObj*                    SchFixedOpen_;
   TSchedObj*                    SchFixedClose_;
   TSchedObj*                    SchOddOpen_;
   TSchedObj*                    SchOddClose_;
   TSchedObj*                    ForceOpen_;
   Kway::word                    TseReady_;
   Kway::word                    OtcReady_;
   Kway::word                    OtcRReady_;
   Kway::word                    TseOddReady_;
   Kway::word                    OtcOddReady_;
   Kway::word                    OtcROddReady_;
   std::vector<TSysEvHnd*>       HndList_;
   TScheduleMgr*                 ScheduleMgr_;
   TDesp                         MainPath_;
   TDesp                         SecPath_;
   bool                          UseIvacNo_;
   bool                          UseBrkID_;
   TDesp                         IvacNo_;
   TDesp                         BrkID_;
   K_mf(void)                    Reg(int,const char*, const char*);
   K_mf(void)                    UpdateState(int,const char*);
   K_mf(void)                    AlertAll(eStateChg);
public:
//   TSettingPage*   		 SysParam_;
   K_ctor                        TSysInfoPage(TScheduleMgr*, TOrdCenter*);
   K_dtor                        ~TSysInfoPage();
   K_mf(TViewArea*)              GetVA();
   K_mf(TaViewArea)              CreateNewVA();
   K_mf(void)                    SetMarketPh(ETrxSegment m);
   K_mf(TTrxSegment)             GetMarketPh(){return MarketPh_;}
   K_mf(TTrxSegment)             GetTrMarketPh(){return TrMarketPh_;}
   K_mf(TTrxSegment)             GetTrxSegment(){return MarketPh_;}
   K_mf(void)                    SetLineState(TMarket,Kway::word,ETradeKind tk);
   K_mf(bool)                    GetLineState(TMarket,TTrxSegment ts=ts_Full);
   K_mf(bool)                    Add(TSysEvHnd*);
   K_mf(bool)                    Del(TSysEvHnd*);
   K_mf(void)                    NormalOpen (TKMemo&);
   K_mf(void)                    NormalClose(TKMemo&);
   K_mf(void)                    FixedOpen  (TKMemo&);
   K_mf(void)                    FixedClose (TKMemo&);
   K_mf(void)                    OddOpen    (TKMemo&);
   K_mf(void)                    OddClose   (TKMemo&);
   K_mf(void)                    ForceOpen  (TKMemo&);
   K_mf(bool)                    IsUseMainPath(){return UseMainPath_;}
   K_mf(bool)                    IsUseSecPath(){return UseSecPath_;}
   K_mf(bool)                    IsRecLdckLdrk(){return RecLdckLdrk_;}
   K_mf(bool)                    IsRecClOrdID(){return RecClOrdID_;}
   K_mf(TDesp)                   GetMainPath(){return MainPath_;}
   K_mf(TDesp)                   GetSecPath(){return SecPath_;}
   K_mf(TIvacKey)                GetErrAccount(){return TIvacKey(TBrokerID(BrkID_.as_string()),
                                                                 TIvacNo(IvacNo_.as_string()));}
};
//---------------------------------------------------------------------------
#endif

