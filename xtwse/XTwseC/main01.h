//========================================================================
//
// main01.h.cpp
//
//========================================================================
#ifndef main01H
#define main01H
//========================================================================
#include "OrdSesMgr.h"
#include "OrdScheduler.h"
#include "TwStkDealVA.h"
#include "TwStkBrokerVA.h"
#include "ExcMgr.h"
#include "MsgCenter.h"
#include "SysInfo.h"
//#include "OrderNoMgr.h"
#include "bxSesMgr.h"
#include "TwseIniFile.h"
//========================================================================
using namespace Kway;
//========================================================================
using namespace Kway::Tw::Stk;
//========================================================================
extern TFieldDesp OrdFieldDesp[31]; //See main01.cpp
extern TFieldDesp OrdSysIDDesp;     //See main01.cpp
extern TFieldDesp OrdFieldDesp2[3]; //See main01.cpp
//========================================================================
struct TOrdFieldMaker
{
   typedef size_t field_n_type;

   K_mf(std::string) Get(const TStkOrder* i, field_n_type fieldNo) const
   {
      if(i==0) return std::string();
      switch(fieldNo)
      {
         case 0:  return i->IvacKey_.GetBrokerID().as_string();
         case 1:  return i->TseOrdID_.as_string();
         case 2:  return i->IvacKey_.GetIvacNo().as_string();
         case 3:  return i->StkNo_.as_string();
         case 4:  return i->Pri_.as_string();
         case 5:  return i->Qty_.as_string(1,0,1);
         case 6:  return i->BSCode_.as_string();
         case 7:  return i->OType_.as_string();
         case 8:  return i->TrxSegment_.as_string(true);
         case 9:  return std::string();
         case 10: return i->OrigSource_.UserID_.as_string();
         case 11: return i->OrigSource_.Time_.as_string();
         case 12: return i->OrigSource_.ClOrdID_.as_string();
         case 13: return i->TseOrigTime_.as_string();
         case 14: return i->TseLastTime_.as_string();
         case 15: return i->BfQty_.as_string(1,0,1);
         case 16: return i->AfQty_.as_string(1,0,1);
         case 17: return i->MatchQty_.as_string(1,0,1);
         case 18: return i->DealQty_.as_string(1,0,1);
//         case 18: return i->DealAmt_.as_string(1,2,1);
         case 19: return i->LastMsg_.as_string();
         case 20: return i->LastOCmd_.as_string(true);
         case 21:
            {
               char buf[10];
               int  val = (int)i->FlowID_.GetFlow();
               sprintf(buf, "%d", val);
               return std::string(buf);
            }
         
         case 22:
            {
               char buf[10];
               int  val = (int)i->LastTseCode_;
               sprintf(buf, "%d", val);
               return std::string(buf);
            }
         // TwseIvacnoFlag
         case 23:
            {
               return i->TseOrderLine_.as_string();
            }
            
         case 24:
            {
               char buf[10];
               int  val = (int)i->WrntMktMkr_;
               sprintf(buf, "%d", val);
               return std::string(buf);
            }
            
         case 25:  return i->SendingTime_.as_string(TKDate::sf_ELong);
         case 26:  return i->LastOkOCmd_ .as_string(true);
         // TwseExCode
         case 27:  return i->SfGroup_    .as_string();
         // TMPAfQty
         case 28:  return i->TMPAfQty_.as_string(1,0,1);
         // TMPIvacNo
         case 29:  return i->TMPIvacNo_.as_string();
         // SubAcc_[0]
         case 30:
            {
               char buf[10];
               buf[0] = i->SubAcc_[0];
               return std::string(buf, 1);
            }
         
         default: return std::string();
       }
   }
   //---------------------------------------------------------------------
   K_mf(void) Put(TStkOrder& u, field_n_type fieldNo, const std::string& str)
   {
      // debug {
      //return;
      switch (fieldNo) {
         case  4:  u.Pri_                .assign(str);  break;
         case 10:  u.OrigSource_.UserID_ .assign(str);  break;
         case 12:  u.OrigSource_.ClOrdID_.assign(str);  break;
         case 21:  u.FlowID_.SetFlow((EOrdFlowID)(atoi(str.c_str())));  break;
      }
      // } debug
   }
};
//========================================================================
class TSysOrdFieldMaker : public TOrdFieldMaker
{
   typedef TOrdFieldMaker  inherited;
public:
   typedef TOrdCenter::TSysOrds   TEvContainer;
   typedef TEvContainer::iterator iterator;
   //---------------------------------------------------------------------
   K_mf(const TFieldDesp&) GetFieldDesp(field_n_type n) const
   {
      return(n==0 ? OrdSysIDDesp : OrdFieldDesp[n-1]);
   }
   //---------------------------------------------------------------------
   K_mf(field_n_type) GetFieldCount() const
   {
      return numofele(OrdFieldDesp)+1;
   }
   //---------------------------------------------------------------------
   K_mf(std::string) Get(const iterator& i, field_n_type fieldNo) const
   {
      return(fieldNo==0 ? i.get_idx().as_string() : inherited::Get(i->get(),fieldNo-1));
   }
   //---------------------------------------------------------------------
   K_mf(void) Put(TEvContainer& evc, iterator& i, field_n_type fieldNo, const std::string& str)
   {
      if(fieldNo > 0)
         inherited::Put(**evc.begin_update(i),fieldNo-1,str);
   }
};
//========================================================================
struct TStkDealDetailMaker
{
   //明細表:[成交明細]
   typedef TOrdCenter::TSysOrds::iterator TMasterIterator;
   //---------------------------------------------------------------------
   K_mf(TStkDeals*) operator() (const TMasterIterator& mi, bool isCreateNeeded)
   {
      return isCreateNeeded ? (*mi)->Deals_.testnew() : (*mi)->Deals_.get();
   }
};
//========================================================================
struct TSysOrdVACreater : public TVACreater
{
 typedef TOrdCenter::TSysOrds            TEvRecs;
 typedef TSysOrdFieldMaker               TFieldMaker;
 typedef TTViewArea<TEvRecs,TFieldMaker> TMasterVA;
 typedef TMasterVA::cursor_type          TMasterCursor;

