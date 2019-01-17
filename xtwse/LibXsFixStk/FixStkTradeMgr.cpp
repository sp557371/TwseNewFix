/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkTradeMgr.h"
#include "Nsa_UtilComm.h"
#include "tplns/Nsa_EvMap_0File.h"
#include "FixStkCoids.h"
#include "FixStkRptTrack.h"
#include "FixStkRptFlow.h"
#include "FixStkOrdFlow.h"
#include "FixStkRptCenter.h"
#include "FixStkOrdCenterVA.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TTradeUnit impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTradeUnit::TTradeUnit (const std::string& sesID)
   : SesID_(sesID)
{
}
//---------------------------------------------------------------------------

K_dtor TTradeUnit::~TTradeUnit ()
{
   TViewAreaMgr& vaMgr     = TradeInfo_->GetVaMgr       ();
   TCoids*       coids     = TradeInfo_->GetFixCoids    ();
   TFsOrdCenter* ordCenter = TradeInfo_->GetFixOrdCenter();
   TRptCenter*   rptCenter = TradeInfo_->GetFixRptCenter();
   TOrdTrack*    ordTrack  = TradeInfo_->GetFixOrdTrack ();
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   TRptTrack*    rptTrack  = TradeInfo_->GetFixRptTrack ();
   #endif
   //**********************************************
   TFsOrdFlow*   ordFlow   = TradeInfo_->GetFixOrdFlow  ();
   TRptFlow*     rptFlow   = TradeInfo_->GetFixRptFlow  ();
   TTradeAdpt*   tradeAdpt = TradeInfo_->GetFixTradeAdpt();

   vaMgr.Del(ordCenter->GetSysOrdsVAC());
   vaMgr.Del(rptCenter->GetSysRptsVAC());
   vaMgr.Del(coids    ->GetVAC       ());
   
   delete tradeAdpt;
   delete rptFlow;
   delete ordFlow;
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   delete rptTrack;
   #endif
   //**********************************************
   delete ordTrack;
   delete rptCenter;
   delete ordCenter;
   delete coids;
   delete TradeInfo_;
   delete TradeFact_;
}
//---------------------------------------------------------------------------

K_mf(void) TTradeUnit::Init ()
{
   TradeFact_ = CreateTradeFact();

   TradeInfo_ = TradeFact_->CreateTradeInfo(SesID_);
   TradeInfo_->UseLogFile  ();
   TradeInfo_->SetFixTradeUnit(this);

   const std::string&  logPath = TradeInfo_->GetLogPath();
         TViewAreaMgr& vaMgr   = TradeInfo_->GetVaMgr  ();
   
   TUtilComm::ConsMsg("Load FixCoids");
   TCoids* coids = new TCoids(TradeInfo_);
   TradeInfo_->SetFixCoids(coids);
   
   TUtilComm::ConsMsg("Load FixOrdCenter");
   TFsOrdCenter* ordCenter = new TFsOrdCenter(logPath);
   TradeInfo_->SetFixOrdCenter(ordCenter);

   TUtilComm::ConsMsg("Load FixRptCenter");
   TRptCenter* rptCenter = new TRptCenter(TradeInfo_);
   TradeInfo_->SetFixRptCenter(rptCenter);

   TUtilComm::ConsMsg("Load FixOrdTrack");
   TOrdTrack* ordTrack = new TOrdTrack (logPath);
   TradeInfo_->SetFixOrdTrack(ordTrack);

   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   TUtilComm::ConsMsg("Load FixRptTrack");
   TRptTrack* rptTrack = new TRptTrack(TradeInfo_);
   TradeInfo_->SetFixRptTrack(rptTrack);
   #endif
   //**********************************************

   TFsOrdFlow* ordFlow = TradeFact_->CreateOrdFlow(TradeInfo_);
   TRptFlow*   rptFlow = TradeFact_->CreateRptFlow(TradeInfo_);

   ordFlow->SetRptFlow(rptFlow);
   
   TVACreater* vacOrdCenter = TradeFact_->CreateOrdCenetrVAC(TradeInfo_);
   ordCenter->SetSysOrdsVAC(vacOrdCenter);

   TTradeAdpt* tradeAdpt = TradeFact_->CreateTradeAdpt(TradeInfo_);
   TradeInfo_->SetFixTradeAdpt(tradeAdpt);

   //==============================================
   // 紀錄參數內容
   // 在 tradeAdpt ->Startup() 時就有可能開始寫 log 了, 所以這邊先放
   TTradeInfo::SFlushInfo& flhInfo  = TradeInfo_->GetFlushInfo  ();
   TTradeLog*              tradeLog = TradeInfo_->GetFixTradeLog();
   char                    logMsg[512];
   int                     logLen;
   
   logLen = sprintf(logMsg, "%c==============================================================================", 0x0a);
   tradeLog->LogMem(elt_Info, logMsg, logLen);

   logLen = sprintf(logMsg, "Param: Coids[%c], OrdCenter[%c], OrdTrack[%c], RptCenter[%c], ExecIDGene[%c]",
                              (flhInfo.Coids_      ? 'Y' : 'N'),
                              (flhInfo.OrdCenter_  ? 'Y' : 'N'),
                              (flhInfo.OrdTrack_   ? 'Y' : 'N'),
                              (flhInfo.RptCenter_  ? 'Y' : 'N'),
                              (flhInfo.ExecIDGene_ ? 'Y' : 'N'));
   tradeLog->LogMem(elt_Info, logMsg, logLen);

   tradeAdpt ->Startup();
   
   //==============================================
   // va 顯示
   vaMgr.Add(std::string("1.委託資料"), ordCenter->GetSysOrdsVAC());
   vaMgr.Add(std::string("2.回報電文"), rptCenter->GetSysRptsVAC());
   vaMgr.Add(std::string("3.ClOrdIDs"), coids    ->GetVAC       ());
}


