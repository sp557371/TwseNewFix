/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_ExcMgr.h"
#include <stdio.h>
#include "Nsa_SysInfo.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// struct STLevUseTypes
/////////////////////////////////////////////////////////////////////////////
struct STLevUseTypes : public TVAGetEditProp
{
   K_mf(TEditStyle) GetEnums (const TViewArea&, const TViewCursor*, size_t, TStrings* result, bool& selectOnly)
   {
      if (result) {
         std::copy(TErrLevel::LevUseTypeStr_,
                   TErrLevel::LevUseTypeStr_+numofele(TErrLevel::LevUseTypeStr_),
                   std::back_inserter(*result));
      }
      selectOnly = true;
      return es_AllRecsEqual;
   }
};
//---------------------------------------------------------------------------
static STLevUseTypes  LevUseTypes;




//=============================================================================
K_mf(const TFieldDesp&) TErrMsgDFieldMaker::GetFieldDesp(size_t n) const
{
   static TFieldDesp FdDsp[]= {
      { "事件代碼"    , 5,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_Null   ,    },
      { "寫入事件"    , 5,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_CanEdit, 0 ,},
      { "傳送事件"    , 5,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_CanEdit, 0 ,},
      { "事件等級"    , 5,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_CanEdit, 0 , &LevUseTypes},
      { "群組別"      , 5,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_CanEdit,    },
      { "事件說明"    , 60, ta_xLeft_yCenter  ,  ta_xLeft_yCenter  , tfs_CanEdit,    },
      { "備註"        , 20, ta_xLeft_yCenter  ,  ta_xLeft_yCenter  , tfs_CanEdit,    },
   };
   if(GDef_ && n == 4) {
      FdDsp[n].GetEditProp_ = (TVAGetEditProp*)&Keys_;
   }
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TErrMsgDFieldMaker::Get(const iterator& i, size_t fieldNo) const
{
   switch(fieldNo) {
   case  0:  return i->first.as_string();
   case  1:  return i->second.EvLogON_  .as_string();
   case  2:  return i->second.NetSendON_.as_string();
   case  3:  return i->second.EvLevel_  .as_string();
   case  4:  return i->second.ObjID_    .as_string();
   case  5:  return i->second.EvMsg_    .as_string();
   case  6:  return i->second.Remark_   .as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TErrMsgDFieldMaker::Put(TEvContainer& c, iterator& i, size_t fieldNo
   , const std::string& str)
{
   if(fieldNo==0)
      return;
   TEvMapD::updater u = c.begin_update(i);
   switch(fieldNo) {
   case  1:  u->EvLogON_  .assign(str);  break;
   case  2:  u->NetSendON_.assign(str);  break;
   case  3:  u->EvLevel_  .assign(str);  break;
   case  4:  u->ObjID_    .assign(str);  break;
   case  5:  u->EvMsg_    .assign(str);  break;
   case  6:  u->Remark_   .assign(str);  break;
   }
}
//=============================================================================
K_mf(const TFieldDesp&)  TErrMsgVAC::TErrMsgFieldMaker::GetFieldDesp  (field_n_type n) const
{
   static TFieldDesp f[] = {
      {"系統事件代號",  12 , ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_Null,}
   };
   return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TErrMsgVAC::TErrMsgFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
   switch(fieldNo) {
   case 0:  return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(size_t) TErrMsgVAC::GetDetails(const char**& a)
{
   static const char* desp[] = {"子系統-事件代碼"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(TVACreater*)TErrMsgVAC::GetDetailVACreater(size_t detailID)
{
   switch(detailID) {
   case 0:return &DetailCreater_;
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(TVACreater*) TErrMsgVAC::OnMasterViewRowMoved(TViewArea& masterVA
   , TViewArea*& detailVA, size_t detailID)
{
   TMsgViewer& va=static_cast<TMsgViewer&>(masterVA);
   const TMsgViewer::cursor_type *cur = va.GetCurRow();
   TErrMsgDViewer *DetailView = static_cast<TErrMsgDViewer*>(detailVA);
   if(cur->IsInvalid())
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(&(cur->GetIterator()->second));
   return inherited::DetailVACreaterNoChange;
}
//=============================================================================
// 事件記錄器的fieldmaker
//=============================================================================
K_mf(const TFieldDesp&) TEvLogFieldMaker::GetFieldDesp(size_t n) const
{
   static TFieldDesp FdDsp[]= {
      { "序號"        , 6,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_Fixed , },
      { "發生時間"    ,23,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_Null  , },
      { "群組別"      , 5,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_Null  , },
      { "事件代碼"    , 5,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_Null  , },
      { "事件等級"    , 5,  ta_xCenter_yCenter,  ta_xCenter_yCenter, tfs_Null  , },
      { "事件說明"    ,30,  ta_xCenter_yCenter,  ta_xLeft_yCenter  , tfs_Null  , },
      { "事件名稱"    ,20,  ta_xCenter_yCenter,  ta_xLeft_yCenter  , tfs_CanEdit, },
      { "詳細資料"    ,30,  ta_xCenter_yCenter,  ta_xLeft_yCenter  , tfs_CanEdit, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TEvLogFieldMaker::Get(const iterator& i, size_t fieldNo) const
{
   switch(fieldNo) {
   case  0: {
      char buf[32];
      sprintf(buf,"%d",i->first);
      return std::string(buf);
      }
   case  1:  return i->second.Time_   .as_string(TKDate::sf_ELong);
   case  2:  return i->second.ObjID_  .as_string();
   case  3:  return i->second.EvCode_ .as_string();
   case  4:  return i->second.EvLevel_.as_string();
   case  5:  return i->second.EvMsg_  .as_string();
   case  6:  return i->second.MsgOwner_;
   case  7:  return i->second.MsgDetail_;
   }
   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TExcMgr::TLogRecorder
/////////////////////////////////////////////////////////////////////////////
struct TExcMgrLogRecoRoom
{
   typedef dword     TKey;
   typedef SEvLog    TRec;
   typedef TCommInn  TInn;

   static K_mf(bool) exec_peek_room (TKey&, TRec&, TInn::rr_room&);
   static K_mf(bool) exec_into_room (TInn::rr_room&, TInn::rr_key&, const TKey&, const TRec&);
};
//---------------------------------------------------------------------------
class TExcMgr::TLogRecorder : public TEvMapReco<TEvLogs, SEvLog, TExcMgrLogRecoRoom>
{
   typedef TEvMapReco<TEvLogs, SEvLog, TExcMgrLogRecoRoom>  inherited;

public:
   inline K_ctor TLogRecorder (TEvLogs* map, const char* cpFilePath)
      : inherited(map, std::string(cpFilePath))
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TExcMgr impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TExcMgr::TExcMgr (bool isHB, bool dolog)
   : inherited()
   , Timer_   (this)
{
   ErrMap_     = new TEvMap;
   Recorder_   = new TEvRecorder  (ErrMap_, std::string("./settings/EventMsg.dat"));
   ErrVA_      = new TErrMsgVAC   (*ErrMap_, &GroupDef_);
   GDRecorder_ = new TGDRecorder  (&GroupDef_, std::string("./Settings/GroupDef.dat"));
   GroupDefVA_ = new TGroupDefVACr(&GroupDef_);
   ObjVA_      = new TObjVAC      (&ObjMap_);

   VAMgr_.Add(std::string("1.事件代碼對照表"),*ErrVA_);
   VAMgr_.Add(std::string("2.群組類別定義"),  *GroupDefVA_);
   VAMgr_.Add(std::string("0.系統狀態檢視"),  *ObjVA_);
   DoLog_ = dolog;

   if (DoLog_) {
      char buf[256];

      EvLogs_  = new TEvLogs;
      EvLogVA_ = new TEvLogVACr(EvLogs_);
      VAMgr_.Add(std::string("3.事件記錄器"), *EvLogVA_);

      sprintf(buf, "./Logs/%s/EvLogs.rec", TSysInfo::GetTradeDateStr().c_str());
      EvLogRecorder_ = new TLogRecorder(EvLogs_, buf);

      sprintf(buf, "./Logs/%s/EvLogs.log", TSysInfo::GetTradeDateStr().c_str());
      EvLogFile_.Open(buf, (TFileMode)(fmOpenAlways|fmAppend));
      EvLogFile_.Seek(0, fsEnd);
   }
   else {
      EvLogs_        = 0;
      EvLogVA_       = 0;
      EvLogRecorder_ = 0;
   }

   HB_.ErrKind_.assign("0");
   HB_.ErrNum_ .assign("000");

   if(isHB)
      Timer_.Start(60*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_ctor TExcMgr::~TExcMgr()
{
   EvLogFile_.Close();
   Timer_.Stop();
   VAMgr_.Del(*ErrVA_);
   VAMgr_.Del(*GroupDefVA_);
   VAMgr_.Del(*ObjVA_);
   if(EvLogVA_)
      VAMgr_.Del(*EvLogVA_);
   if(EvLogVA_)        delete EvLogVA_;
   if(EvLogRecorder_)  delete EvLogRecorder_;
   if(EvLogs_)         delete EvLogs_;
   delete ObjVA_;
   delete GDRecorder_;
   delete GroupDefVA_;
   delete ErrVA_;
   delete Recorder_;
   delete ErrMap_;
}

//---------------------------------------------------------------------------
K_mf(bool) TExcMgr::GetErrDef(const TErrCodeSet& ercode, TErrLevel& erlevel
   , TObjID& objid, std::string& ermsg)
{
   TEvMap::const_iterator ifind = ErrMap_->find(ercode.ErrKind_);
   if(ifind == ErrMap_->end()) {
      if(ercode == HB_) {
         ermsg = "Alive!";
         return true;
      }
      return false;
   }

   if(const TEvMapD* mapd = &ifind->second) {
      TEvMapD::const_iterator L = mapd->find(ercode.ErrNum_);
      if(L == mapd->end()) {
         if(ercode == HB_) {
            ermsg = "Alive!";
            return true;
         }
         return false;
      }

      objid   = L->second.ObjID_;
      erlevel = L->second.EvLevel_;
      ermsg   = L->second.EvMsg_.as_string();
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

K_mf(void) TExcMgr::WriteEvLog (const TObjID&      objid, const TErrCodeSet& ercode,   const TErrLevel&   erlevel,
                                const std::string& msg,   const std::string& msgOwner, const std::string& msgDetail)
{
   if (ercode == HB_)
      return;

   SEvLog evlog;

   evlog.Time_      = TKDateTimeMS::Now();
   evlog.ObjID_     = objid;
   evlog.EvCode_    = ercode;
   evlog.EvLevel_   = erlevel;
   evlog.EvMsg_     .assign(msg);
   evlog.MsgOwner_  = msgOwner;
   evlog.MsgDetail_ = msgDetail;
   
   if (EvLogs_)
      EvLogs_->insert(std::make_pair(EvLogs_->size(), evlog));

   if (EvLogFile_.IsOpened()) {
      char buf[1024];
      
      sprintf(buf, "%s %-4s %s %s %-60s %-30s %-70s\r\n",
                        evlog.Time_   .as_string(TKDate::sf_ELong).c_str(),
                        evlog.ObjID_  .as_string().c_str(),
                        evlog.EvCode_ .as_string().c_str(),
                        evlog.EvLevel_.as_string().c_str(),
                        msg      .c_str(),
                        msgOwner .c_str(),
                        msgDetail.c_str());
      
      EvLogFile_.Write(buf);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TExcMgr::OnTimer(TTimerBase*)
{
   Alert(HB_);
}
//---------------------------------------------------------------------------

K_mf(std::string) TExcMgr::GetFuncStatus (const TErrCodeSet& ercode)
{
   TEvMap::const_iterator ifind = ErrMap_->find(ercode.ErrKind_);
   if (ifind == ErrMap_->end())
      return std::string();

   const TEvMapD*          mapd = &ifind->second;
   TEvMapD::const_iterator L    = mapd->find(ercode.ErrNum_);
   if (L == mapd->end())
      return std::string();

   return L->second.EvLogON_.as_string(true) + L->second.NetSendON_.as_string(true);
}


/////////////////////////////////////////////////////////////////////////////
// struct TExcMgrLogRecoRoom impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TExcMgrLogRecoRoom::exec_peek_room (TKey& key, TRec& rec, TInn::rr_room& room)
{
   char caMsgOwner[256], caMsgDetail[256];

   TInn::rr_live_peek peeker[] = {
      { &key,                             sizeof(key)          },
      { &rec.Time_,                       sizeof(rec.Time_)    },
      { &rec.ObjID_,                      sizeof(rec.ObjID_)   },
      { &rec.EvCode_,                     sizeof(rec.EvCode_)  },
      { &rec.EvLevel_,                    sizeof(rec.EvLevel_) },
      { &rec.EvMsg_,                      sizeof(rec.EvMsg_)   },
      { K_GetLo(&peeker[7].obj_size_, 1), 1                    },
      { caMsgOwner,                       0                    },
      { K_GetLo(&peeker[9].obj_size_, 1), 1                    },
      { caMsgDetail,                      0                    },
   };

   bool resu = room.peek(peeker, peeker+numofele(peeker));
   if (resu) {
      rec.MsgOwner_ .assign(caMsgOwner,  peeker[7].obj_size_);
      rec.MsgDetail_.assign(caMsgDetail, peeker[9].obj_size_);
   }
   return resu;
}
//---------------------------------------------------------------------------

K_mf(bool) TExcMgrLogRecoRoom::exec_into_room (TInn::rr_room& room, TInn::rr_key& rrkey, const TKey& key, const TRec& rec)
{
   TInn::rr_live_into peeker[] = {
      { &key,                             sizeof(key)          },
      { &rec.Time_,                       sizeof(rec.Time_)    },
      { &rec.ObjID_,                      sizeof(rec.ObjID_)   },
      { &rec.EvCode_,                     sizeof(rec.EvCode_)  },
      { &rec.EvLevel_,                    sizeof(rec.EvLevel_) },
      { &rec.EvMsg_,                      sizeof(rec.EvMsg_)   },
      { K_GetLo(&peeker[7].obj_size_, 1), 1                                                   },
      { rec.MsgOwner_.c_str(),            static_cast<TInn::rr_size>(rec.MsgOwner_.length())  },
      { K_GetLo(&peeker[9].obj_size_, 1), 1                                                   },
      { rec.MsgDetail_.c_str(),           static_cast<TInn::rr_size>(rec.MsgDetail_.length()) },
   };

   return room.into(rrkey, peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------
