/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_EvtMsg.h"
#include "Nsa_TextSimpReco.h"
#include "Nsa_LineBuf.h"
#include "tplns/Nsa_EvMap_0File.h"
#include "tplns/Nsa_EvCtn_1k_VA.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TEvtMsgKind impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TEvtMsgKind::assign (const char* str)
{
        if (strcmp(str, "Info")    == 0)  Data_ = emk_Info;
   else if (strcmp(str, "Warning") == 0)  Data_ = emk_Warning;
   else if (strcmp(str, "Error")   == 0)  Data_ = emk_Error;
   else                                   Data_ = emk_None;
}
//---------------------------------------------------------------------------

K_mf(std::string) TEvtMsgKind::as_string () const
{
   if      (Data_ & emk_Info)     return std::string("Info");
   else if (Data_ & emk_Warning)  return std::string("Warning");
   else if (Data_ & emk_Error)    return std::string("Error");

   return std::string();
}

/////////////////////////////////////////////////////////////////////////////
// class TEvtMsg::TRecsRecorder
/////////////////////////////////////////////////////////////////////////////
struct SEvtMsgRecoRoom
{
   typedef TEmID     TKey;
   typedef SEmAll    TRec;
   typedef TCommInn  TInn;

   static K_mf(bool) exec_peek_room (TKey&, TRec&, TInn::rr_room&);
   static K_mf(bool) exec_into_room (TInn::rr_room&, TInn::rr_key&, const TKey&, const TRec&);
};
//---------------------------------------------------------------------------
class TEvtMsg::TRecsRecorder : public TEvMapReco<TRecs, SEmAll, SEvtMsgRecoRoom>
{
   typedef TEvMapReco<TRecs, SEmAll, SEvtMsgRecoRoom>  inherited;
   
public:
   inline K_ctor TRecsRecorder (TRecs& recs, const char* filePath)
      : inherited(&recs, std::string(filePath))
   {}
};

/////////////////////////////////////////////////////////////////////////////
// class TEvtMsg impl
/////////////////////////////////////////////////////////////////////////////
const TEmGrpName  TEvtMsg::SysSts ("系統狀態");
const TEmGrpName  TEvtMsg::DbgSts ("偵錯狀態");
const TEmGrpName  TEvtMsg::ExptSts("異常狀態");
const TEmGrpName  TEvtMsg::SesSts ("連線狀態");
//---------------------------------------------------------------------------

K_ctor TEvtMsg::TEvtMsg ()
   : NewEmID_(0)
   , RecsVAC_(NULL)
{
}
//---------------------------------------------------------------------------

K_mf(void) TEvtMsg::Reload (const char* filePath, const char* fileName)
{
   Kway::TFileMode opmode = Kway::TFileMode(Kway::fmCreatePath | Kway::fmAppend);
   char            buf[256];

   sprintf(buf, "%s/%s.txt", filePath, fileName);
   TxtFile_.Open(buf, opmode);

   //==============================================
   // 檢查 txt 最後一筆的序號
   TEmID                txtEmID;
   TTextSimpReco_Reader tsrReader(buf);
   std::string          strLast = tsrReader.GetLastRec(512);

   if (strLast.empty() == false) {
      // 98/02/13 09:23:44.045 [7371] [Info] [資訊源連線] [上市(172.18.200.24:7001)] [Recv HeartBeat]
      const char* ptr = strchr(strLast.c_str(), (int)'[');
      if (ptr) {
         txtEmID.assign(++ptr);
      }
   }

   sprintf(buf, "%s/%s.bin", filePath, fileName);
   RecsRecorder_ = new TRecsRecorder(Recs_, buf);

   if (Recs_.empty() == false) {
      NewEmID_ = Recs_.begin()->first;
   }

   if (NewEmID_.get_orig() < txtEmID.get_orig()) {
      NewEmID_ = txtEmID;
   }
}
//---------------------------------------------------------------------------