/////////////////////////////////////////////////////////////////////////////
// class TTradeMgr::TRecsRecorder
/////////////////////////////////////////////////////////////////////////////
struct TTradeMgrRecoRoom
{
   typedef TTradeMgr::TRecs::key_type  TKey;
   typedef TTradeMgr::SRecBase         TRec;
   typedef TCommInn                    TInn;

   static K_mf(bool) exec_peek_room (TKey&, TRec&, TInn::rr_room&);
   static K_mf(bool) exec_into_room (TInn::rr_room&, TInn::rr_key&, const TKey&, const TRec&);
};
//---------------------------------------------------------------------------
class TTradeMgr::TRecsRecorder : public TEvMapReco<TRecs, SRecBase, TTradeMgrRecoRoom>
{
   typedef TEvMapReco<TRecs, SRecBase, TTradeMgrRecoRoom>  inherited;

public:
   inline K_ctor TRecsRecorder (TRecs* map, const char* filePath)
      : inherited(map, std::string(filePath))
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TTradeMgr impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTradeMgr::TTradeMgr ()
   : RecsRecorder_(NULL)
{
   //==============================================
   // 自行建立[預設]的註冊區
   DefReg_.PoliName_         = NULL;
   DefReg_.TradeUnitCreater_ = NULL;
   DefReg_.Param_            = NULL;
}
//---------------------------------------------------------------------------

K_dtor TTradeMgr::~TTradeMgr ()
{
   Cleanup();
}
//---------------------------------------------------------------------------

K_mf(void) TTradeMgr::Reload (const char* idxPath)
{
   char fullPath[256];
   sprintf(fullPath, "%s/FstUnit.bin", idxPath);
   RecsRecorder_ = new TRecsRecorder(&Recs_, fullPath);

   for (TRecsI iter=Recs_.begin(); iter!=Recs_.end(); ++iter) {
      if (iter->second.TradeUnitPtr_.get() != NULL)
         continue;
      
      CreateTradeUnit(iter);
   }
}
//---------------------------------------------------------------------------

K_mf(TTradeUnitPtr) TTradeMgr::CreateTradeUnit (TRecsI& iter)
{
   TTradeUnit* unit = CreateTradeUnit(iter->first, iter->second.PoliName_);
         
   iter->second.TradeUnitPtr_.reset(unit);
   
   if (unit) {
      VAMgr_.Add(iter->first, unit->GetVaMgr());
   }
   
   return iter->second.TradeUnitPtr_;
}
//---------------------------------------------------------------------------

K_mf(TTradeUnit*) TTradeMgr::CreateTradeUnit (const std::string& sesID, const std::string& poliName)
{
   if (poliName.empty()) {
      // 使用預設的
      if (DefReg_.TradeUnitCreater_ == NULL)
         return NULL;

      return DefReg_.TradeUnitCreater_(sesID, DefReg_.Param_);
   }

   TTradeUnitRegsI iter = (TradeUnitRegs_.begin() + 1);
   for (; iter!=TradeUnitRegs_.end(); ++iter) {
      const TTradeUnitReg* reg = *iter;
      
      if (reg->PoliName_ && strcmp(reg->PoliName_, poliName.c_str()) == 0) {
         if (reg->TradeUnitCreater_) {
            return reg->TradeUnitCreater_(sesID, reg->Param_);
         }
         break;
      }
   }

   return NULL;
}
//---------------------------------------------------------------------------

K_mf(TTradeUnitPtr) TTradeMgr::QueryUnit (const std::string& sesID, const std::string& poliName)
{
   TRecsI iter = Recs_.find(sesID);

   if (iter == Recs_.end()) {
             iter = Recs_.fetch(sesID);
      TRecsU upd  = Recs_.begin_update(iter);
      
      upd->PoliName_ = poliName;
      return CreateTradeUnit(iter);
   }
   else if (iter->second.TradeUnitPtr_.get() == NULL) {
      return CreateTradeUnit(iter);
   }
   
   return iter->second.TradeUnitPtr_;
}
//---------------------------------------------------------------------------

K_mf(TTradeUnitPtr) TTradeMgr::GetFirstUnit ()
{
   if (Recs_.empty())
      return TTradeUnitPtr();

   return Recs_.begin()->second.TradeUnitPtr_;
}
//---------------------------------------------------------------------------

K_mf(void) TTradeMgr::Cleanup (TRecsI& iter, bool autoErase)
{
   TTradeUnit* unit = iter->second.TradeUnitPtr_.get();
      
   if (unit) {
      VAMgr_.Del(unit->GetVaMgr());
   }

   if (autoErase)
      Recs_.erase(iter);
}
//---------------------------------------------------------------------------

K_mf(void) TTradeMgr::Cleanup ()
{
   if (RecsRecorder_) {
      delete RecsRecorder_;
      RecsRecorder_ = NULL;
   }

   for (TRecsI iter=Recs_.begin(); iter!=Recs_.end(); ++iter) {
      Cleanup(iter, false);
   }
   
   Recs_.clear();
}
//---------------------------------------------------------------------------

K_mf(TTradeMgr&) TTradeMgr::GetInst ()
{
   static TTradeMgr tradeMgr;
   return tradeMgr;
}


/////////////////////////////////////////////////////////////////////////////
// struct TTradeMgrRecoRoom impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TTradeMgrRecoRoom::exec_peek_room (TKey& key, TRec& rec, TInn::rr_room& room)
{
   char caKey[256], caPoliName[256];
      
   TInn::rr_live_peek peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 1), 1 },
      { caKey,                            0 },
      { K_GetLo(&peeker[3].obj_size_, 1), 1 },
      { caPoliName,                       0 },
   };

   bool resu = room.peek(peeker, peeker+numofele(peeker));
   if (resu) {
      key          .assign(caKey,      peeker[1].obj_size_);
      rec.PoliName_.assign(caPoliName, peeker[3].obj_size_);
   }
   return resu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTradeMgrRecoRoom::exec_into_room (TInn::rr_room& room, TInn::rr_key& rrkey, const TKey& key, const TRec& rec)
{
   TInn::rr_live_into peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 1), 1                                                  },
      { key.c_str(),                      static_cast<TInn::rr_size>(key.length())           },
      { K_GetLo(&peeker[3].obj_size_, 1), 1                                                  },
      { rec.PoliName_.c_str(),            static_cast<TInn::rr_size>(rec.PoliName_.length()) },
   };

   return room.into(rrkey, peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------



/////////////////////////////////////////////////////////////////////////////
// 淘汰區
/////////////////////////////////////////////////////////////////////////////
/*
K_mf(void) TTradeMgr::InitUnit (TTradeUnitPtr ptr)
{
   const std::string& sesID = ptr->GetSesID();
   TRecsI             iter  = Recs_.find(sesID);

   if (iter != Recs_.end()) {
      Cleanup(iter, true);
   }
   
   iter         = Recs_.fetch(sesID);
   iter->second = ptr;

   TTradeUnit* unit = ptr.get();
   
   VAMgr_.Add(sesID, unit->GetVAC());
}
//---------------------------------------------------------------------------
*/
