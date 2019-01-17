/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "CtrlRsReqResu.h"
//#include "SysMgrObjs.h"
#include "ExtDef.h"
#include "Nsa_LineBuf.h"
#include "Nsa_Per.h"
#include "tplns/Nsa_EvMap_2k_File.h"
#include "tplns/Nsa_EvMap_2k_VA.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// struct TReqID impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TReqID::assign (const char* str)
{
   Time_   .Clear();
   RandNum_.assign0();
   SeqNum_ .assign0();

   if (!str || strlen(str) == 0)
      return;

   Time_.assign(str);

   const char* ptr = strchr(str, (int)' ');
   if (ptr) {
      RandNum_.assign(++ptr);
      ptr = strchr(ptr, (int)' ');
      if (ptr)
         SeqNum_.assign(++ptr);
   }
}
//---------------------------------------------------------------------------

K_mf(std::string) TReqID::as_string () const
{
   char buf[30];
   int  len;

   if (SeqNum_.is_0())
      len = sprintf(buf, "%s %04d", Time_.as_string().c_str(), RandNum_.get_orig());
   else
      len = sprintf(buf, "%s %04d %02d", Time_.as_string().c_str(), RandNum_.get_orig(), SeqNum_.get_orig());

   return std::string(buf, len);
}
//---------------------------------------------------------------------------

