/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ApaSesSmpFixUtil.h"
#include "Nsa_Time.h"
#include "Nsa_TextSimpReco.h"
#include "Nsa_UtilComm.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
   
/////////////////////////////////////////////////////////////////////////////
// class TApaFixSeqNoFile impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TApaFixSeqNoFile::TApaFixSeqNoFile (const char* filePath, bool isAutoFlush)
   : IsOverwrite_(false)
   , IsAutoFlush_(isAutoFlush)
{
   if (IsOverwrite_)  File_.Open(filePath, (TFileMode)(fmWrite |fmCreatePath));
   else               File_.Open(filePath, (TFileMode)(fmAppend|fmCreatePath));
}
//---------------------------------------------------------------------------

K_mf(void) TApaFixSeqNoFile::Append (const TSmpInt& seqNo, const FIX::TFixMsgType2& msgType, EKind kind)
{
   static const char* caKind[] = {
      "Rt",
      "Rcv",
      "RcvTimer",
      "RcvEnd",
   };

   // 2009/09/21 14:06:09.260 0000001 A Rt
   // 2009/09/21 14:06:09.260 0000002 A Rcv
   char logMsg[512];
   int  len = sprintf(logMsg, "%s %07d %s %-10s\r\n",
                                 TTimeStamp::Now().as_string().c_str(),
                                 seqNo.get_orig(),
                                 msgType.as_string().c_str(),
                                 caKind[kind]);

   if (IsOverwrite_)
      File_.Seek(0, fsBegin);
   
   File_.Write(logMsg, len);
   
   if (IsAutoFlush_) {
      File_.Flush();
   }
}
//---------------------------------------------------------------------------

K_mf(TSmpInt) TApaFixSeqNoFile::GetLastSeqNo ()
{
   TTextSimpReco_Reader reader(File_.GetFullName().c_str());
   std::string          strRec = reader.GetLastRec(100);

   if (strRec.empty())
      return TSmpInt();

   std::string::size_type pos = strRec.find(' ');
   if (pos == std::string::npos)
      return TSmpInt();

   TSmpInt seqNo;
   seqNo.assign(strRec.c_str() + pos + 1);

   return seqNo;
}


/////////////////////////////////////////////////////////////////////////////
// class TApaFixRecover impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TApaFixRecover::TApaFixRecover (TApaSesSmpFix* sesFix, const TSmpString& apaConnSesID, const TSmpString& fixTargetCompID)
   : SesFix_         (sesFix)
   , LogFileHnd_     (SesFix_->GetLogFileHnd())
   , ApaConnSesID_   (apaConnSesID)
   , FixTargetCompID_(fixTargetCompID)
   , Timer_          (this)
{
   //==============================================
   // 建立 SeqNoFile
   char filePath[256];

   sprintf(filePath, "%s/FixSeqNo_%s.txt", SesFix_->GetLogPath().c_str(), FixTargetCompID_.c_str());
   SeqNoFile_ = new TApaFixSeqNoFile(filePath, sesFix->IsAutoFlush());

   LastSeqNo_ = SeqNoFile_->GetLastSeqNo();
}
//---------------------------------------------------------------------------

K_dtor TApaFixRecover::~TApaFixRecover ()
{
   Timer_.Stop();
   delete SeqNoFile_;
}
//---------------------------------------------------------------------------

