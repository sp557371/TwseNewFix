/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_DbgInfo.h"
#include "Nsa_UtilComm.h"
#include "tplns/Nsa_EvMap_2k_VA.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
#if defined(_NsaVer_DbgInfo_)

/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoID impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TDbgInfoID::assign (const char* str)
{
   inherited::Data_ = edi_Unknown;
   
   if (!str || strlen(str) == 0)
      return;
      
   inherited::Data_ = (EDbgInfoID)(atoi(str));
   if (inherited::Data_ < edi_Unknown || inherited::Data_ >= edi_Total)
      inherited::Data_ = edi_Unknown;
}
//---------------------------------------------------------------------------

K_mf(std::string) TDbgInfoID::as_string () const
{
   if (inherited::Data_ <= edi_Unknown || inherited::Data_ >= edi_Total)
      return std::string();

   static const char* DbgInfoID_Item[edi_Total] = {
      "",
      "TFsSysOrds_updater_dtor",
      "TFsOrdFlow_DoReq",
      "TApaSesSmpBase_OnMesRecvReady",
      "TApaSesSmpBase_TApaSesSmpPkt_GetMsg",
      "TApaSesSmp_MsgUnpack",
      "TSmpMsgMiddlewareEvent_OnSetMsgData",
      "TTwseFsOrdFlow_TRsOrdID_Insert",
      "TTwseFsOrdFlow_TRsOrdID_FindRsFixSoid",
      "TTwseFstAdpt_TOrdImpl_DoReqNew",
      
      "Mds_TCsNyseDict_StkProc",
   };

   return std::string(DbgInfoID_Item[inherited::Data_]);
}


/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoHnd impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TDbgInfoHnd::TDbgInfoHnd (EDbgInfoID diID, const void* owner, bool keepAll, const std::string& param)
   : DiID_    (diID)
   , Owner_   (owner)
   , BegTime_ (TTimeUs::Now())
   , KeepAll_ (keepAll)
   , DbgParam_(param)
{
}
//---------------------------------------------------------------------------

K_dtor TDbgInfoHnd::~TDbgInfoHnd ()
{
   EndTime_  = TTimeUs::Now();
   //CostTime_ = abs(EndTime_ - BegTime_);
   CostTime_ = (EndTime_ - BegTime_);

   TDbgInfoMgr& mgr = TDbgInfoMgr::GetInst();
   mgr.AddInfo(this);
}


/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoMgr impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TDbgInfoMgr::TDbgInfoMgr ()
   : RecsVac_(NULL)
   , ShowAll_(true)
{
   //==============================================
   // 定義特別顯示的部分
   //DbgInfoIDSet_.set(edi_TwseFix_TSmpMsgMiddlewareEvent_OnSetMsgData);
   //DbgInfoIDSet_.set(edi_TwseFix_TTwseFsOrdFlow_TRsOrdID_Insert);
   //DbgInfoIDSet_.set(edi_TwseFix_TTwseFsOrdFlow_TRsOrdID_FindRsFixSoid);

   if (DbgInfoIDSet_.any())
      ShowAll_ = false;
}
//---------------------------------------------------------------------------

K_mf(void) TDbgInfoMgr::AddInfo (TDbgInfoHnd* diHnd)
{
   if (!ShowAll_) {
      if (DbgInfoIDSet_.test(diHnd->DiID_.get_orig()) == false)
         return;
   }

   char buf[512];
   
   strcpy(buf, diHnd->DiID_.as_string().c_str());
   if (diHnd->Owner_) {
      char tmp[20];
      
      sprintf(tmp, "_%p", diHnd->Owner_);
      strcat(buf, tmp);
   }

   TDiName    diName(buf);
   TRecsHnd   rsHnd (this, diName);
   SDbgInfo1& udDat1 = rsHnd.BeginUpdate1();

   udDat1.ExecCount_++;
   udDat1.ExecTime_.IncUs(diHnd->CostTime_);

   if (diHnd->KeepAll_) {
      SDbgInfo2& udDat2 = rsHnd.BeginUpdate2_Inc();

      udDat2.TimeGap_  .assign(diHnd->CostTime_);
      udDat2.DbgParam_ = diHnd->DbgParam_;
      
      udDat2.BegTime_ = diHnd->BegTime_;
      udDat2.EndTime_ = diHnd->EndTime_;
   }
}
//---------------------------------------------------------------------------

K_mf(TDbgInfoMgr&) TDbgInfoMgr::GetInst ()
{
   static TDbgInfoMgr mgr;
   return mgr;
}


/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoMgrFm1
/////////////////////////////////////////////////////////////////////////////
class TDbgInfoMgrFm1
{
   typedef TDbgInfoMgr::TRsName  container;
   typedef container::iterator   iterator;
   typedef container::updater    updater;

