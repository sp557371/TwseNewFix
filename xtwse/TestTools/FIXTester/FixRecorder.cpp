//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include <stdio.h>
#include "FixRecorder.h"
#include "SesFix.h"
#include "FixAdminMsg.h"
//#include <IniFiles.hpp>
#include "KwayBase.h"
#include "KTime.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Fix::Stk;
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
const int   Pos_Gap      =      0;
const int   Pos_GapFill  =     -1;
const int   Pos_GapSkip  =     -2;
const int   Pos_GapErr   =     -3;
//const char  FixLogPath[] = "\\StkFIXLog\\%4y\\%2m\\%2d";
/////////////////////////////////////////////////////////////////////////////
static K_mf(void)  StrMakePath(std::string& aPath)
{
   switch(aPath[aPath.length()-1]) {
   case '/' :
   case '\\':  break;
   default:    aPath += "\\";     break;
   }
}
/////////////////////////////////////////////////////////////////////////////
K_ctor  TStkFixRecorder::TStkFixRecorder(const std::string& path)
   : _LastRecvSeqNum(0)
   , _LastContSeqNum(0)
   , _LastSendSeqNum(0)
   , _KwayCompID("KWAY")
   , _TargetCompID("BROKER")
   , _OrgPath(path)
   , _Counter(0)
{
   char  buf[128] = {0};
   sprintf(buf,"./Logs/%s/",TKDate::Now().as_string(TKDate::sf_ELong, 0).c_str());
   _FixLogPath = buf;
}
//---------------------------------------------------------------------------
K_dtor TStkFixRecorder::~TStkFixRecorder ()
{
}
//---------------------------------------------------------------------------
K_mf(bool) TStkFixRecorder::LinkTo(TStkSesFix* aSes)
{
   _TradeID = aSes->GetSesID();

   //std::string  fn = _FixLogPath + _TradeID + "/Tag11Map.bin";
   //Map_ = new TTag11Map();
   //Recorder_ = new TTag11Recorder(*Map_, fn.c_str());

   aSes->RegMsgHandler(fMsgType_Logon_s,         fem_Logon,     this);
   aSes->RegMsgHandler(fMsgType_Logout_s,        fem_Logout,    this);
   aSes->RegMsgHandler(fMsgType_SequenceReset_s, fem_SeqReset,  this);
   aSes->RegMsgHandler(fMsgType_ResendRequest_s, fem_ResendReq, this);
   aSes->RegMsgHandler(fMsgType_Reject_s,        fem_SessReject,this);
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TStkFixRecorder::Unlink (TStkSesFix* aSes)
{
   aSes->DelMsgHandler(fMsgType_Logon_s,         fem_Logon);
   aSes->DelMsgHandler(fMsgType_Logout_s,        fem_Logout);
   aSes->DelMsgHandler(fMsgType_SequenceReset_s, fem_SeqReset);
   aSes->DelMsgHandler(fMsgType_ResendRequest_s, fem_ResendReq);
   aSes->DelMsgHandler(fMsgType_Reject_s,        fem_SessReject);
   CloseProfile(aSes);
   /*
   if(Recorder_) {
      delete Recorder_;
      Recorder_ = NULL;
   }
   if(Map_) {
      delete Map_;
      Map_ = NULL;
   }
   */
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkFixRecorder::IsLogonOK()
{
   return(_Loger.IsOpened()  &&  _RecvIdx.IsOpened()  &&  _SendIdx.IsOpened());
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::WriteTitle(TStkSesFix* aSes, const char* aMsg)
{
  static char buf[512];
  sprintf(buf, "******************** [%s] ********************", aMsg);
  if(aSes->GetWriteOrNot())
     WriteLog(aSes, buf);
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::WriteErrLog(TStkSesFix* aSes, const char* remark, const char* data)
{
  static char  buf[1024*5];
  sprintf(buf, "[ERR] : %s - %s", remark, data);
  if(aSes->GetWriteOrNot())
     WriteLog(aSes, buf);
}
//---------------------------------------------------------------------------
K_mf(TStkFixRecorder::SIdxRec)  TStkFixRecorder::WriteLog(TStkSesFix* aSes, const char* aRemark)
{
   Kway::TFile*   Loger;
   SIdxRec  iRec = { 0, 0 };
   if(IsLogonOK()) {
      Loger = &_Loger;
   } else {
      static Kway::TFile  sLoger;
      if(!sLoger.IsOpened()) {
         std::string LogPath = _OrgPath + _FixLogPath;
         StrMakePath(LogPath);
         LogPath += "FIX_Pre.log";
         sLoger.Open(LogPath.c_str(), Kway::TFileMode(Kway::fmCreatePath|Kway::fmOpenAlways|Kway::fmAppend));
         if(!sLoger.IsOpened())
            return iRec;
      }
      Loger = &sLoger;
   }
   std::string  logstr = GetLocalDateTime() + std::string(" ");
   //std::string logstr = TKDateTimeMS::Now().as_string(TKDate::sf_ELong) + " ";
   iRec._Pos = Loger->Seek(0,Kway::fsEnd) + logstr.length();
   iRec._Len = strlen(aRemark);
   logstr += aRemark;
   logstr += "\n";
   size_t   logsz = logstr.length();
   if(Loger->Write(logstr.c_str(), logsz) != logsz)
      iRec._Pos = Pos_GapErr;
   return iRec;
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::WriteLogIdx(TStkSesFix* aSes, int aMsgSeqNum, Kway::TFile& aIdx)
{
   if(aMsgSeqNum > 0  &&  aIdx.IsOpened()) {
      aIdx.Seek(0, Kway::fsBegin);
      char  buf[11] = {0};
      sprintf(buf, "%10d", aMsgSeqNum);
      aIdx.Write(buf, strlen(buf));
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::WriteLogIdx(TStkSesFix* aSes, const SFixStr& aFix, int aMsgSeqNum, Kway::TFile& aIdx)
{
   WriteAdminMsg(aSes, aFix);
   SIdxRec  iRec = WriteLog(aSes, aFix.Msg_);
   if(aMsgSeqNum > 0  &&  aIdx.IsOpened()) {
      aIdx.Seek(sizeof(iRec) * aMsgSeqNum, Kway::fsBegin);
      aIdx.Write(&iRec, sizeof(iRec));
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::FillSeqGap (int aSeqBegin, int aSeqEnd)
{
   int  fBufN = aSeqEnd - aSeqBegin + 1;
   if(fBufN < 0)
      return;
   
   SIdxRec*  fBuf = new SIdxRec[fBufN];
   _RecvIdx.Seek(sizeof(SIdxRec) * aSeqBegin, Kway::fsBegin);
   size_t  szRec = _RecvIdx.Read(fBuf, sizeof(SIdxRec)*fBufN) / sizeof(SIdxRec);
 
   size_t  L = 0;
   for(L = 0; L < szRec; ++L)
      if(fBuf[L]._Pos == Pos_Gap)
         fBuf[L]._Pos =  Pos_GapFill;
   for( ; L < fBufN; ++L)
      fBuf[L]._Pos = Pos_GapFill;
    
   _RecvIdx.Seek(sizeof(SIdxRec) * aSeqBegin, Kway::fsBegin);
   _RecvIdx.Write(fBuf, sizeof(SIdxRec)*fBufN);
   delete [] fBuf;
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::OnSeqReset(TStkSesFix*, const TFixMsg&)
{  
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::OnResendReq(TStkSesFix* aSes, const TFixMsg& aFixMsg)
{
   const char*  sBeginSeqNo = NULL;
   const char*  sEndSeqNo   = NULL;
   for(TFixValueI L = aFixMsg.GetiBodyBegin(); L != aFixMsg.GetiBodyEnd(); ++L)
      switch(L->first) {
      case fTag_BeginSeqNo: sBeginSeqNo = L->second;  break;
      case fTag_EndSeqNo:   sEndSeqNo   = L->second;  break;
      //default:              aSes->SendReject(aFixMsg, srrTagNotDefinedForThisMessage, L->first);
      //                      return;
      }
   MAC_Check_RequiredTagMissing(aSes, aFixMsg, sBeginSeqNo==0, fTag_BeginSeqNo);
   MAC_Check_RequiredTagMissing(aSes, aFixMsg, sEndSeqNo==0,   fTag_EndSeqNo);
   int   aBeginSeqNo = atoi(sBeginSeqNo);
   int   aEndSeqNo   = (atoi(sEndSeqNo)==0 ? _LastSendSeqNum:atoi(sEndSeqNo));
   char  BufNewSeqNo[128];
   if(aEndSeqNo > _LastSendSeqNum) {
      aSes->SendReject(aFixMsg, srrValueIsIncorrect, fTag_EndSeqNo);
      const TFixTagValue  BodyValues[] = {
                              MakeFixTagVal(fTag_NewSeqNo, _LastSendSeqNum+2, BufNewSeqNo),
      };                         
      aSes->Send(fMsgType_SequenceReset_s, BodyValues, BodyValues+numofele(BodyValues));
      return;
   }
   if(aBeginSeqNo > aEndSeqNo) {
      aSes->SendReject(aFixMsg, srrValueIsIncorrect, fTag_BeginSeqNo);
      return;
   }

   SFixStr  FixStr;
   //int      aGapBegin = 0;

   const TFixTagValue  BodyValues[] = {
			// 2012/11/28 miguel 刪除 tag 43 (fTag_PossDupFlag)
	  //TFixTagValue (fTag_PossDupFlag, "Y"),  
			// 2012/11/28 end
	  TFixTagValue (fTag_GapFillFlag, "Y"),
      MakeFixTagVal(fTag_NewSeqNo,    aEndSeqNo+1, BufNewSeqNo),
      //MakeFixTagVal(fTag_NewSeqNo,    _LastSendSeqNum+1, BufNewSeqNo),
   };
   aSes->Send(fMsgType_SequenceReset_s, BodyValues, BodyValues+numofele(BodyValues), aBeginSeqNo);

   /*
   const char* LogErrMsg;
   for(;  aBeginSeqNo <= aEndSeqNo;  ++aBeginSeqNo)
   {
      SIdxRec  iRec;
      if(_SendIdx.Seek(aBeginSeqNo*sizeof(iRec), Kway::fsBegin) == -1)
      {
         LogErrMsg = "_SendIdx.Seek()";
__LOG_ERR:
         sprintf(BufNewSeqNo, "ResendRequest(%d) %s", aBeginSeqNo, LogErrMsg);
         WriteErrLog(aSes, "Internal Error : ", BufNewSeqNo);
         if(aGapBegin==0)
            aGapBegin = aBeginSeqNo;
         continue;
      }
      if(_SendIdx.Read(&iRec, sizeof(iRec)) != sizeof(iRec))
      {
         LogErrMsg = "_SendIdx.Read()";
         goto __LOG_ERR;
      }
      if(iRec._Pos <= 0)
      {
         LogErrMsg = "iRec._Pos <= 0";
         goto __LOG_ERR;
      }
      if(_Loger.Seek(iRec._Pos, Kway::fsBegin) != (size_t)iRec._Pos)
      {
         LogErrMsg = "_Loger.Seek()";
         goto __LOG_ERR;
      }
      if(_Loger.Read(FixStr.Msg_, iRec._Len) != (Kway::TFile::size_type)iRec._Len)
      {
         LogErrMsg = "_Loger.Read()";
         goto __LOG_ERR;
      }
      FixStr.MsgLen_ = (int)iRec._Len;
      if(aGapBegin==0)
      {
         if(!aSes->Resend(FixStr))
            aGapBegin = aBeginSeqNo;
      }
      else if(aSes->SkipAdminMessage(FixStr) != NULL)
      {
         const TFixTagValue   BodyValues[] = {
                              TFixTagValue (fTag_GapFillFlag, "Y"),
                              MakeFixTagVal(fTag_NewSeqNo,    aBeginSeqNo, BufNewSeqNo),
         };
         aSes->Send(fMsgType_SequenceReset_s, BodyValues, BodyValues+numofele(BodyValues), aGapBegin);
         if(!aSes->Resend(FixStr))
            aGapBegin = aBeginSeqNo;
         else
            aGapBegin = 0;
      }
   } 
   if(aGapBegin != 0)
   {
      const TFixTagValue   BodyValues[] = {
                           TFixTagValue (fTag_GapFillFlag, "Y"),
                           TFixTagValue (fTag_PossDupFlag, "Y"),  
			   MakeFixTagVal(fTag_NewSeqNo,    _LastSendSeqNum+1, BufNewSeqNo),
      };
      aSes->Send(fMsgType_SequenceReset_s, BodyValues, BodyValues+numofele(BodyValues), aGapBegin);
   }
   */
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::OnSessReject(TStkSesFix* aSes, const TFixMsg& aFixMsg)
{
  // 收到Reject, 只記錄不處理*
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::OnLogout(TStkSesFix* aSes, const TFixMsg&)
{
   aSes->SetLogoutOK();
   aSes->Send(fMsgType_Logout_s, 0, 0);
   _Loger.Close();
   _RecvIdx.Close();
   _SendIdx.Close();
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::OnLogon(TStkSesFix* aSes, const TFixMsg& aFixMsg)
{
   fprintf(stderr, "OnLogon---->1\n");

   TFixValueI  HeartBtIntI = aFixMsg.GetBodyTag(fTag_HeartBtInt, aSes);
   if(HeartBtIntI == TFixValueI(0))
      return;

   fprintf(stderr, "OnLogon---->2\n");
   int  HeartBtInt = atoi(HeartBtIntI->second);

   _KwayCompID  = aFixMsg.GetiTargetCompID()->second;
   _TargetCompID= aFixMsg.GetiSenderCompID()->second;

   if(aFixMsg.GetMsgSeqNum() < _LastRecvSeqNum) {
      fprintf(stderr, "OnLogon---->34=%d, LastRecv=%d, LastCont=%d\n", aFixMsg.GetMsgSeqNum(), _LastRecvSeqNum, _LastContSeqNum);
      aSes->SendReject(aFixMsg, srrValueIsIncorrect, aFixMsg.GetMsgSeqNum());
      //LogonFailure(aSes, aFixMsg, "Logon failure(MsgSeqNum is less then expected)!");
      return;
   }
   fprintf(stderr, "OnLogon---->3\n");
   aSes->SetLogonOK();
   aSes->SetHeartbeatInterval(HeartBtInt);
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::FirstMessage(TStkSesFix* aSes,const TFixMsg& aFixMsg, TFixEv* aMsgHandler)
{
   fprintf(stderr, "CheckContinuousMessage---->2.1 FirstMessage\n");
   TFixValueI  MsgType = aFixMsg.GetiMsgType();
   if(MsgType==TFixValueI(0) || std::string(MsgType->second)!=std::string(fMsgType_Logon_s))  {
      aSes->SendReject(aFixMsg, srrInvalidMsgType, aFixMsg.GetMsgSeqNum());
      fprintf(stderr, "CheckContinuousMessage---->2.11 FirstMessage 35!=A\n");
      SetLastRecvSeqNum(aSes, aFixMsg.GetMsgSeqNum());
      return;
   }
   
   (*aMsgHandler)(aFixMsg);
   if(!aSes->IsLogonOK()) {
      fprintf(stderr, "CheckContinuousMessage---->2.12 FirstMessage, logon fail log file no open ...\n");
      SetLastRecvSeqNum(aSes, aFixMsg.GetMsgSeqNum());
      return;
   }

   int  MsgSeqNum = aFixMsg.GetMsgSeqNum();
   SetLastRecvSeqNum(aSes, MsgSeqNum);
   fprintf(stderr, "CheckContinuousMessage---->2.2 FirstMessage, 34=%d, LastRecv=%d, LastCont=%d\n", aFixMsg.GetMsgSeqNum(), _LastRecvSeqNum, _LastContSeqNum);
   if(aSes->GetWriteOrNot()) {
      fprintf(stderr, "CheckContinuousMessage---->2.3 FirstMessage\n");
      SIdxRec  iRec;
      iRec._Pos = Pos_GapSkip;
      _RecvIdx.Seek(sizeof(iRec) * MsgSeqNum, Kway::fsBegin);
      _RecvIdx.Write(&iRec, sizeof(iRec));
   }
   int ExpectSeq = _LastContSeqNum+1;
   if(MsgSeqNum == ExpectSeq) {
      SetLastContSeqNum(aSes, _LastContSeqNum+1);
      fprintf(stderr, "CheckContinuousMessage---->2.4 FirstMessage -> SetLastContSeqNum=%d\n", _LastContSeqNum);
   }
   else if(MsgSeqNum >  ExpectSeq)  {
          fprintf(stderr, "CheckContinuousMessage---->2.5 FirstMessage -> ResendRequest\n");
	  aSes->ResendRequest(ExpectSeq, MsgSeqNum-1);
        }
}
//===========================================================================
K_mf(void) TStkFixRecorder::CheckContinuousMessage(TStkSesFix* aSes, SFixStr& aFixStr, TFixEv* aMsgHandler)
{
   fprintf(stderr, "CheckContinuousMessage---->1 34=%d, LastRecv=%d, LastCont=%d\n", aFixStr.GetMsgSeqNum(), _LastRecvSeqNum, _LastContSeqNum);
   std::string  MsgType1(fMsgType_SequenceReset_s);

   TFixMsg  FixMsg;
   int  MsgSeqNum = aFixStr.GetMsgSeqNum();
   if(MsgSeqNum <= _LastContSeqNum) {
      aSes->SendReject(FixMsg, srrValueIsIncorrect, MsgSeqNum);
      fprintf(stderr, "CheckContinuousMessage---->1.1 MsgSeqNum(%d) <= _LastContSeqNum(%d)\n", aFixStr.GetMsgSeqNum(), _LastContSeqNum);
      WriteRecvLog(aSes, aFixStr, -1);
      SetLastRecvSeqNum(aSes, MsgSeqNum);
      return;
   }
   
   WriteRecvLog(aSes, aFixStr, MsgSeqNum);
   if(!FixMsg.Assign(aSes, aFixStr)) { 
      fprintf(stderr, "CheckContinuousMessage---->1.2\n");
      return;
   }

   TFixValueI  MsgType = FixMsg.GetiMsgType();
   // 2013/11/15 change to check Sesssion logon flag
   //if(!IsLogonOK()) {
   if(!aSes->IsLogonOK()) {
      fprintf(stderr, "CheckContinuousMessage---->2\n");
      FirstMessage(aSes, FixMsg, aMsgHandler);
      return;
   } 
   
   if(MsgType!=TFixValueI(0) &&  std::string(MsgType->second)==std::string(fMsgType_ResendRequest_s)) {
      fprintf(stderr, "CheckContinuousMessage---->3 Appia send ResendRequest\n");
      OnResendReq(aSes, FixMsg);
      return;
   }

   TFixValueI  NewSeqNoI = TFixValueI(0);
   int         NewSeqNo;
   if(MsgType!=TFixValueI(0) &&  std::string(MsgType->second)==std::string(fMsgType_SequenceReset_s)) {
      fprintf(stderr, "CheckContinuousMessage---->4 Appia send SequenceReset\n");
      NewSeqNoI = FixMsg.GetBodyTag(fTag_NewSeqNo);
      if(NewSeqNoI == TFixValueI(0)) { 
         fprintf(stderr, "CheckContinuousMessage---->4.1 SequenceReset, NewSeqNoI == TFixValueI(0)\n");
	 return;
      }
      NewSeqNo = atoi(NewSeqNoI->second);
      TFixValueI  GapFillFlag = FixMsg.GetBodyTag(fTag_GapFillFlag);
      if(GapFillFlag==TFixValueI(0)  ||  *(GapFillFlag->second)=='N') {
         if(NewSeqNo <= _LastRecvSeqNum) {
            fprintf(stderr, "CheckContinuousMessage---->4.2 SequenceReset, send SendReject, NewSeqNo(%d) <= _LastRecvSeqNum(%d)\n", NewSeqNo, _LastRecvSeqNum);
	    aSes->SendReject(FixMsg, srrValueIsIncorrect, fTag_NewSeqNo);
         } else {
            --NewSeqNo;
            if(aSes->GetWriteOrNot())
               FillSeqGap(_LastContSeqNum+1, NewSeqNo);
            SetLastRecvSeqNum(aSes, NewSeqNo);
            SetLastContSeqNum(aSes, NewSeqNo);
         }
         return;
      }
      if(*(GapFillFlag->second) != 'Y') {
         fprintf(stderr, "CheckContinuousMessage---->4.3 FIXTester send SendReject\n");
	 aSes->SendReject(FixMsg, srrValueIsIncorrect, fTag_GapFillFlag);
         return;
      }
      if(NewSeqNo <= MsgSeqNum) {
         fprintf(stderr, "CheckContinuousMessage---->4.4 FIXTester send SendReject\n");
	 aSes->SendReject(FixMsg, srrValueIsIncorrect, fTag_NewSeqNo);
         return;
      }
   }

   int  ExpectLastSeq = _LastRecvSeqNum+1;
   if(MsgSeqNum >= ExpectLastSeq)
      SetLastRecvSeqNum(aSes, MsgSeqNum);
   
   // 2013/11/15 add -ne fMsgType_Logout_s condition 
   if(MsgSeqNum > ExpectLastSeq && std::string(MsgType->second)!=std::string(fMsgType_Logout_s)) {
      fprintf(stderr, "CheckContinuousMessage---->5 FIXTester send ResendRequest\n");
      aSes->ResendRequest(ExpectLastSeq, MsgSeqNum-1);
      return;
   }
   // 2012/09/20 miguel 刪除判斷
   //if(MsgSeqNum != _LastContSeqNum+1)
   //   return;
   // 2012/09/20 end
   if(NewSeqNoI != TFixValueI(0)) {
      --NewSeqNo;
      if(aSes->GetWriteOrNot())
         FillSeqGap(_LastContSeqNum+1, NewSeqNo);
      SetLastRecvSeqNum(aSes, NewSeqNo);
      SetLastContSeqNum(aSes, NewSeqNo);
      MsgSeqNum       = NewSeqNo;
   } else {
      SetLastContSeqNum(aSes, MsgSeqNum);
      fprintf(stderr, "CheckContinuousMessage---->6 34=%d, LastRecv=%d, LastCont=%d\n", MsgSeqNum, _LastRecvSeqNum, _LastContSeqNum);
      (*aMsgHandler)(FixMsg);
   }

   while(_LastContSeqNum < _LastRecvSeqNum) {
      if(aSes->GetWriteOrNot() == false) {
         SetLastContSeqNum(aSes, MsgSeqNum);
      } else {
         SIdxRec  iRec;
         ++MsgSeqNum;
         _RecvIdx.Seek(MsgSeqNum*sizeof(iRec), Kway::fsBegin);
         _RecvIdx.Read(&iRec, sizeof(iRec));
         switch(iRec._Pos) {
         case Pos_Gap:		
         case Pos_GapErr:	
            return;		
         case Pos_GapSkip:	
         case Pos_GapFill:
            SetLastContSeqNum(aSes, MsgSeqNum);
            break;
         default:		
            SetLastContSeqNum(aSes, MsgSeqNum);
           _Loger.Seek(iRec._Pos, Kway::fsBegin);
           aFixStr.MsgLen_ = static_cast<int>(_Loger.Read(aFixStr.Msg_, iRec._Len));
           if(FixMsg.Assign(aSes, aFixStr))
              (*aMsgHandler)(FixMsg);
           break;
         }
      }
   }
}
//===========================================================================
K_mf(int)  TStkFixRecorder::FetchSeqNum(TStkSesFix* aSes)
{
   ++_LastSendSeqNum;
   SetLastSendSeqNum(aSes, _LastSendSeqNum);
   return _LastSendSeqNum;
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkFixRecorder::NeedReqResend(TStkSesFix* aSes)
{
   int  From = _LastContSeqNum+1;
   int  To   = _LastRecvSeqNum-1;
   if(To-From < 0)
      return false;
   aSes->ResendRequest(From,To);
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::WriteAdminMsg(TStkSesFix* aSes, const SFixStr& aFix)
{
   std::string  msg;

   SFixStr  FixMsg;
   memcpy(&FixMsg, &aFix, sizeof(FixMsg));
   
   TFixMsg  Fix;
   if(!Fix.Assign(aSes, FixMsg))
      return;
      
   TFixValueI  iter = Fix.GetiMsgType();
   std::string msgType = std::string(iter->second);

   if(msgType == std::string(fMsgType_TestRequest_s)) {
      msg = "Test Request : ";
      TFixValueI  vai = Fix.GetBodyTag(fTag_TestReqID);
      if(vai != TFixValueI(0))
         msg += " TestReqID (" + std::string(vai->second) + ")";
   } else if(msgType == std::string(fMsgType_Heartbeat_s)) {
      msg = "Heartbeat : ";
      TFixValueI  vai = Fix.GetBodyTag(fTag_TestReqID);
      if(vai != TFixValueI(0))
         msg += "TestReqID (" + std::string(vai->second) + ")";
   } else if(msgType == std::string(fMsgType_ResendRequest_s)) {
      msg = "Resend Request : ";
      TFixValueI  bseq = Fix.GetBodyTag(fTag_BeginSeqNo);
      TFixValueI  eseq = Fix.GetBodyTag(fTag_EndSeqNo);
      if(bseq != TFixValueI(0))
         msg += " BeginSeqNo (" + std::string(bseq->second) + ")";
      if(eseq != TFixValueI(0))
         msg += " EndSeqNo (" + std::string(eseq->second) + ")";
   } else if(msgType == std::string(fMsgType_SequenceReset_s)) {
      msg = "Sequence Reset : ";
      TFixValueI  gapfill= Fix.GetBodyTag(fTag_GapFillFlag);
      TFixValueI  newseq = Fix.GetBodyTag(fTag_NewSeqNo);
      if(gapfill != TFixValueI(0))
         msg += " GapFillFlag (" + std::string(gapfill->second) + ")";
      if(newseq != TFixValueI(0))
         msg += " NewSeqNo (" + std::string(newseq->second) + ")";
   } else if(msgType == std::string(fMsgType_Reject_s)) {
      msg = "Session Reject : ";
      TFixValueI  reason= Fix.GetBodyTag(fTag_SessionRejectReason);
      TFixValueI  text  = Fix.GetBodyTag(fTag_Text);
      if(reason != TFixValueI(0))
         msg += " RejectReason (" + std::string(reason->second) + ")";
      if(text != TFixValueI(0))
         msg += " Text (" + std::string(text->second) + ")";
   } else if(msgType == std::string(fMsgType_Logon_s)) {
      msg = "Logon : ";
      TFixValueI  user  = Fix.GetBodyTag(fTag_RawData);
      TFixValueI  hbint = Fix.GetBodyTag(fTag_HeartBtInt);
      if(user != TFixValueI(0))
         msg += " LoginData (" + std::string(user->second) + ")";
      if(hbint != TFixValueI(0))
         msg += " HeartbeatInterval (" + std::string(hbint->second) + ")";
   } else if(msgType == std::string(fMsgType_Logout_s))
      msg = "Logout : ";
   else
      return;
   
   WriteTitle(aSes, msg.c_str());
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::SetLastRecvSeqNum(TStkSesFix* aSes, int num) 
{   
   aSes->DoRefresh(); 
   _LastRecvSeqNum = num; 
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::SetLastContSeqNum(TStkSesFix* aSes, int num) 
{ 
   aSes->DoRefresh(); 
   _LastContSeqNum = num; 
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::SetLastSendSeqNum(TStkSesFix* aSes, int num)
{
   aSes->DoRefresh(); 
   _LastSendSeqNum = num; 
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::CloseProfile(TStkSesFix* aSes)
{
   _Loger.Close();
   _RecvIdx.Close();
   _SendIdx.Close();
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkFixRecorder::OpenNewProfile(TStkSesFix* aSes)
{
   _LogPath= _OrgPath + _FixLogPath;
   StrMakePath(_LogPath);
   _LogPath += _TradeID;
   _LogPath += "\\";
   Kway::TFileMode logmode = Kway::TFileMode(Kway::fmCreatePath|Kway::fmOpenAlways|Kway::fmReadWrite);
   //OpenOrderInfo();
   if(_Loger.IsOpened() == false)
      _Loger.Open(std::string(_LogPath + "FIX.log").c_str(), logmode);
   if(_RecvIdx.IsOpened() == false)
      _RecvIdx.Open(std::string(_LogPath + "RecvIdx.int").c_str(), logmode);
   if(_SendIdx.IsOpened() == false)
      _SendIdx.Open(std::string(_LogPath + "SendIdx.int").c_str(), logmode);

   char  buf[11] = {0};
   char  buf1[11]= {0};
   _RecvIdx.Read(buf, 10);
   _SendIdx.Read(buf1, 10);
   int  in = atoi(buf);
   int  out= atoi(buf1);
   SetLastRecvSeqNum(aSes, in);
   SetLastContSeqNum(aSes, in);
   SetLastSendSeqNum(aSes, out);
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TStkFixRecorder::OpenProfile(TStkSesFix* aSes)
{
   _LogPath= _OrgPath + _FixLogPath;
   StrMakePath(_LogPath);
   _LogPath += _TradeID;
   _LogPath += "\\";

   Kway::TFileMode logmode = Kway::TFileMode(Kway::fmCreatePath|Kway::fmOpenAlways|Kway::fmReadWrite|Kway::fmDenyWrite|Kway::fmAppend);
   Kway::TFileMode idxmode = Kway::TFileMode(Kway::fmOpenAlways|Kway::fmReadWrite|Kway::fmDenyWrite);

   //OpenOrderInfo();
   _Loger.Open(std::string(_LogPath + "FIX.log").c_str(),    logmode);
   _RecvIdx.Open(std::string(_LogPath + "RecvIdx.bin").c_str(), idxmode);
   _SendIdx.Open(std::string(_LogPath + "SendIdx.bin").c_str(), idxmode);

   if(!_Loger.IsOpened() || !_RecvIdx.IsOpened() || !_SendIdx.IsOpened()) {
      struct TWriteFileErrLog
      {
        static K_mf(void) Write (TStkFixRecorder* aRecorder, TStkSesFix* aSes, const char* aFileDesp, Kway::TFile& aFile) {
           std::string  ErrMsg = aFileDesp;
           ErrMsg += ":" + aFile.GetLastErr();
           aRecorder->WriteErrLog(aSes, "Internal Error : ", ErrMsg.c_str());
        }
      };
      if(!_Loger.IsOpened())   TWriteFileErrLog::Write(this, aSes, "Loger",   _Loger);
      if(!_RecvIdx.IsOpened()) TWriteFileErrLog::Write(this, aSes, "RecvIdx", _RecvIdx);
      if(!_SendIdx.IsOpened()) TWriteFileErrLog::Write(this, aSes, "SendIdx", _SendIdx);
      return false;
   }

   int  fRecN = static_cast<int>(_RecvIdx.GetFileSize())/sizeof(SIdxRec);
   if(fRecN <= 1) {
      SetLastRecvSeqNum(aSes, 0);
      SetLastContSeqNum(aSes, 0);
   } else {
      SIdxRec*  fBuf = new SIdxRec[fRecN];
      _RecvIdx.Seek(0,fsBegin);
      fRecN = static_cast<int>(_RecvIdx.Read(fBuf, sizeof(SIdxRec)*fRecN) / sizeof(SIdxRec));
      int  L = fRecN-1;
      SetLastRecvSeqNum(aSes, L);
      SetLastContSeqNum(aSes, L);

      for(;  L > 0;  --L)
         if(fBuf[L]._Pos != Pos_Gap) {
            SetLastRecvSeqNum(aSes, L);
            SetLastContSeqNum(aSes, L);
            switch(fBuf[L]._Pos) {
            case Pos_GapFill: goto __BREAK_L;
            case Pos_GapSkip:
            case Pos_GapErr:  continue;
            default:          break;
            }
            for(int FL = 1;  FL < L;  ++FL)
               if(fBuf[FL]._Pos == Pos_Gap) {
                  SetLastContSeqNum(aSes, FL - 1);
                  break;
               }
            break;
         }
__BREAK_L:
      delete [] fBuf;
   }
   SetLastSendSeqNum(aSes, static_cast<int>(_SendIdx.GetFileSize()/sizeof(SIdxRec)-1));
   if(_LastSendSeqNum < 0)
      SetLastSendSeqNum(aSes, 0);
   return true;
}
//---------------------------------------------------------------------------
/*
K_mf(void)  TStkFixRecorder::OpenOrderInfo()
{
   Kway::TFileMode infomode= Kway::TFileMode(Kway::fmCreatePath|Kway::fmReadWrite|Kway::fmAppend);
   Kway::TFile*  file = new Kway::TFile();
   file->Open(std::string(_LogPath + _IniFileName).c_str(), infomode);

   if(file->GetFileSize() == (Kway::TFile::size_type)0) {
      if(!file->IsOpened())
         file->Open(std::string(_LogPath + _IniFileName).c_str(), infomode);
      file->WriteLN("[Term]");
      file->WriteLN("NORMAL=0");
      file->WriteLN("ODDLOT=0");
      file->WriteLN("[SeqNo]");
      file->WriteLN("NORMAL=0");
      file->WriteLN("ODDLOT=6000");
      file->WriteLN("[TAG11]");
      file->WriteLN("COUNT=0");
   }
   delete file;
}
*/
//---------------------------------------------------------------------------
/*
K_mf(int)  TStkFixRecorder::GetTag11()
{
   TTag11Map::iterator  iter = Map_->fetch(_TradeID);
   TTag11Map::updater   upd  = Map_->begin_update(iter);
   upd->Num_ = upd->Num_ + 1;
   return upd->Num_;
}
*/
//---------------------------------------------------------------------------
/*
K_mf(std::string)  TStkFixRecorder::GetTag11()
{
   TIniFile *_ini = new TIniFile(std::string(_LogPath+_IniFileName).c_str());
   int count = _ini->ReadInteger("TAG11", "COUNT", 0);
   char buf[4] = {0};
   sprintf(buf, "%03d", count);

   Kway::TKDateTimeMS  timeObj = Kway::TKDateTimeMS::Now();
   std::string  timeS = timeObj.GetTime().as_string(false) + timeObj.GetMS().as_string(); //093033123 YYMMDDHHMMSSsss
   timeS += std::string(buf); //HHMMSSsss + xxx
   ++count;
   if(count > 999)
      count = 0;
   _ini->WriteInteger("TAG11", "COUNT", count);
   delete _ini;

   return std::string(timeS);
}
//---------------------------------------------------------------------------
K_mf(std::string)  TStkFixRecorder::GetTag37(ETradingSes tradSes)
{
   std::string  tmp;
   TIniFile *_ini = new TIniFile(std::string(_LogPath+_IniFileName).c_str());

   int term, seqNo;
   char buf[5] = {0};

   if(tradSes == NormalSes) {
      term = _ini->ReadInteger("Term", "NORMAL", 0);
      char buf1[2] = {0};
      sprintf(buf1, "%c", TermKey[term]);
      tmp = buf1;
      seqNo = _ini->ReadInteger("SeqNo", "NORMAL", 0);
      sprintf(buf, "%04d", seqNo);
      tmp += buf;

      ++seqNo;
      if(seqNo > 9999) {
         seqNo = 0;
         ++term;
         if(term > numofele(TermKey)-1)
            term = 0;
      }
      _ini->WriteInteger("Term", "NORMAL", term);
      _ini->WriteInteger("SeqNo", "NORMAL", seqNo);
   } else if(tradSes == OddlotSes) {
      term = _ini->ReadInteger("Term", "ODDLOT", 0);
      tmp = TermKey[term];
      seqNo = _ini->ReadInteger("SeqNo", "ODDLOT", 0);
      sprintf(buf, "%04d", seqNo);
      tmp += buf;

      ++seqNo;
      if(seqNo > 9999) {
         seqNo = 6000;
         ++term;
         if(term > numofele(TermKey)-1)
            term = 0;
      }
      _ini->WriteInteger("Term", "ODDLOT", term);
      _ini->WriteInteger("SeqNo", "ODDLOT", seqNo);
   }

   delete _ini;

   return std::string(tmp);
}
*/
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::WriteSendLog(TStkSesFix* aSes, const SFixStr& aFix, int aMsgSeqNum, bool isResend)
{
   if(isResend)
      WriteLog(aSes, "Resend...");
   if(aSes->GetWriteOrNot())
      WriteLogIdx(aSes, aFix, aMsgSeqNum, _SendIdx);
   else
      WriteLogIdx(aSes, aMsgSeqNum, _SendIdx);
}
//---------------------------------------------------------------------------
K_mf(void)  TStkFixRecorder::WriteRecvLog(TStkSesFix* aSes, const SFixStr& aFix, int aMsgSeqNum)
{
   if(aSes->GetWriteOrNot())
      WriteLogIdx(aSes, aFix, aMsgSeqNum, _RecvIdx);
   else
      WriteLogIdx(aSes, aMsgSeqNum, _RecvIdx);
}
//---------------------------------------------------------------------------