K_mf(void) TEvtMsg::addMsg (EEvtMsgKind msgKind, const char* grpName, const char* grpSubName1, const char* grpSubName2, const char* msgStr)
{
   ost::MutexLock locker(Critical_);

   SEmAll emAll;

   emAll.MsgTime_ = TKDateTimeMS::Now();
   emAll.MsgKind_.assignT(msgKind);
   emAll.GrpName_.assign (grpName);
   emAll.MsgStr_ .assign (msgStr);
   if (grpSubName1)  emAll.GrpSubName1_.assign(grpSubName1);
   if (grpSubName2)  emAll.GrpSubName2_.assign(grpSubName2);

   ++NewEmID_;
   if (!(msgKind & emk_OnlyToTxt)) {
      Recs_.insert(TRecs::value_type(NewEmID_, emAll));
   }

   // 2009/08/21 09:23:44.045 [07371] [Info   ] [載入模組] [資訊源連線] [上市(172.18.200.24:7001)] [Recv HeartBeat]
   TLineBufNew line(512);
   char        msgBuf[50];
   int         msgLen;

   // "2009/08/21 09:23:44.045"
   line.Append(emAll.MsgTime_.as_string(TKDate::sf_ELong));
   // " [07371]"
   msgLen = sprintf(msgBuf, " [%05d]", NewEmID_.get_orig());
   line.Append(msgBuf, msgLen);
   // " [Info   ] "
   msgLen = sprintf(msgBuf, " [%-7s]", emAll.MsgKind_.as_string().c_str());
   line.Append(msgBuf, msgLen);
   // " [載入模組]"
   line.Append(" [", 2);
   line.Append(grpName);
   line.Append("]",  1);
   // " [資訊源連線]"
   if (grpSubName1) {
      line.Append(" [", 2);
      line.Append(grpSubName1);
      line.Append("]",  1);
   }
   // " [上市(172.18.200.24:7001)]"
   if (grpSubName2) {
      line.Append(" [", 2);
      line.Append(grpSubName2);
      line.Append("]",  1);
   }
   // " [Recv HeartBeat]"
   line.Append(" [", 2);
   line.Append(msgStr);
   line.Append("]",  1);

   line.AppendNewLine();

   TxtFile_.Write(line.GetCStr(), (Kway::dword)line.GetUsedSize());
}


/////////////////////////////////////////////////////////////////////////////
// class TEvtMsgFm
/////////////////////////////////////////////////////////////////////////////
class TEvtMsgFm
{
   typedef TEvtMsg::TRecs       container;
   typedef container::iterator  iterator;
   enum {
      IField_EmID,
         IField_MsgTime,
         IField_MsgKind,
         IField_GrpName,
         IField_GrpSubName1,
         IField_GrpSubName2,
         IField_MsgStr,
      IField_Total
   };

public:
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "訊息代號",     9, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Fixed,   },
         { "訊息時間",    22, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "訊息種類",    10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "群組名稱",    12, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "群組子名稱1", 25, ta_xCenter_yCenter, ta_xLeft_yCenter,   tfs_CanEdit, },
         { "群組子名稱2", 25, ta_xCenter_yCenter, ta_xLeft_yCenter,   tfs_CanEdit, },
         { "訊息內容",    55, ta_xCenter_yCenter, ta_xLeft_yCenter,   tfs_CanEdit, },
      };
      return FdDsp[n];
   };

   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_EmID:         return i->first.as_string();
         case IField_MsgTime:      return i->second.MsgTime_.as_string();
         case IField_MsgKind:      return i->second.MsgKind_.as_string();
         case IField_GrpName:      return i->second.GrpName_;
         case IField_GrpSubName1:  return i->second.GrpSubName1_;
         case IField_GrpSubName2:  return i->second.GrpSubName2_;
         case IField_MsgStr:       return i->second.MsgStr_;
      }
      return std::string();
   }

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container&, iterator&, size_t, const std::string&) {}
};
/////////////////////////////////////////////////////////////////////////////
// class TEvtMsg::TRecsVAC
/////////////////////////////////////////////////////////////////////////////
class TEvtMsg::TRecsVAC : public TEvCtn1kVA<TRecs, TEvtMsgFm>
{
   typedef TEvCtn1kVA<TRecs, TEvtMsgFm>  inherited;

public:
   inline K_ctor TRecsVAC (TRecs* recs)
      : inherited(recs)
   {}
};