K_mf(void) TApaFixRecover::LineUp (const TSmpInt& seqNo, const FIX::TFixMsgType2& msgType, TMessageRef msgRef, bool isRt,
                                   const char* caller)
{
   SRsSeqNo    dat;
   TRsSeqNoIns ins;
   // debug {
   char        logMsg[512];
   int         len;
   // } debug

   dat.MsgType_ = msgType;
   dat.MsgRef_  = msgRef;
   dat.IsRt_    = isRt;

   ins = RsSeqNo_.insert(TRsSeqNo::value_type(seqNo, dat));

   if (!ins.second) {
       len = sprintf(logMsg, "<TApaFixRecover::LineUp> Caller[%s], SesID[%s], FixSeqNo[%d][%d] 排隊失敗!",
                                 caller,
                                 ApaConnSesID_.c_str(),
                                 seqNo        .get_orig(),
                                 LastSeqNo_   .get_orig());

       LogFileHnd_.WriteMem(elt_Error, logMsg, len);
   }
   else {
       len = sprintf(logMsg, "<TApaFixRecover::LineUp> Caller[%s], SesID[%s], FixSeqNo[%d][%d] 進行排隊!",
                                 caller,
                                 ApaConnSesID_.c_str(),
                                 seqNo        .get_orig(),
                                 LastSeqNo_   .get_orig());

       LogFileHnd_.WriteMem(elt_Info, logMsg, len);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaFixRecover::Init ()
{
   char msgBuf[256];
   int  msgLen;

   msgLen = sprintf(msgBuf, "<TApaFixRecover::Init> LastSeqNo[%d] 設成 0", LastSeqNo_.get_orig());
   LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);

   LastSeqNo_.assign0();
}
//---------------------------------------------------------------------------

K_mf(void) TApaFixRecover::CheckFirst (const TSmpInt& newSeqNo)
{
   char    logMsg[256];
   int     logLen;
   TSmpInt snoAp;
   TSmpInt snoMw;
   bool    isRestore = false;
   
   if (newSeqNo > 0) {
      TSmpInt endSeqNo(newSeqNo.get_orig() - 1);
      
      snoAp = LastSeqNo_;
      snoMw = endSeqNo;

      if (endSeqNo == LastSeqNo_) {
         logLen = sprintf(logMsg, "<TApaFixRecover::CheckFirst> Fix 最後序號一致, SesID[%s], MySeqNo[%d]",
                                       ApaConnSesID_.c_str(), endSeqNo.get_orig());

         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      }
      else if (endSeqNo > LastSeqNo_) {
         TSmpInt begSeqNo(LastSeqNo_.get_orig() + 1);
      
         logLen = sprintf(logMsg, "<TApaFixRecover::CheckFirst> MW 端 Fix 序號比較大, 發動回補序號 SesID[%s] Beg[%d] End[%d]",
                                       ApaConnSesID_.c_str(),
                                       begSeqNo     .get_orig(),
                                       endSeqNo     .get_orig());
         
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         
         SesFix_->VirFireBfFirstRestore(ApaConnSesID_);
         SesFix_->SmpRestoreMessage    (ApaConnSesID_, begSeqNo, TSmpInt(-1));
         isRestore = true;
      }
      else {
         logLen = sprintf(logMsg, "<TApaFixRecover::CheckFirst> AP 端 Fix 序號比較大(自行修正), SesID[%s], My[%d], Mw[%d]",
                                       ApaConnSesID_.c_str(), LastSeqNo_.get_orig(), endSeqNo.get_orig());
         
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         LastSeqNo_ = endSeqNo;
      }

   }
   else {
      TSmpInt begSeqNo(LastSeqNo_.get_orig() + 1);

      snoAp = LastSeqNo_;
      snoMw = newSeqNo;

      logLen = sprintf(logMsg, "<TApaFixRecover::CheckFirst> MW 端無序號資料, 發動回補序號 SesID[%s] Beg[%d] End[-1]",
                                    ApaConnSesID_.c_str(), begSeqNo.get_orig());
                                    
      LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

      SesFix_->VirFireBfFirstRestore(ApaConnSesID_);
      SesFix_->SmpRestoreMessage    (ApaConnSesID_, begSeqNo, TSmpInt(-1));
      isRestore = true;
   }

   SesFix_->VirFireFixInMsgSeqNumStatus(snoAp, snoMw, true, isRestore);
}
//---------------------------------------------------------------------------

K_mf(bool) TApaFixRecover::CheckRt (const TSmpInt& seqNo, const FIX::TFixMsgType2& msgType, TMessageRef msgRef)
{
   char               logMsg[256];
   int                logLen;
   TSmpInt::orig_type oSeqNo = seqNo.get_orig();
   
   if (oSeqNo <= 0) {
      logLen = sprintf(logMsg, "<TApaFixRecover::CheckRt> 序號[%d]怎麼會 <= 0 呢???", oSeqNo);
      LogFileHnd_.WriteMem(elt_Warning, logMsg, logLen);
      
      return false;
   }
      
   TSmpInt begSeqNo(LastSeqNo_.get_orig() + 1);

   if (LastSeqNo_ < seqNo) {
      if (seqNo == begSeqNo) {
         SeqNoFile_->Append(seqNo, msgType, TApaFixSeqNoFile::ekd_Rt);
         LastSeqNo_ = begSeqNo;
         return true;
      }
   }
   else {
      // MW 的序號怎麼會比我小呢? 看起來序號是有被重置了
      SesFix_->VirFireFixInMsgSeqNumStatus(LastSeqNo_, seqNo, false, true);
      
      logLen = sprintf(logMsg, "<TApaFixRecover::CheckRt> MW 序號過小[%d], 重置本身序號 bf[%d]",
                                    oSeqNo, LastSeqNo_.get_orig());
      LogFileHnd_.WriteMem(elt_Warning, logMsg, logLen);

      LastSeqNo_.assign(oSeqNo);
      if (oSeqNo == 1) {
         // 代表是第一筆, 也就沒有回補問題了
         return true;
      }

      begSeqNo.assign(1);
   }

   //==============================================
   // 如果是 logon 訊息時, 外部會先處理
   if (msgType == MsgType_Logon)  LineUp(seqNo, msgType, msgRef, false, "CheckRt");
   else                           LineUp(seqNo, msgType, msgRef, true,  "CheckRt");
   
   //==============================================
   // 這邊發現 SeqNo 有斷號了, 所以發動回補吧
   TSmpInt endSeqNo(seqNo.get_orig() - 1);
   
   logLen = sprintf(logMsg, "<TApaFixRecover::CheckRt> 發動回補序號 [%s] [%d] [%d]",
                                 ApaConnSesID_.c_str(),
                                 begSeqNo     .get_orig(),
                                 endSeqNo     .get_orig());
   LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

   SesFix_->SmpRestoreMessage(ApaConnSesID_, begSeqNo, endSeqNo);

   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TApaFixRecover::CheckRcv (const TSmpInt& seqNo, const FIX::TFixMsgType2& msgType, TMessageRef msgRef)
{
   TSmpInt begSeqNo(LastSeqNo_.get_orig() + 1);

   //==============================================
   // 檢查這筆回補資料是否可以讓序號連貫
   if (seqNo == begSeqNo) {
      char logMsg[128];
      int  len;
      
      SeqNoFile_->Append(seqNo, msgType, TApaFixSeqNoFile::ekd_Rcv);
      LastSeqNo_ = begSeqNo;

      // log {
      len = sprintf(logMsg, "<TApaFixRecover::CheckRcv> 處理序號[%d]", seqNo.get_orig());
      LogFileHnd_.WriteMem(elt_Info, logMsg, len);
      // } log

      //===========================================
      // 這一筆讓資料連貫了,有可能下一筆也是,所以啟動Timer檢查
      Timer_.Start(0, &this_type::OnTimer);

      return true;
   }

   LineUp(seqNo, msgType, msgRef, false, "CheckRcv");

   return false;
}
//---------------------------------------------------------------------------

K_mf(void) TApaFixRecover::CheckRcvEnd (const TSmpInt& endSeqNo)
{
   //==============================================
   // 回補結束囉
   char msgBuf[512];
   int  msgLen;

   if (endSeqNo < 0) {
      msgLen = sprintf(msgBuf, "<CheckRcvEnd> NewSeqNo[%d] < 0", endSeqNo.get_orig());
      LogFileHnd_.WriteMem(elt_Error, msgBuf, msgLen);
      return;
   }
   else if (endSeqNo == 0) {
      msgLen = sprintf(msgBuf, "<CheckRcvEnd> NewSeqNo == 0, 代表 LastSeqNo[%d] 是正確的",
                                    LastSeqNo_.get_orig());

      LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
      return;
   }
   
   if (RsSeqNo_.empty()) {
      msgLen = sprintf(msgBuf, "<CheckRcvEnd> 回補 Queue 是空的, 設定 NewSeqNo[%d], LastSeqNo[%d]",
                                    endSeqNo.get_orig(), LastSeqNo_.get_orig());
      
      LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
      LastSeqNo_ = endSeqNo;
      return;
   }

   //==============================================
   // 處理回補的 Queue
   TRsSeqNoI iterTmp;
   TRsSeqNoI iter = RsSeqNo_.begin();

   //==============================================
   // Ex:
   //    LastSeqNo_       = 49
   //    endSeqNo         = 50
   //    RsSeqNo_.begin() = 51
   if (iter->first > endSeqNo) {
      // 將 endSeqNo 設定為最新的 SeqNo
      // 其餘的部分讓 timer 來處理囉
      
      // log {
      msgLen = sprintf(msgBuf, "<CheckRcvEnd> 設定最後序號, LastSeqNo[%d], EndSeqNo[%d], RsBeg[%d]",
                                    LastSeqNo_.get_orig(),
                                    endSeqNo.get_orig(),
                                    iter->first.get_orig());

      LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
      // } log

      SeqNoFile_->Append(endSeqNo, TFixMsgType2(), TApaFixSeqNoFile::ekd_RcvEnd);

      LastSeqNo_ = endSeqNo;
      
      Timer_.Start(0, &this_type::OnTimer);
   }
   else {
      //===========================================
      // 先把 Timer 停掉
      Timer_.Stop();

      //===========================================
      // 這邊只處理比 EndSeqNo 小的資料, 有可能會有比 EndSeqNo 大的資料, 應該就是即時的資料囉
      while (iter->first <= endSeqNo) {
         const TSmpInt& seqNo = iter->first;
         SRsSeqNo&      dat   = iter->second;
      
         SeqNoFile_->Append(seqNo, dat.MsgType_, TApaFixSeqNoFile::ekd_RcvEnd);
      
         // log {
         msgLen = sprintf(msgBuf, "<CheckRcvEnd> 處理序號, LastSeqNo[%d], EndSeqNo[%d], RsCurr[%d]",
                                       LastSeqNo_.get_orig(),
                                       endSeqNo  .get_orig(),
                                       seqNo     .get_orig());

         LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
         // } log

         SDispFixMsgInfo dfmInfo(ApaConnSesID_, dat.MsgType_, dat.MsgRef_, dat.IsRt_, TSmpMsgRef());
         SesFix_->DispFixMessage(dfmInfo);
      
         LastSeqNo_ = seqNo;

         //===========================================
         // 往下移動一筆
         iterTmp = iter;
         ++iter;

         RsSeqNo_.erase(iterTmp);
         if (iter == RsSeqNo_.end())
            break;
      }

      //===========================================
      // 如果這邊還有發現回補 Queue 還有資料, 那就啟動 Timer 處理囉
      if (!RsSeqNo_.empty()) {
         msgLen = sprintf(msgBuf, "<CheckRcvEnd> 回補 Queue 尚未處理完, 啟動 Timer, LastSeqNo[%d], EndSeqNo[%d], RsBeg[%d]",
                                       LastSeqNo_.get_orig(),
                                       endSeqNo  .get_orig(),
                                       RsSeqNo_.begin()->first.get_orig());

         LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);

         Timer_.Start(0, &this_type::OnTimer);
      }
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaFixRecover::OnTimer (TTimerBase*)
{
   if (RsSeqNo_.empty()) {
      Timer_.Stop();
      return;
   }

   char      logMsg[128];
   int       logLen;
   TSmpInt   begSeqNo(LastSeqNo_.get_orig() + 1);
   TRsSeqNoI iter    = RsSeqNo_.begin();
   size_t    execCnt = 0;

   // log {
   logLen = sprintf(logMsg, "<TApaFixRecover::OnTimer> RsBeg[%d], CurrSeqNo[%d]",
                                 iter->first.get_orig(), begSeqNo.get_orig());

   LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
   // } log

   while (iter->first == begSeqNo) {
      SRsSeqNo& dat = iter->second;
      
      SeqNoFile_->Append(begSeqNo, dat.MsgType_, TApaFixSeqNoFile::ekd_RcvTimer);
      
      // log {
      logLen = sprintf(logMsg, "<TApaFixRecover::OnTimer> 處理序號[%d]", begSeqNo.get_orig());
      LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      // } log

      SDispFixMsgInfo dfmInfo(ApaConnSesID_, dat.MsgType_, dat.MsgRef_, dat.IsRt_, TSmpMsgRef());
      SesFix_->DispFixMessage(dfmInfo);
      if (dat.MsgType_.is_app())
         ++execCnt;
      
      LastSeqNo_ = begSeqNo;
      
      TRsSeqNoI iterTmp = iter;

      //===========================================
      // 往下移動一筆
      ++iter;
      RsSeqNo_.erase(iterTmp);
      if (iter == RsSeqNo_.end())
         break;

      //===========================================
      // 一次只要處理 10 筆 app msg 就好了
      if (execCnt >= 10)
         break;
   
      //===========================================
      // 準備下一筆資料
      ++begSeqNo;
   }
}
//---------------------------------------------------------------------------
