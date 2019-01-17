//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif

#include <time.h>
#include <stdarg.h>
#include "SesFix.h"
#include "FixAdminMsg.h"
#include "FixAppMsg.h"
#include "FixSesMgr.h"
#include "TwStkIvacNo.h"
#include "KIniFile.h"
using namespace Kway::Tw::Fix::Stk;
using namespace Kway::Tw::Stk;

namespace Kway {
namespace Tw   {
namespace Fix  {
namespace Stk  {
//---------------------------------------------------------------------------
std::string  GetFixDateTimeMs(bool utc, bool date, bool time, bool ms, bool log)
{
   std::string  timeStr;
   Kway::TKDateTimeMS  dt = Kway::TKDateTimeMS().Now();
   if(utc)
      dt.IncHour(-8); //東八區處理方式,特別注意個時區不同需調整
   if(date)
      timeStr.append(dt.GetDate().as_string(Kway::TKDate::sf_ELong, 0)); 
   if(time) {
      if(date)
         timeStr.append("-");
      timeStr.append(dt.GetTime().as_string());
   }
   if(ms) {
      if(time)
         timeStr.append(".");
      timeStr.append(dt.GetMS().as_string());   
   }
   if(log)
      timeStr.append(" ");
   return std::string(timeStr);
}
//---------------------------------------------------------------------------
const char* GmtDateTimeToStr(char* buf, size_t maxlen, struct tm* gmt, bool date, bool time)
{
   if(time == false)
     strftime(buf, maxlen, "%Y%m%d", gmt);     // yyyymmdd (GMT)
   else if(date == false)
     strftime(buf, maxlen, "%H:%M:%S", gmt);   // hh:mm:ss (GMT)
   else
     strftime(buf, maxlen, "%Y%m%d-%H:%M:%S", gmt);   // yyyymmdd-hh:mm:ss (GMT)
   return buf;
}
//---------------------------------------------------------------------------
const char* GetLocalDateTime()
{
   static char buf[32];
   time_t timer;
   struct tm *tb;
   timer = time(NULL);
   // converts date/time to a structure
   tb = localtime(&timer);
   return GmtDateTimeToStr(buf, sizeof(buf), tb, true, true);
}
//---------------------------------------------------------------------------
const char* GetUTCDateTime(bool datestr, bool timestr)
{
   static char buf[32];
   time_t timer;
   struct tm *gmt;
   //gets time of day
   timer = time(NULL);
   //converts date/time to a GMT structure
   gmt = gmtime(&timer);

   return GmtDateTimeToStr(buf, sizeof(buf), gmt, datestr, timestr);
}
//---------------------------------------------------------------------------
const char* ConvertToUTCDateTime(const Kway::TKTime* tm, bool datestr, bool timestr)
{
   Kway::TKDate dt = Kway::TKDate::Now();
   static char buf[32];
   time_t timer;
   struct tm loctm;
   struct tm *gmt;
   loctm.tm_sec = tm->GetSec();
   loctm.tm_min = tm->GetMin();
   loctm.tm_hour= tm->GetHour();
   loctm.tm_mday= dt.GetDay();
   loctm.tm_mon = dt.GetMon()-1;
   loctm.tm_year= dt.GetEYear()-1900;
   loctm.tm_isdst=0;
   //converts date/time to time_t
   timer = mktime(&loctm);
   //converts date/time to a GMT structure
   gmt = gmtime(&timer);

   return GmtDateTimeToStr(buf, sizeof(buf), gmt, datestr, timestr);
}
/////////////////////////////////////////////////////////////////////////////
static const char  FIXCheckSumMsgTag[] = FixSOH_s "10=";
static const int   szFIXCheckSumMsgTag = sizeof(FIXCheckSumMsgTag) - 1;//-1 for '\0'
/////////////////////////////////////////////////////////////////////////////
//unix had memrchr()
//#ifdef __BORLANDC__
//Debug:Solaris hasn't memrchr
K_mf(const void*)  memrchr(const void* aBuf, int aByte, size_t aLen)
{
   const char* aStr = (const char*)aBuf + aLen;
   for(; aLen > 0; --aLen)
      if(*(--aStr) == (char)aByte)
         return aStr;
   return NULL;
}
//#endif
//---------------------------------------------------------------------------
static K_mf(int)  FixCalcCheckSum(const char* aMsgBuf, int aLen)
{
   Kway::byte cks = 0;
   for(int idx=0;  idx < aLen;  cks += (Kway::byte)aMsgBuf[idx++])
      /*do nothig*/;
   return(cks);
}
//---------------------------------------------------------------------------
static K_mf(void)  FixAddCheckSum(SFixStr& aStr)
{
   char*  MsgEnd = aStr.Msg_ + aStr.MsgLen_;
   memcpy(MsgEnd, FIXCheckSumMsgTag+1,   //+1 for skip <SOH>
                  szFIXCheckSumMsgTag-1);//-1 for <SOH>
   MsgEnd += szFIXCheckSumMsgTag-1;
   int   cks = FixCalcCheckSum(aStr.Msg_, aStr.MsgLen_);
   MsgEnd += sprintf(MsgEnd, "%03d", cks);
   *MsgEnd = FixSOH;
   ++MsgEnd;
   *MsgEnd = 0;
   aStr.MsgLen_ = (MsgEnd - aStr.Msg_);
}
}; // Stk
}; // Fix
}; // Tw
}; // Kway
/////////////////////////////////////////////////////////////////////////////
K_ctor  TStkSesFix::TStkSesFix(const char* aBeginString)
   : inherited()
   , _IsRecvChecking(false)
   , _RPos(0)
   , _RPosLastCheck(NULL)
   , _CloseOnTxEmpty(false)
   , Timer_(this)
   , SenderTimer_(this)
   , TargetTimer_(this)
   , _IsLogonOK(false)
   , _Log(true)
   , Counter_(0)
   , LogonTimer_(this)
   , FIXVersion_(aBeginString)
{
   Timer_.Stop();

   memset(&_RecvBuf, 0, sizeof(_RecvBuf));
   HeadCode = (fTag_BeginString + '0');
   _BeginStringLen = (Kway::byte)sprintf(_BeginString, "%c=%s" FixSOH_s "%d=",
                                                 HeadCode, aBeginString, fTag_BodyLength);

   _FixRecorder = new TStkFixRecorder(".\\");
   RegMsgHandler(fMsgType_TestRequest_s, fem_TestReq,this);
   RegMsgHandler(fMsgType_Heartbeat_s,   fem_Heartbeat,this);

}
//---------------------------------------------------------------------------
K_dtor TStkSesFix::~TStkSesFix()
{
   if(_FixRecorder)
      delete _FixRecorder;
   GetFixSesMgr().UnRegTradSes(this);
}
//---------------------------------------------------------------------------
K_mf(void) TStkSesFix::OnMesRecvReady(Kway::TMesBase*, size_t /*size*/) 
{ 
   fprintf(stderr, "OnMesRecvReady----->\n");
   CheckRecv(); 
}
//---------------------------------------------------------------------------
K_mf(void) TStkSesFix::OnMesLinkReady(Kway::TMesBase*)
{
   GetFixSesMgr().RegTradSes(this);
   _FixRecorder->LinkTo(this);
  if(_Log == false) {
      _FixRecorder->OpenNewProfile(this);
  } else {
      _FixRecorder->OpenProfile(this);
  }
  //Auto-Logon
  if(_IsLogonOK == false) {
     /*
     Kway::TMesBase* Mes_ = GetMes();
     fprintf(stderr, "OnMesLinkReady()-->Mes_=%p\n", Mes_);
     if(Mes_!=NULL && Mes_->IsLinkReady()) {
        SendLogon();
     }
     */
     fprintf(stderr, "Start LogonTimer()----->1\n");
     StartLogonTimer(3);
  }
}
//-----------------------------------------------------------------------
K_mf(void)  TStkSesFix::OnMesLinkBroken(Kway::TMesBase*, const std::string& reason)
{
   GetFixSesMgr().UnRegTradSes(this);
   if(_Log == true)
      _FixRecorder->WriteLog(this, "Session Link Broken");
   
   if(_HeartBtInt) {
     StopSenderTimer();
     StopTargetTimer();
   }
   SetLogoutOK();
   _FixRecorder->Unlink(this);
   DelMsgHandler(fMsgType_TestRequest_s, fem_TestReq);
   DelMsgHandler(fMsgType_Heartbeat_s, fem_Heartbeat);
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::OnMesLinkFail(Kway::TMesBase*, const std::string& reason)
{
   SetLogoutOK();
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkSesFix::SetMsgHandler(const char* aMsgType, TFixEvMask evm, TFixMsgEv* aHandler)
{
   std::string MsgType(aMsgType);
   TRegEvsI i = _RegEvs.find(MsgType);
   if(aHandler == NULL) {
      if(i != _RegEvs.end())
         _RegEvs.erase(i);
      else
         return false;
   } else {
      if(i == _RegEvs.end())
         _RegEvs.insert(TRegEvsV(MsgType, TRegEvPair(evm, aHandler)));
      else
         return false;
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkSesFix::RegMsgHandler(const char* aMsgType, TFixEvMask evm, TFixMsgEv* aMsgHandler)
{
   return SetMsgHandler(aMsgType, evm, aMsgHandler);
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkSesFix::DelMsgHandler(const char* aMsgType, TFixEvMask evm)
{
   return SetMsgHandler(aMsgType, evm, NULL);
}
//---------------------------------------------------------------------------
K_mf(int)  TStkSesFix::CheckBeginStr(const char* aMsgBegin, int aMsgLen)
{
   if(aMsgLen < _BeginStringLen)
      return 0;
   char* aMsgEnd      = (char*)aMsgBegin + aMsgLen;
   char* FindBeginStr = std::search((char*)aMsgBegin, aMsgEnd,
                               _BeginString, _BeginString+_BeginStringLen);
   if(FindBeginStr == aMsgEnd) {
      char*  NewHead = (char*)memchr(aMsgBegin+1, _BeginString[0], aMsgLen-1);
      return ( NewHead==0 ? (0-aMsgLen) : (aMsgBegin-NewHead));
   }
   return(FindBeginStr - aMsgBegin + 1);
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::CheckRecv()
{
   //fprintf(stderr, "===> <====\n");
   if(_IsRecvChecking)
      return;
   _IsRecvChecking = true;

   try {
   char*  Buf = _RecvBuf.Msg_;
   const int  MaxLen = sizeof(_RecvBuf.Msg_);
   Kway::TMesBase*  Mes_ = GetMes();
   
   //for(int count=0; count<20; count++) {
   if(Mes_ == NULL) {
      _IsRecvChecking = false;
      return;
      //break;
   }
   //memset(Buf+_RPos, 0, MaxLen-_RPos-1);
   //static char  TestBuf[4096+1];
   int  rdsz = Mes_->Recv(Buf + _RPos, MaxLen - _RPos - 1);
   if(rdsz <= 0) {
      _IsRecvChecking = false;
      return;
      //break;
   }
   //memcpy(TestBuf, Buf+_RPos, rdsz);
   //TestBuf[rdsz] = 0;
   _RPos += rdsz;
      
   while(1) {
      if(_RPos > MaxLen - 1) {
         Buf[MaxLen] = 0;
         _FixRecorder->WriteErrLog(this, "Recv Buffer Overrun", Buf); 
         _RPos = 0;
         _RPosLastCheck = NULL;
         break;
      }
      if(_RPosLastCheck == NULL) {
         int  BeginResult = CheckBeginStr(Buf, _RPos);
         if(BeginResult == 0)
            break;
         if(BeginResult < 0) {
            BeginResult = (-BeginResult);
            _GiveupBytes += BeginResult;
            if((_RPos -= BeginResult) > 0)
               memcpy(Buf, Buf + BeginResult, _RPos);
            //break;
            continue;
         }
         _RPosLastCheck = Buf + _BeginStringLen;
         if(--BeginResult > 0) {
            _RPos          -= BeginResult;
            _RPosLastCheck -= BeginResult;
            _GiveupBytes   += BeginResult;
            memcpy(Buf, Buf + BeginResult, _RPos);
         }
      }
      char* MsgEnd = Buf + _RPos;
      int   NewLen = (MsgEnd - _RPosLastCheck);
      if(NewLen < 0) {
         *MsgEnd = 0;
         _FixRecorder->WriteErrLog(this, "Internal Error : NewLen < 0", Buf);
         _RPos = 0;
         _RPosLastCheck = NULL;
         break;
      }
      if(NewLen <= szFIXCheckSumMsgTag)
          break;
      char*  ChkSumPos = std::search<char*, char*>(_RPosLastCheck, MsgEnd,
                            (char*)FIXCheckSumMsgTag, (char*)FIXCheckSumMsgTag+szFIXCheckSumMsgTag);
      char*  LastSOH;
      if(ChkSumPos == MsgEnd) {
         LastSOH = (char*)memrchr(_RPosLastCheck, FixSOH, NewLen);
         if(LastSOH)
            _RPosLastCheck = LastSOH;
         break;
      } else {
         _RPosLastCheck = ChkSumPos;
         ++ChkSumPos;
         if(MsgEnd - ChkSumPos - szFIXCheckSumMsgTag-1 <= 0)
           break;
         LastSOH = (char*)memchr(ChkSumPos + szFIXCheckSumMsgTag,
                                 FixSOH, MsgEnd - ChkSumPos - szFIXCheckSumMsgTag);
         if(LastSOH == NULL) {
           //int i = 0;
           break;
         }
         ++_RPosLastCheck;
         NewLen = (LastSOH - Buf);
         char*  NextBegin = Buf + NewLen + 1; 
         *LastSOH = 0;
         _RecvBuf.MsgLen_ = NewLen;
         int  cks = FixCalcCheckSum(Buf, ChkSumPos - Buf);
         if(cks == atoi(ChkSumPos+szFIXCheckSumMsgTag-1)) {
            int   BodyLen = atoi(Buf + _BeginStringLen);
            char* AfBody  = (char*)memchr(Buf + _BeginStringLen, FixSOH, BodyLen);
            if((AfBody + BodyLen + 1) == ChkSumPos) {
               if(_HeartBtInt)
                 StopTargetTimer();
               //if(Form2->CheckBox_view->Checked)
               //   Form2->ListBox1->Items->Add(AnsiString(_RecvBuf.Msg_, _RecvBuf.MsgLen_));
               _FixRecorder->CheckContinuousMessage(this, _RecvBuf, this);
               if(_HeartBtInt)
                  StartTargetTimer(_HeartBtInt+3);
            } else { 
               _FixRecorder->WriteErrLog(this, "BodyLength Error!", Buf); 
            }
         } else {
            _FixRecorder->WriteErrLog(this, "Check Sum Error!", Buf);
         }
         _RPosLastCheck = NULL;
         _RPos = MsgEnd - NextBegin;
         if(_RPos > 0) {
            memcpy(Buf, NextBegin, _RPos);
            //Buf[_RPos] = 0;
         } else
            break;
      }
   } //while(1);
   //} // for()
   } catch(...) {
      _IsRecvChecking = false;
   }
   _IsRecvChecking = false;
}
//===========================================================================
K_mf(void)  TStkSesFix::DoEvent(const TFixMsg& aFixMsg)
{
   if(aFixMsg.GetiMsgType() == TFixValueI(0)) {
      SendReject(aFixMsg, srrRequiredTagMissing, fTag_MsgType);
      return;
   }
   
   TRegEvsI  i=_RegEvs.find(std::string(aFixMsg.GetiMsgType()->second));
   if(i == _RegEvs.end()) {
      //SendReject(aFixMsg, srrInvalidMsgType,0);
      return;
   }
   if((i->second.second) == 0) {
      SendReject(aFixMsg, srrInvalidMsgType,0);
      return;
   }
   (*i->second.second)(this, aFixMsg, i->second.first);
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::OnTestReq(TStkSesFix* aSes, const TFixMsg& aFixMsg)
{
   //_FixRecorder->WriteErrLog(this, "OnTestReq", "OnTestReq-1");
   TFixValueI iTestID = aFixMsg.GetBodyTag(fTag_TestReqID);   // don't reject if not exists
   if(iTestID == TFixValueI(0))
      return;
   //_FixRecorder->WriteErrLog(this, "OnTestReq", "OnTestReq-2");

   const TFixTagValue  BodyValues[] = {
      TFixTagValue(fTag_TestReqID, iTestID->second),
   };
   Send(fMsgType_Heartbeat_s, BodyValues, BodyValues+numofele(BodyValues));
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::OnHeartbeat(TStkSesFix* aSes, const TFixMsg& aFixMsg)
{
   //_FixRecorder->WriteErrLog(this, "OnHeartbeat", "OnHeartbeat-1");
   TFixValueI iTestID = aFixMsg.GetBodyTag(fTag_TestReqID);   // don't reject if not exists
   if(iTestID == TFixValueI(0))
      return;
   //_FixRecorder->WriteErrLog(this, "OnHeartbeat", "OnHeartbeat-2");
   if(_TestID.empty())
      return;
   if(_TestID == iTestID->second)
     _TestID = std::string("");
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::SendTestReq()
{
   _TestID = std::string(GetUTCDateTime(true,true));//GetLocalDateTime());
   const TFixTagValue  BodyValues[] = {
      TFixTagValue(fTag_TestReqID, _TestID.c_str()),
   };
   Send(fMsgType_TestRequest_s, BodyValues, BodyValues+numofele(BodyValues));
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkSesFix::SendBody(const SFixStr& aFixBody, int aMsgSeqNum, bool isResend)
{
   SFixStr  FixStr;
   char buffer[16] = {0}; 
   //int len = (Kway::byte)sprintf(buffer, "%c=%s" FixSOH_s "%d=", HeadCode, "FIX.4.2", fTag_BodyLength); 
   int len = (Kway::byte)sprintf(buffer, "%c=%s" FixSOH_s "%d=", HeadCode, FIXVersion_.c_str(), fTag_BodyLength); 
   memcpy(FixStr.Msg_, buffer, len);
   FixStr.MsgLen_ = len; 
   //memcpy(FixStr.Msg_, _BeginString, _BeginStringLen);
   //FixStr.MsgLen_ = _BeginStringLen;
   FixStr.MsgLen_ += sprintf(FixStr.Msg_ + FixStr.MsgLen_, "%d" FixSOH_s, aFixBody.MsgLen_);
   memcpy(FixStr.Msg_ + FixStr.MsgLen_, aFixBody.Msg_, aFixBody.MsgLen_);
   FixStr.MsgLen_ += aFixBody.MsgLen_;
   FixAddCheckSum(FixStr);
   Kway::TMesBase* Mes_ = GetMes();
   if(Mes_==NULL) {
      _FixRecorder->WriteErrLog(this, "Internal Error : No <Mes> to Send FIX Message!", FixStr.Msg_);
      return false;
   }
   _FixRecorder->WriteSendLog(this, FixStr, aMsgSeqNum);
   fprintf(stderr, "SendBody---1\n");
   if(Mes_ && Mes_->IsLinkReady()) {
      fprintf(stderr, "SendBody---2, FixStr.Msg_=%s, %d, UID=%s\n", FixStr.Msg_, FixStr.MsgLen_, Mes_->GetUID().c_str());
      fprintf(stderr, "SendBody()-->Mes_=%p\n", Mes_);
      int i = Mes_->Send(FixStr.Msg_, FixStr.MsgLen_);
      fprintf(stderr, "SendBody---2, %d\n", i);
      if(_HeartBtInt)
         StartSenderTimer(_HeartBtInt);
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkSesFix::Send(const char* aMsgType, const TFixTagValue* aBegin, const TFixTagValue* aEnd, int aMsgSeqNum)
{
   int      MsgSeqNum;
   SFixStr  FixBody;

   if(aMsgSeqNum > 0)
      MsgSeqNum= aMsgSeqNum;
   else
      MsgSeqNum= _FixRecorder->FetchSeqNum(this);

   FixBody.Append(fTag_MsgType,aMsgType);
   FixBody.Append(fTag_SenderCompID, _FixRecorder->GetKwayCompID().c_str());
   FixBody.Append(fTag_TargetCompID, _FixRecorder->GetTargetCompID().c_str());
   FixBody.Append(fTag_MsgSeqNum,    MsgSeqNum);
   FixBody.Append(fTag_SendingTime,  GetFixDateTimeMs().c_str());
   //FixBody.Append(fTag_SendingTime,  GetUTCDateTime(true,true));//GetLocalDateTime());
   FixBody.Append(aBegin,aEnd);
   return SendBody(FixBody, MsgSeqNum, false);
}
//---------------------------------------------------------------------------
K_mf(bool) TStkSesFix::SendAnOrder(const char* aMsgType, std::string& str)
{
   fprintf(stderr, "SendAnOrder()--->1\n");
   int      MsgSeqNum;
   SFixStr  FixBody;
   
   MsgSeqNum = _FixRecorder->FetchSeqNum(this);

   //FixBody.Append(fTag_MsgType,aMsgType);
   //FixBody.Append(fTag_SenderCompID, _FixRecorder->GetKwayCompID().c_str());
   //FixBody.Append(fTag_TargetCompID, _FixRecorder->GetTargetCompID().c_str());
   FixBody.Append(fTag_MsgSeqNum,    MsgSeqNum);
   FixBody.Append(fTag_SendingTime,  GetFixDateTimeMs().c_str());
   //FixBody.Append(fTag_SendingTime,  GetUTCDateTime(true,true));
   
   //排除Tags
   if(FIXVersion_ == std::string("FIX.4.4")) {
      std::string  findS = std::string(FixSOH_s) + std::string("21=1");
      std::string::size_type  pos = str.find(findS);
      if(pos != std::string::npos) {
         str.erase(pos, findS.length());
      }
   }
   FixBody.Append(str);
   return SendBody(FixBody, MsgSeqNum, false);
}
//---------------------------------------------------------------------------
K_mf(bool) TStkSesFix::SendReject(const TFixMsg& aFixMsg, const char* aReason, const char* aText, TFixTag aTag)
{
   char           BufSeqNum[16];
   char           BufTagNum[16];
   TFixTagValue   BodyValues[4];
   int            BodyCount;
   BodyValues[BodyCount=0] = MakeFixTagVal(fTag_RefSeqNum, aFixMsg.GetMsgSeqNum(), BufSeqNum);
   if(aTag)
      BodyValues[++BodyCount] = MakeFixTagVal(fTag_RefTagID, aTag, BufTagNum);
   BodyValues[++BodyCount] = TFixTagValue(fTag_SessionRejectReason, const_cast<char*>(aReason));
   BodyValues[++BodyCount] = TFixTagValue(fTag_Text,                const_cast<char*>(aText));
   return Send(fMsgType_Reject_s, BodyValues, BodyValues+BodyCount+1);
}
//---------------------------------------------------------------------------
static const char* ReasonText[] = {
                           "Invalid tag number",
                           "Required tag missing",
                           "Tag not defined for this message type",
                           "Undefined Tag",
                           "Tag specified without a value",
                           "Value is incorrect (out of range) for this tag",
                           "Incorrect data format for value",
                           "Decryption problem",
                           "Signature problem",
                           "CompID problem",
                           "SendingTime accuracy problem",
                           "Invalid MsgType",
                           "XML Validation error",
                           "Tag appears more than once",
                           "Tag specified out of required order",
                           "Repeating group fields out of order",
                           "Incorrect NumInGroup count for repeating group",
                           "Non data value includes field delimiter (SOH character)",
                           };
//----------------------
K_mf(bool)  TStkSesFix::SendReject(const TFixMsg& aFixMsg, ESessionRejectReason aReason, TFixTag aTag)
{
   char  sReason[16];
   sprintf(sReason, "%d", (int)aReason);
   const char* sReasonText = ((size_t)aReason >= numofele(ReasonText) ? "Session Rject!" : ReasonText[aReason]);
   return SendReject(aFixMsg, sReason, sReasonText, aTag);
}
//---------------------------------------------------------------------------
K_mf(bool) TStkSesFix::ResendRequest(int aBeginSeqNum, int aEndSeqNum)
{
   char  BufBeginSeqNo[16], BufEndSeqNo[16];
   const TFixTagValue    BodyValues[] = {
         MakeFixTagVal(fTag_BeginSeqNo, aBeginSeqNum, BufBeginSeqNo),
//         MakeFixTagVal(fTag_EndSeqNo,   aEndSeqNum,   BufEndSeqNo),
// Full Range
         MakeFixTagVal(fTag_EndSeqNo,   0,   BufEndSeqNo),
         };
   return Send(fMsgType_ResendRequest_s, BodyValues, BodyValues+numofele(BodyValues));
}
//---------------------------------------------------------------------------
K_mf(char*) TStkSesFix::SkipAdminMessage (const SFixStr& aSrcFixStr)
{
   static const char  FIXMsgTypeTag[] = FixSOH_s "35=";
   static const int   szFIXMsgTypeTag = sizeof(FIXMsgTypeTag) - 1;
   static const char  FIXMsgTypeTag2[] = FixSOH_s "49=";
   static const int   szFIXMsgTypeTag2 = sizeof(FIXMsgTypeTag2) - 1;
   char* BodyBegin = aSrcFixStr.SearchTag(FIXMsgTypeTag, szFIXMsgTypeTag);
   char* BodyEnd   = aSrcFixStr.SearchTag(FIXMsgTypeTag2, szFIXMsgTypeTag2);
   if(BodyBegin==NULL || BodyEnd==NULL) {
      return NULL;
   }
   std::string MsgType = std::string(BodyBegin + szFIXMsgTypeTag, BodyEnd-BodyBegin-szFIXMsgTypeTag);
   static const char  FixAdminMsgTypes[][3] = {
                                       fMsgType_Logon_s,
                                       fMsgType_Logout_s,
                                       fMsgType_ResendRequest_s,
                                       fMsgType_Heartbeat_s,
                                       fMsgType_TestRequest_s,
                                       fMsgType_SequenceReset_s,
                                       fMsgType_Reject_s,
                                       };
   for(int L = 0;  L < numofele(FixAdminMsgTypes);  ++L)
      if(MsgType == std::string(FixAdminMsgTypes[L])) {
         return NULL;
      }
   return(BodyBegin+1);
}
//---------------------------------------------------------------------------
K_mf(bool) TStkSesFix::Resend (const SFixStr& aSrcFixStr)
{
   char* BodyBegin = SkipAdminMessage(aSrcFixStr);
   char* BodyEnd   = aSrcFixStr.SearchTag(FIXCheckSumMsgTag, szFIXCheckSumMsgTag);

   if(BodyBegin==NULL  ||  BodyEnd==NULL) {
      return false;
   }
   static const char  FIXSendingTimeTag[] = FixSOH_s "52=";
   static const int   szFIXSendingTimeTag = sizeof(FIXSendingTimeTag) - 1;
   char* FindIns = aSrcFixStr.SearchTag(FIXSendingTimeTag, szFIXSendingTimeTag);
   if(FindIns == NULL) {
      return false;
   }
   ++FindIns;
   SFixStr  FixBody;
   FixBody.MsgLen_ = FindIns - BodyBegin;
   memcpy(FixBody.Msg_, BodyBegin, FixBody.MsgLen_);
   FixBody.Append(fTag_PossDupFlag, "Y");
   FixBody.Append(fTag_SendingTime,  GetFixDateTimeMs().c_str());
   //FixBody.Append(fTag_SendingTime,  GetUTCDateTime(true,true));//GetLocalDateTime());
   static const char  FIXOrigSendingTimeTag[] = "122=";
   static const int   szFIXOrigSendingTimeTag = sizeof(FIXOrigSendingTimeTag) - 1;
   memcpy(FixBody.Msg_ + FixBody.MsgLen_, FIXOrigSendingTimeTag, szFIXOrigSendingTimeTag);
   FixBody.MsgLen_ += szFIXOrigSendingTimeTag;
   FindIns += szFIXSendingTimeTag;
   int   LeftLen = BodyEnd - FindIns + 1;
   memcpy(FixBody.Msg_ + FixBody.MsgLen_, FindIns, LeftLen);
   FixBody.MsgLen_ += LeftLen;
   return SendBody(FixBody, -1, true);
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::OnLogonTimer(Kway::TTimerBase*)
{
   fprintf(stderr, "OnLogonTimer()-->1\n");
   StopLogonTimer();

   if(_IsLogonOK == false) {
      fprintf(stderr, "OnLogonTimer()-->2\n");
      Kway::TMesBase* Mes_ = GetMes();
      if(Mes_!=NULL && Mes_->IsLinkReady()) {
         fprintf(stderr, "OnLogonTimer()-->2.1\n");
         SendLogon();
         //StartLogonTimer(3);
      } else {
         fprintf(stderr, "OnLogonTimer()-->2.2\n");
         StartLogonTimer(3);
      }
   } else {
      fprintf(stderr, "OnLogonTimer()-->3\n");
      StartLogonTimer(3);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TStkSesFix::OnTimer(Kway::TTimerBase* Sender/*TTimer*/)
{
   if(_CloseOnTxEmpty)
      Close();
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::OnSenderTimer(Kway::TTimerBase* Sender/*TTimer*/)
{
   if(_FixRecorder->IsLogonOK() && _IsLogonOK)
   {
     if(!_FixRecorder->NeedReqResend(this))
       Send(fMsgType_Heartbeat_s, 0, 0);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TStkSesFix::OnTargetTimer(Kway::TTimerBase* Sender/*TTimer*/)
{
   if(_FixRecorder->IsLogonOK() && _IsLogonOK) {
      if(_TestID.empty()) {
         SendTestReq();
         StartTargetTimer(_HeartBtInt+3); // 3 seconds for transmit time
      } else {
         _FixRecorder->WriteErrLog(this, "Session Close", "Over TestRequest no response");
         SetCloseOnTxEmpty();
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::OnMesSendEmpty(Kway::TMesBase*)
{
   if(_CloseOnTxEmpty)
      StartTimer(2);
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::SetHeartbeatInterval(int aSecs)
{
   fprintf(stderr, "SetHeartbeatInterval: _FixRecorder->IsLogonOK()=%s, _IsLogonOK=%s\n", _FixRecorder->IsLogonOK()?"Y":"N", _IsLogonOK?"Y":"N");
   if(_FixRecorder->IsLogonOK() && _IsLogonOK) {
      SendTestReq();
      _HeartBtInt = aSecs;
      if(_HeartBtInt) {
         StartTargetTimer(_HeartBtInt+3);
         StartSenderTimer(_HeartBtInt);
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::SetLogonOK()
{
   _IsLogonOK = true;
   DoRefresh();
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::SetLogoutOK()
{
   _IsLogonOK = false;
   DoRefresh();
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkSesFix::SettingApply(const std::string& setting)
{
   if(setting.empty()) {
      OnStateChanged("Usage:SesID;Tag56;HeartBeatInt;BrokerID;PWD;EndT;Log;");
      return false;
   }

   std::string  setstr = setting;
   std::string  tmp;
   char  buf[128] = {0};
   std::string::size_type  pos;
   const size_t  itemcnt = 7;
   size_t  i = 0;
   while(!setstr.empty()) {
      pos = setstr.find(';');
      if(i>=itemcnt || pos==std::string::npos)
         break;
      switch(i) {
      case 0:   _SenderCompID = setstr.substr(0, pos);      break;
      case 1:   _TargetCompID = setstr.substr(0, pos);      break;
      case 2:   _HeartBtInt   = atoi(setstr.substr(0, pos).c_str());     break;
      case 3:   _BrokerID     = setstr.substr(0, pos);      break;
      case 4:   _PWD          = atoi(setstr.substr(0, pos).c_str());
                if(_PWD > 9999) {
                   OnStateChanged("PWD設定錯誤,不得大於4位數!");
                   return false;
                }
                break;
      case 5:   tmp.clear();
                tmp = setstr.substr(0, pos);
                if(tmp.empty() == false)
                   _EndTime.assign(tmp);
                break;
      case 6:   _Log = atoi(setstr.substr(0, pos).c_str());   break;
      }
      setstr = setstr.substr(pos+1);
      ++i;
   }

   if(i < itemcnt-1) {
      OnStateChanged("Usage:SesID;Tag56;HeartBeatInt;BrokerID;PWD;EndT;Log;");
      return false;
   }

   if(_SenderCompID.empty()) {
      OnStateChanged("SesID不可為空值!");
      return false;
   }
   if(_TargetCompID.empty()) {
      OnStateChanged("Tag56不可為空值!");
      return false;
   }
   if(_BrokerID.empty()) {
      OnStateChanged("BrokerID不可為空值!");
      return false;
   }

   if(GetFixSesMgr().IsSesIDExist(this)) {
      if(!GetFixSesMgr().IsOnlineSesID(this)) {
         OnStateChanged("重覆設定: SesID之前已存在!");
         return false;
      }
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TStkSesFix::DoRefresh()
{
   static char  buf[256];
   sprintf(buf, "%s %s - Recv(%d);Cont(%d);Send(%d); Ord(%d)", _SenderCompID.c_str(), _IsLogonOK?"已登入":"未登入",
                _FixRecorder->GetLastRecvSeqNum(), _FixRecorder->GetLastContSeqNum(), _FixRecorder->GetLastSendSeqNum(), Counter_);
   OnStateChanged(buf);
}
//-----------------------------------------------------------------------
K_mf(void)  TStkSesFix::SendLogon()
{
   _FixRecorder->SetKwayCompID  (_SenderCompID);
   _FixRecorder->SetTargetCompID(_TargetCompID);
   //SetHeartbeatInterval(_HeartBtInt);
   std::string  tag96 = GetRawData();

   char  bufHeartBt[10] = {0};
   const TFixTagValue  BodyValues[] = {
         TFixTagValue(fTag_EncryptMethod, "0"),
         MakeFixTagVal(fTag_HeartBtInt, _HeartBtInt, bufHeartBt),
         TFixTagValue(fTag_RawDataLength, "5"),
         TFixTagValue(fTag_RawData, tag96.c_str()),
         //TFixTagValue (fTag_ResetSeqNumFlag, "N"),
   };
   Send(fMsgType_Logon_s, BodyValues, BodyValues+numofele(BodyValues));
}
//-----------------------------------------------------------------------
K_mf(std::string)  TStkSesFix::GetAppendNo()
{
   int  randomNum = rand()%1000==0 ? 1 : rand()%1000;
   char buf[4] = {0};
   sprintf(buf, "%03d", randomNum);
   std::string  appendNo(buf);
   int value     = randomNum * _PWD; // 999 * 9999 = 9989001
   char buf1[8] = {0};
   sprintf(buf1, "%07d", value);
   std::string key = buf1;
   appendNo += key.substr(3, 2); //9989001
   return std::string(appendNo);
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkSesFix::CheckLast50(std::string& a)
{
   std::string  a3 = a.substr(0,3);
   TKIniFile  *_ini;
   std::string  tmp = "./Settings/AppendNo.ini";
   _ini = new TKIniFile(tmp.c_str());
   char  sid[10] = {0};
   sprintf(sid, "%s", _SenderCompID.c_str());

   char buf[7] = {0};

   for(int i=1; i<51; ++i) {
      sprintf(buf, "Last%d", i);
      tmp = std::string(_ini->ReadString(sid, buf, "").c_str());
      if(tmp == a3)
         return false;
   }
   int num = _ini->ReadInt(sid, "count", 0);
   ++num;
   if(num > 50)
      num = 1;
   sprintf(buf, "Last%d", num);
   _ini->WriteInt(sid, "count", num);
   _ini->WriteString(sid, buf, a3.c_str());
   delete _ini;
   return true;
}
//-----------------------------------------------------------------------
K_mf(std::string)  TStkSesFix::GetRawData()
{
   /*
   Kway::TKTimeMS  timeObj = Kway::TKTimeMS::Now();
   Kway::word  ms = timeObj.GetMS();
   while(ms == 0) {
      timeObj = Kway::TKTimeMS::Now();
      ms = timeObj.GetMS();
   }
   */
   std::string  appendNo = GetAppendNo();
   while(CheckLast50(appendNo) == false) {
      appendNo = GetAppendNo();
   }
   return std::string(appendNo);
}
//-----------------------------------------------------------------------
K_mf(void) TStkSesFix::SendLogout()
{
   Send(fMsgType_Logout_s, 0, 0);
}
//-----------------------------------------------------------------------
K_mf(void) TStkSesFix::SendHeartBt()
{
   Send(fMsgType_Heartbeat_s, 0, 0);
}
//-----------------------------------------------------------------------
K_mf(void)  TStkSesFix::SendOrder(std::string& ord)
{
   fprintf(stderr, "TStkSesFix::SendOrder()--->1\n");
   Kway::Nsa::TTimeStamp  time = Kway::Nsa::TTimeStamp::Now();
   if(_EndTime.IsInvalid() == false) {
      fprintf(stderr, "TStkSesFix::SendOrder()--->2\n");
      if(time-_EndTime >= 0) {
         fprintf(stderr, "TStkSesFix::SendOrder()--->return\n");
         return;
      }
   }
   fprintf(stderr, "TStkSesFix::SendOrder()--->3\n");
   Kway::TMesBase* Mes_ = GetMes();
   if(Mes_) {
      fprintf(stderr, "TStkSesFix::SendOrder()--->4\n");
      SendAnOrder(fMsgType_NewOrderSingle_s, ord);
      ++Counter_;
   }
   fprintf(stderr, "TStkSesFix::SendOrder()--->5\n");
}
//-----------------------------------------------------------------------