/////////////////////////////////////////////////////////////////////////////
// class TEvtMsg impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TEvtMsg::~TEvtMsg ()
{
   if (RecsVAC_) {
      VAMgr_.Del(*RecsVAC_);
      delete RecsVAC_;
      RecsVAC_ = NULL;
   }
   
   TxtFile_.Close();
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TEvtMsg::GetVAC ()
{
   if (RecsVAC_ == NULL) {
      RecsVAC_ = new TRecsVAC(&Recs_);

      VAMgr_.Add(std::string("1.全部訊息"), *RecsVAC_);
   }
   return VAMgr_;
}


/////////////////////////////////////////////////////////////////////////////
// struct SEvtMsgRecoRoom impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) SEvtMsgRecoRoom::exec_peek_room (TKey& key, TRec& rec, TInn::rr_room& room)
{
   char grpName[256], grpSubName1[256], grpSubName2[256];
   char msgStr [2048];

   TInn::rr_live_peek peeker[] = {
      { &key,                             sizeof(key)          },
      { &rec.MsgTime_,                    sizeof(rec.MsgTime_) },
      { K_GetLo(&peeker[3].obj_size_, 1), 1                    },
      { grpName,                          0                    },
      { K_GetLo(&peeker[5].obj_size_, 1), 1                    },
      { grpSubName1,                      0                    },
      { K_GetLo(&peeker[7].obj_size_, 1), 1                    },
      { grpSubName2,                      0                    },
      { K_GetLo(&peeker[9].obj_size_, 2), 2                    },
      { msgStr,                           0                    },
      { &rec.MsgKind_,                    sizeof(rec.MsgKind_) },
   };
   
   bool resu = room.peek(peeker, peeker+numofele(peeker));
   if (resu) {
      rec.GrpName_    .assign(grpName,     peeker[3].obj_size_);
      rec.GrpSubName1_.assign(grpSubName1, peeker[5].obj_size_);
      rec.GrpSubName2_.assign(grpSubName2, peeker[7].obj_size_);
      rec.MsgStr_     .assign(msgStr,      peeker[9].obj_size_);
   }
   return resu;
}
//---------------------------------------------------------------------------

K_mf(bool) SEvtMsgRecoRoom::exec_into_room (TInn::rr_room& room, TInn::rr_key& rrkey, const TKey& key, const TRec& rec)
{
   TInn::rr_live_into peeker[] = {
      { &key,                             sizeof(key)                                           },
      { &rec.MsgTime_,                    sizeof(rec.MsgTime_)                                  },
      { K_GetLo(&peeker[3].obj_size_, 1), 1                                                     },
      { rec.GrpName_.c_str(),             static_cast<TInn::rr_size>(rec.GrpName_.length())     },
      { K_GetLo(&peeker[5].obj_size_, 1), 1                                                     },
      { rec.GrpSubName1_.c_str(),         static_cast<TInn::rr_size>(rec.GrpSubName1_.length()) },
      { K_GetLo(&peeker[7].obj_size_, 1), 1                                                     },
      { rec.GrpSubName2_.c_str(),         static_cast<TInn::rr_size>(rec.GrpSubName2_.length()) },
      { K_GetLo(&peeker[9].obj_size_, 2), 2                                                     },
      { rec.MsgStr_.c_str(),              static_cast<TInn::rr_size>(rec.MsgStr_.length())      },
      { &rec.MsgKind_,                    sizeof(rec.MsgKind_)                                  },
   };

   return room.into(rrkey, peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------