   enum {
      IField_DbgName,
         IField_ExecCount,
         IField_ExecTime,
         IField_MaxTimeGap,
      IField_Total
   };

public:
   // GetFieldCount
   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }

   // GetFieldDesp
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "偵錯物件",     40, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
         { "執行次數",     13, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "執行時間",     10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "最大執行時間", 10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      };
      return FdDsp[n];
   };

   // Get
   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_DbgName:     return i->first;
         case IField_ExecCount:   return i->second.ExecCount_ .as_string(0, 0, true);
         case IField_ExecTime:    return i->second.ExecTime_  .as_string();
         case IField_MaxTimeGap:  return i->second.MaxTimeGap_.as_string();
      }
      return std::string();
   }

   // Put
   K_mf(void) Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
   {
      if (fieldNo == IField_MaxTimeGap) {
         updater upd = c.begin_update(i);
         if (str == std::string("y") || str == std::string("Y"))
            upd->MaxTimeGap_ = FindMaxTimeGap(i);
         else if (str == std::string("<0"))
            upd->MaxTimeGap_ = FindLess0TimeGap(i);
         else if (str == std::string("r"))
            upd->MaxTimeGap_ = RecalcExecTime(i);
         else
            upd->MaxTimeGap_.assign(str);
      }
   }

   K_mf(TDiSeqNo) FindMaxTimeGap (iterator& iter)
   {
      TDbgInfoMgr::TRsSeqNo* rsSeqNo = iter->second.GetContainer(rsSeqNo);
      TDiSeqNo               seqNo;
      TDiTimeGap             timeGap;

      for (TDbgInfoMgr::TRsSeqNoI iterSeqNo=rsSeqNo->begin(); iterSeqNo!=rsSeqNo->end(); ++iterSeqNo) {
         if (iterSeqNo->second.TimeGap_ > timeGap) {
            seqNo   = iterSeqNo->first;
            timeGap = iterSeqNo->second.TimeGap_;
         }
      }
      return seqNo;
   }

   K_mf(TDiSeqNo) FindLess0TimeGap (iterator& iter)
   {
      TDbgInfoMgr::TRsSeqNo* rsSeqNo = iter->second.GetContainer(rsSeqNo);
      TDiSeqNo               seqNo;

      for (TDbgInfoMgr::TRsSeqNoI iterSeqNo=rsSeqNo->begin(); iterSeqNo!=rsSeqNo->end(); ++iterSeqNo) {
         if (iterSeqNo->second.TimeGap_ < 0) {
            seqNo = iterSeqNo->first;
            break;
         }
      }
      return seqNo;
   }

   K_mf(TDiSeqNo) RecalcExecTime (iterator& iter)
   {
      SDbgInfo1&             dat1    = iter->second.GetDat1();
      TDbgInfoMgr::TRsSeqNo* rsSeqNo = iter->second.GetContainer(rsSeqNo);
      TDiSeqNo               seqNo;

      dat1.ExecTime_.Encode(0, 0, 0, 0);
      for (TDbgInfoMgr::TRsSeqNoI iterSeqNo=rsSeqNo->begin(); iterSeqNo!=rsSeqNo->end(); ++iterSeqNo) {
         const SDbgInfo2& dat2 = iterSeqNo->second;
         
         dat1.ExecTime_.IncUs(dat2.EndTime_ - dat2.BegTime_);
      }
      return TDiSeqNo();
   }
};
/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoMgrFm2
/////////////////////////////////////////////////////////////////////////////
class TDbgInfoMgrFm2
{
   typedef TDbgInfoMgr::TRsSeqNo  container;
   typedef container::iterator    iterator;
   typedef container::updater     updater;

   enum {
      IField_SeqNo,
         IField_TimeGap,
         IField_DbgParam,
         IField_BegTime,
         IField_EndTime,
      IField_Total
   };

public:
   // GetFieldCount
   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }

   // GetFieldDesp
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "SeqNo",    10, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
         { "執行時間", 10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "執行參數", 13, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "BegTime",  10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "EndTime",  10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      };
      return FdDsp[n];
   };

   // Get
   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_SeqNo:     return i->first.as_string();
         case IField_TimeGap:   return i->second.TimeGap_.as_string(0, 0, true);
         case IField_DbgParam:  return i->second.DbgParam_;
         case IField_BegTime:   return i->second.BegTime_.as_string();
         case IField_EndTime:   return i->second.EndTime_.as_string();
      }
      return std::string();
   }

   // Put
   K_mf(void) Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
   {
      if (fieldNo == IField_SeqNo)
         return;
         
      updater u = c.begin_update(i);
      switch (fieldNo) {
         case IField_TimeGap:   u->TimeGap_ .assign(str);  break;
         case IField_DbgParam:  u->DbgParam_.assign(str);  break;
      }
   }
};
/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoMgr::TRecsVac
/////////////////////////////////////////////////////////////////////////////
class TDbgInfoMgr::TRecsVac : public TEvMap2kVA<TRsName, TDbgInfoMgrFm1, TDbgInfoMgrFm2>
{
   typedef TEvMap2kVA<TRsName, TDbgInfoMgrFm1, TDbgInfoMgrFm2>  inherited;

public:
   inline K_ctor TRecsVac (TRsName* rs) : inherited(rs)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoMgr impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TDbgInfoMgr::~TDbgInfoMgr ()
{
   if (RecsVac_)  delete RecsVac_;
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TDbgInfoMgr::GetVac ()
{
   if (!RecsVac_)
      RecsVac_ = new TRecsVac(&RsName_);
   return *RecsVac_;
}

/////////////////////////////////////////////////////////////////////////////
#endif // defined(_NsaVer_DbgInfo_)