K_mf(bool) TReqID::operator < (const TReqID& a) const
{
   if (Time_ < a.Time_) {
      return true;
   }
   else if (Time_ == a.Time_) {
      if (RandNum_ < a.RandNum_) {
         return true;
      }
      else if (RandNum_ == a.RandNum_) {
         if (SeqNum_ < a.SeqNum_)
            return true;
      }
   }
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TReqID::operator == (const TReqID& a) const
{
   return (Time_ == a.Time_ && RandNum_ == a.RandNum_ && SeqNum_ == a.SeqNum_);
}


/////////////////////////////////////////////////////////////////////////////
// class TProcResu impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TProcResu::as_desc () const
{
   if (inherited::Data_ < epr_LinUp || inherited::Data_ >= epr_Total)
      return std::string();

   static const char* itemDesc[] = {
      "排隊中",
      "處理中",
      "Appia 指令已全部傳送",
      "工作完成",
      "Tandem Request 欄位資料錯誤",
      "Tandem 連線中斷",
      "與 Appia 連線逾時",
   };

   return std::string(itemDesc[inherited::Data_]);
}


/////////////////////////////////////////////////////////////////////////////
// class TApaSentList impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TApaSentList::as_string () const
{
   static const char* itemList[] = {
      "Cmd",
      "Query",
   };

   TLineBuf line(128);
   
   for (size_t i=0; i<asl_Total; ++i) {
      if (inherited::test(i)) {
         line.Append(itemList[i]);
         line.Append(' ');
      }
   }

   if (!line.Empty()) {
      line.EraseBack(1);
      return std::string(line.GetCStr());
   }
      
   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// struct SRsReqResu2 impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) SRsReqResu2::IsSentOk (ETandFuncCode funcCode) const
{
   // 如果是 StatusQuery, 只需檢查是否送過 query 即可
   if (funcCode == tfc_StatusQuery)
      return ApaSentList_.test(asl_Query);

   // 檢查 cmd 是否發送過了
   if (!ApaSentList_.test(asl_Cmd))
      return false;

   // cmd 送過了, 看看是否有回應了
   // 因為必須要有回應後, 才會發送 query
   if (StatusCode1_.empty())
      return false;

   // 指令有問題,就不需要在 query 了
   if (StatusCode1_.get_orig() != esc_Normal)
      return true;

   return ApaSentList_.test(asl_Query);
}
//---------------------------------------------------------------------------

K_mf(bool) SRsReqResu2::IsFinish (ETandFuncCode funcCode) const
{
   // 不管是何種 FuncCode, StatusCode1 都必須要有狀態
   if (StatusCode1_.empty())
      return false;

   // 如果是 StatusQuery 時, StatusCode1 有狀態即代表完成
   if (funcCode == tfc_StatusQuery)
      return true;

   // 如果 StatusCode1 不正常, 接下來的工作也就可以不需要了
   if (StatusCode1_.get_orig() != esc_Normal)
      return true;
   
   return (StatusCode2_.empty() ? false : true);
}


/////////////////////////////////////////////////////////////////////////////
// class TCtrlRsReqResu::TRecsRecorder
/////////////////////////////////////////////////////////////////////////////
class TCtrlRsReqResu::TRecsRecorder
{
   struct SRecoStore1
   {
      template <class TPersistent, class TStorage, class TKey, class TRec>
      K_mf(bool) exec (TPersistent& per, TStorage& storage, TKey& key, TRec& rec)
      {
         return storage.store(per, &key,          sizeof(key)           ) &&
                storage.store(per, rec.RecvTime_                        ) &&
                storage.store(per, rec.SentTime_                        ) &&
                storage.store(per, &rec.ProcResu_, sizeof(rec.ProcResu_)) &&
                storage.store(per, rec.FuncCode_                        ) &&
                storage.store(per, rec.MsgType_                         ) &&
                storage.store(per, rec.MsgTime_                         ) &&
                storage.store(per, rec.AllMarketFlag_                   ) &&
                storage.store(per, rec.NewSeqNum_);
      }
   };

   struct SRecoStore2
   {
      template <class TPersistent, class TStorage, class TKey, class TRec>
      K_mf(bool) exec (TPersistent& per, TStorage& storage, TKey& key, TRec& rec)
      {
         return storage.store(per, &key,              sizeof(key)                     ) &&
                storage.store(per, &rec.ApaSentList_, sizeof(rec.ApaSentList_)        ) &&
                storage.store(per, rec.TermNo_                                        ) &&
                storage.store(per, rec.ConnState_                                     ) &&
                storage.store(per, rec.ServiceDownFlag_                               ) &&
                storage.store(per, rec.SuspendFlag_                                   ) &&
                storage.store(per, rec.HoldFlag_                                      ) &&
                storage.store(per, rec.InMsgSeqNum_                                   ) &&
                storage.store(per, rec.LastInMsgTime_                                 ) &&
                storage.store(per, rec.OutMsgSeqNum_                                  ) &&
                storage.store(per, rec.LastOutMsgTime_                                ) &&
                storage.store(per, rec.StatusCode1_                                   ) &&
                storage.store(per, rec.StatusCode2_                                   ) &&
                storage.store(per, rec.Text_                                          ) &&
                storage.store(per, rec.ReqIdx_                                        ) &&
                storage.store(per, rec.AppiaGrp_);
      }
   };

   typedef TEvMapReco_v2  <TRsReqID, SRsReqResu1, SRecoStore1>  TRecorder1;
   typedef TMapMaker_2k2d <TRsReqID, TRsFixSesID>               TMapMakerImpl;
   typedef TEvMapReco2d_v2<TMapMakerImpl, SRecoStore2>          TRecorder2;

   TRecorder1  Recorder1_;
   TRecorder2  Recorder2_;

public:
   inline K_ctor TRecsRecorder (TRsReqID& ctn, const char* path1, const char* path2)
      : Recorder1_(&ctn, std::string(path1))
      , Recorder2_(&ctn, std::string(path2))
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TCtrlRsReqResu impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TCtrlRsReqResu::TCtrlRsReqResu ()
   : RecsVac_(NULL)
{
   char filePath[256], filePath1[256], filePath2[256];
   
   strcpy(filePath, GetProcLogPath().c_str());
   sprintf(filePath1, "%sRsReqResu.bin",    filePath);
   sprintf(filePath2, "%sRsReqResuDet.bin", filePath);

   RecsRecorder_ = new TRecsRecorder(RsReqID_, filePath1, filePath2);
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlRsReqResu::AddReq (TReqID& key, const SRsReqResu1& dat1, TRsFixSesID& rsFixSesID)
{
   TRecsHnd hnd(this);
   bool     ret;

   do {
      ret = hnd.Insert1(key, dat1);
      if (ret)
         break;

      key.SeqNum_++;

   } while (true);

   TRsFixSesID* map2 = hnd.GetMap2();
   *map2 = rsFixSesID;
}
//---------------------------------------------------------------------------

K_mf(bool) TCtrlRsReqResu::FindNewReq (TReqID& reqID)
{
   TRecsHnd hnd(this);

   if (hnd.MoveFirst1() == false) {
      reqID.clear();
      return false;
   }

   do {
      const SRsReqResu1* cpDat1 = hnd.FindDat1();
      
      if (cpDat1->ProcResu_ == epr_LinUp) {
         reqID = hnd.GetKey1();
         return true;
      }
   } while (hnd.MoveNext1());

   reqID.clear();
   return false;
}


/////////////////////////////////////////////////////////////////////////////
// class  TCtrlRsReqResuFm1
/////////////////////////////////////////////////////////////////////////////
class TCtrlRsReqResuFm1
{
   typedef TCtrlRsReqResu::TRsReqID  container;
   typedef container::iterator       iterator;
   enum {
      IField_ReqID,
         IField_RecvTime,
         IField_FuncCode,
         IField_MsgType,
         IField_MsgTime,
         IField_AllMarketFlag,
         IField_NewSeqNum,
         IField_ProcResu,
         IField_SentTime,
      IField_Total
   };

public:
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "ReqID",         25, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
         { "RecvTime",      11, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "FuncCode",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "MsgType",       10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "MsgTime",       10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "AllMarketFlag", 10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "NewSeqNum",     10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "ProcResu",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "SentTime",      11, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      };
      return FdDsp[n];
   }

   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_ReqID:          return i->first                .as_string();
         case IField_RecvTime:       return i->second.RecvTime_     .as_string();
         case IField_FuncCode:       return i->second.FuncCode_     .as_desc  ();
         case IField_MsgType:        return i->second.MsgType_      .as_string();
         case IField_MsgTime:        return i->second.MsgTime_      .as_string();
         case IField_AllMarketFlag:  return i->second.AllMarketFlag_.as_string();
         case IField_NewSeqNum:      return i->second.NewSeqNum_    .as_string();
         case IField_ProcResu:       return i->second.ProcResu_     .as_desc  ();
         case IField_SentTime:       return i->second.SentTime_     .as_string();
      }
      return std::string();
   }

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container& c, iterator& i, size_t fieldNo, const std::string& str)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class  TCtrlRsReqResuFm2
/////////////////////////////////////////////////////////////////////////////
class TCtrlRsReqResuFm2
{
   typedef TCtrlRsReqResu::TRsFixSesID  container;
   typedef container::iterator          iterator;
   enum {
      IField_FixSesID,
         IField_ReqIdx,
         IField_AppiaGrp,
         IField_ApaSentList,
         IField_TermNo,
         IField_ConnState,
         IField_ServiceDownFlag,
         IField_SuspendFlag,
         IField_HoldFlag,
         IField_InMsgSeqNum,
         IField_LastInMsgTime,
         IField_OutMsgSeqNum,
         IField_LastOutMsgTime,
         IField_StatusCode1,
         IField_StatusCode2,
         IField_Text,
      IField_Total
   };

public:
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "FixSesID",        10, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
         { "ReqIdx",           7, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "AppiaGroup",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit,   },
         { "ApaSentList",     11, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "TermNo",          10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "ConnState",       10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "ServiceDownFlag", 16, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "SuspendFlag",     11, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "HoldFlag",        10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "InMsgSeqNum",     11, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "LastInMsgTime",   13, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "OutMsgSeqNum",    12, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "LastOutMsgTime",  14, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "StatusCode1",     10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "StatusCode2",     10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "Text",            10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      };
      return FdDsp[n];
   }

   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_FixSesID:         return i->first                  .as_string();
         case IField_ReqIdx:
            {
               char buf[20];
               int  len = sprintf(buf, "%d", i->second.ReqIdx_.get_orig());
               return std::string(buf, len);
            }
            break;
         case IField_AppiaGrp:         return i->second.AppiaGrp_       .as_string();
         case IField_ApaSentList:      return i->second.ApaSentList_    .as_string();
         case IField_TermNo:           return i->second.TermNo_         .as_string();
         case IField_ConnState:        return i->second.ConnState_      .as_string();
         case IField_ServiceDownFlag:  return i->second.ServiceDownFlag_.as_string();
         case IField_SuspendFlag:      return i->second.SuspendFlag_    .as_string();
         case IField_HoldFlag:         return i->second.HoldFlag_       .as_string();
         case IField_InMsgSeqNum:      return i->second.InMsgSeqNum_    .as_string();
         case IField_LastInMsgTime:    return i->second.LastInMsgTime_  .as_string();
         case IField_OutMsgSeqNum:     return i->second.OutMsgSeqNum_   .as_string();
         case IField_LastOutMsgTime:   return i->second.LastOutMsgTime_ .as_string();
         case IField_StatusCode1:      return i->second.StatusCode1_    .as_string();
         case IField_StatusCode2:      return i->second.StatusCode2_    .as_string();
         case IField_Text:             return i->second.Text_           .as_string();
      }
      return std::string();
   }

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container& c, iterator& i, size_t fieldNo, const std::string& str)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class TCtrlRsReqResu::TRecsVac
/////////////////////////////////////////////////////////////////////////////
class TCtrlRsReqResu::TRecsVac : public TEvMap2kVA<TRsReqID, TCtrlRsReqResuFm1, TCtrlRsReqResuFm2>
{
   typedef TEvMap2kVA<TRsReqID, TCtrlRsReqResuFm1, TCtrlRsReqResuFm2>  inherited;

public:
   inline K_ctor TRecsVac (TRsReqID* map1) : inherited(map1)  {}
};

/////////////////////////////////////////////////////////////////////////////
// class TCtrlRsReqResu impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TCtrlRsReqResu::~TCtrlRsReqResu ()
{
   if (RecsVac_)  delete RecsVac_;
   delete RecsRecorder_;
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TCtrlRsReqResu::GetVac ()
{
   if (RecsVac_ == NULL)
      RecsVac_ = new TRecsVac(&RsReqID_);

   return *RecsVac_;
}
//---------------------------------------------------------------------------