    //---------------------------------------------------------------------
    K_ctor TSysOrdVACreater(TEvRecs& recs):Recs_(&recs){}
    K_mf(TaViewArea) CreateNewVA(){ return TaViewArea(new TMasterVA(Recs_)); }

 protected:
    TEvRecs    *Recs_;
    TStkDealVACrDetail<TMasterVA,TStkDealDetailMaker>  StkDealVACr_;
    //---------------------------------------------------------------------
    K_mf(size_t) GetDetails(const char**& a)
    {
       static const char* desp[] = { "成交明細" };
       a = desp;
       return numofele(desp);
    }
    //---------------------------------------------------------------------
    K_mf(TVACreater*) GetDetailVACreater(size_t detailID)
    {
       return &StkDealVACr_;
    }
};
//========================================================================
class TTseOrdFieldMaker
{
 typedef TOrdFieldMaker  inherited;

 public:
   typedef TTseOrds::TEvRecs        TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;
   //---------------------------------------------------------------------
   K_mf(const TFieldDesp&) GetFieldDesp(field_n_type n) const
   {
      return OrdFieldDesp2[n]; //Debug: n 太大程式會ERR
   }
   //---------------------------------------------------------------------
   K_mf(field_n_type) GetFieldCount() const
   {
      return 2;
   }
   //---------------------------------------------------------------------
   K_mf(std::string) Get(const iterator& i, field_n_type fieldNo) const
   {
     if(fieldNo==0)
        return TTseOrds::TOrdRecsDefine::GetTseOrdID(i).as_string();
     return i->as_string();
   }
   //---------------------------------------------------------------------
   K_mf(void) Put(TEvContainer& evc, iterator& i, field_n_type fieldNo, const std::string& str)
   {
      return;
   }
};
//========================================================================
struct TTseOrdVACreater : public TVACreater
{
   typedef TTseOrdFieldMaker               TFieldMaker;
   typedef TFieldMaker::TEvContainer       TEvRecs;
   typedef TTViewArea<TEvRecs,TFieldMaker> TViewArea;
   typedef TOrdCenter::TBrokerOrdsDefine::TEvRecs::mapped_type TMasterRec;
   //---------------------------------------------------------------------
   K_mf(TaViewArea) CreateNewVA()
   {
      return TaViewArea(new TViewArea(0)); //Hasn't VA Controls
   }
   //---------------------------------------------------------------------
   K_mf(void) SetContainer(const TMasterRec& masterRec, TViewArea& detailVA, size_t detailID)
   {
      detailVA.SetContainer(masterRec.Recs_[detailID].get());
   }
};
//========================================================================
class Main01
{
   typedef Main01 this_type;
private:
   TOrdSesMgr               *OrdSesMgr_;
   TSysOrdVACreater         *SysOrdVACreater_;
   TScheduler               *Scheduler_;
   Kway::Tw::bx::TbxSesMgr* bxSesMgr_;
   TExcMgr*                 ExcMgr_;
   TMsgTable*               MsgC_;
//   TSysInfoPage*            SysInfo_;
   TScheduleMgr*            ScheduleMgr_;
//   Kway::TViewAreaMgr       SysSettingVA_;
//   TOrdNoMgr*               OrderNoMgr_;
   TSegDefs*                SegDefs_;
   TTwseMainIni*            MainIni_;
   typedef TBrokerVACreater<TOrdCenter::TBrokerOrdsDefine,TTseOrdVACreater,TTseOrds::DetailDesp,TTseOrds::oti_End> DfTBrokerVACreater;
   DfTBrokerVACreater *BrokerOrdVACreater_;
   typedef TScheduleObj<this_type> TSchedObj;
   TSchedObj*            TrxSeg_[12];
   TSchedObj*            ReloadSeg_;


   K_mf(void) SetOrdFlow();
   K_mf(void) SetOrdCenter();
   K_mf(void) SetVA();
   K_mf(void) DoRegSchedule();
   K_mf(void) ReSchedule(TKMemo&);
   K_mf(void) LoadTradeSegs();
   K_mf(void) TrSegmentChg(TKMemo&);

protected:
   Kway::byte            HostID_;
   bool                  UseHostID_;
   bool                  UseClOrdIDMgr_;
   bool                  UseExcMgr_;
   bool                  UseEvLog_;
public:
   Kway::Tw::Stk::TOrdCenter *OrdCenter_;
   Kway::Tw::Stk::TOrdFlow   *OrdFlow_;
   Kway::Tw::Stk::TSysOrds   *SysOrds_;
   Kway::TViewAreaMgr        *OrdCenterVA_;

   K_ctor Main01(Kway::Signon::TSignonSvrDB*);
   K_dtor ~Main01();

   inline K_mf(Kway::Tw::Stk::TOrdCenter*) GetOrdCenter ()  { return OrdCenter_; }
   inline K_mf(Kway::Tw::Stk::TOrdFlow*)   GetOrdFlow   ()  { return OrdFlow_;   }
   inline K_mf(Kway::Tw::Stk::TSysOrds*)   GetSysOrds   ()  { return SysOrds_;   }
};
//---------------------------------------------------------------------------
#endif
