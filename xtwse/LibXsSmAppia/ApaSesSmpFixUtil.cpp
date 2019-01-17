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
   // �إ� SeqNoFile
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
       len = sprintf(logMsg, "<TApaFixRecover::LineUp> Caller[%s], SesID[%s], FixSeqNo[%d][%d] �ƶ�����!",
                                 caller,
                                 ApaConnSesID_.c_str(),
                                 seqNo        .get_orig(),
                                 LastSeqNo_   .get_orig());

       LogFileHnd_.WriteMem(elt_Error, logMsg, len);
   }
   else {
       len = sprintf(logMsg, "<TApaFixRecover::LineUp> Caller[%s], SesID[%s], FixSeqNo[%d][%d] �i��ƶ�!",
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

   msgLen = sprintf(msgBuf, "<TApaFixRecover::Init> LastSeqNo[%d] �]�� 0", LastSeqNo_.get_orig());
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
         logLen = sprintf(logMsg, "<TApaFixRecover::CheckFirst> Fix �̫�Ǹ��@�P, SesID[%s], MySeqNo[%d]",
                                       ApaConnSesID_.c_str(), endSeqNo.get_orig());

         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      }
      else if (endSeqNo > LastSeqNo_) {
         TSmpInt begSeqNo(LastSeqNo_.get_orig() + 1);
      
         logLen = sprintf(logMsg, "<TApaFixRecover::CheckFirst> MW �� Fix �Ǹ�����j, �o�ʦ^�ɧǸ� SesID[%s] Beg[%d] End[%d]",
                                       ApaConnSesID_.c_str(),
                                       begSeqNo     .get_orig(),
                                       endSeqNo     .get_orig());
         
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         
         SesFix_->VirFireBfFirstRestore(ApaConnSesID_);
         SesFix_->SmpRestoreMessage    (ApaConnSesID_, begSeqNo, TSmpInt(-1));
         isRestore = true;
      }
      else {
         logLen = sprintf(logMsg, "<TApaFixRecover::CheckFirst> AP �� Fix �Ǹ�����j(�ۦ�ץ�), SesID[%s], My[%d], Mw[%d]",
                                       ApaConnSesID_.c_str(), LastSeqNo_.get_orig(), endSeqNo.get_orig());
         
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         LastSeqNo_ = endSeqNo;
      }

   }
   else {
      TSmpInt begSeqNo(LastSeqNo_.get_orig() + 1);

      snoAp = LastSeqNo_;
      snoMw = newSeqNo;

      logLen = sprintf(logMsg, "<TApaFixRecover::CheckFirst> MW �ݵL�Ǹ����, �o�ʦ^�ɧǸ� SesID[%s] Beg[%d] End[-1]",
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
      logLen = sprintf(logMsg, "<TApaFixRecover::CheckRt> �Ǹ�[%d]���| <= 0 �O???", oSeqNo);
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
      // MW ���Ǹ����|��ڤp�O? �ݰ_�ӧǸ��O���Q���m�F
      SesFix_->VirFireFixInMsgSeqNumStatus(LastSeqNo_, seqNo, false, true);
      
      logLen = sprintf(logMsg, "<TApaFixRecover::CheckRt> MW �Ǹ��L�p[%d], ���m�����Ǹ� bf[%d]",
                                    oSeqNo, LastSeqNo_.get_orig());
      LogFileHnd_.WriteMem(elt_Warning, logMsg, logLen);

      LastSeqNo_.assign(oSeqNo);
      if (oSeqNo == 1) {
         // �N��O�Ĥ@��, �]�N�S���^�ɰ��D�F
         return true;
      }

      begSeqNo.assign(1);
   }

   //==============================================
   // �p�G�O logon �T����, �~���|���B�z
   if (msgType == MsgType_Logon)  LineUp(seqNo, msgType, msgRef, false, "CheckRt");
   else                           LineUp(seqNo, msgType, msgRef, true,  "CheckRt");
   
   //==============================================
   // �o��o�{ SeqNo ���_���F, �ҥH�o�ʦ^�ɧa
   TSmpInt endSeqNo(seqNo.get_orig() - 1);
   
   logLen = sprintf(logMsg, "<TApaFixRecover::CheckRt> �o�ʦ^�ɧǸ� [%s] [%d] [%d]",
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
   // �ˬd�o���^�ɸ�ƬO�_�i�H���Ǹ��s�e
   if (seqNo == begSeqNo) {
      char logMsg[128];
      int  len;
      
      SeqNoFile_->Append(seqNo, msgType, TApaFixSeqNoFile::ekd_Rcv);
      LastSeqNo_ = begSeqNo;

      // log {
      len = sprintf(logMsg, "<TApaFixRecover::CheckRcv> �B�z�Ǹ�[%d]", seqNo.get_orig());
      LogFileHnd_.WriteMem(elt_Info, logMsg, len);
      // } log

      //===========================================
      // �o�@������Ƴs�e�F,���i��U�@���]�O,�ҥH�Ұ�Timer�ˬd
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
   // �^�ɵ����o
   char msgBuf[512];
   int  msgLen;

   if (endSeqNo < 0) {
      msgLen = sprintf(msgBuf, "<CheckRcvEnd> NewSeqNo[%d] < 0", endSeqNo.get_orig());
      LogFileHnd_.WriteMem(elt_Error, msgBuf, msgLen);
      return;
   }
   else if (endSeqNo == 0) {
      msgLen = sprintf(msgBuf, "<CheckRcvEnd> NewSeqNo == 0, �N�� LastSeqNo[%d] �O���T��",
                                    LastSeqNo_.get_orig());

      LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
      return;
   }
   
   if (RsSeqNo_.empty()) {
      msgLen = sprintf(msgBuf, "<CheckRcvEnd> �^�� Queue �O�Ū�, �]�w NewSeqNo[%d], LastSeqNo[%d]",
                                    endSeqNo.get_orig(), LastSeqNo_.get_orig());
      
      LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
      LastSeqNo_ = endSeqNo;
      return;
   }

   //==============================================
   // �B�z�^�ɪ� Queue
   TRsSeqNoI iterTmp;
   TRsSeqNoI iter = RsSeqNo_.begin();

   //==============================================
   // Ex:
   //    LastSeqNo_       = 49
   //    endSeqNo         = 50
   //    RsSeqNo_.begin() = 51
   if (iter->first > endSeqNo) {
      // �N endSeqNo �]�w���̷s�� SeqNo
      // ��l�������� timer �ӳB�z�o
      
      // log {
      msgLen = sprintf(msgBuf, "<CheckRcvEnd> �]�w�̫�Ǹ�, LastSeqNo[%d], EndSeqNo[%d], RsBeg[%d]",
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
      // ���� Timer ����
      Timer_.Stop();

      //===========================================
      // �o��u�B�z�� EndSeqNo �p�����, ���i��|���� EndSeqNo �j�����, ���ӴN�O�Y�ɪ�����o
      while (iter->first <= endSeqNo) {
         const TSmpInt& seqNo = iter->first;
         SRsSeqNo&      dat   = iter->second;
      
         SeqNoFile_->Append(seqNo, dat.MsgType_, TApaFixSeqNoFile::ekd_RcvEnd);
      
         // log {
         msgLen = sprintf(msgBuf, "<CheckRcvEnd> �B�z�Ǹ�, LastSeqNo[%d], EndSeqNo[%d], RsCurr[%d]",
                                       LastSeqNo_.get_orig(),
                                       endSeqNo  .get_orig(),
                                       seqNo     .get_orig());

         LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
         // } log

         SDispFixMsgInfo dfmInfo(ApaConnSesID_, dat.MsgType_, dat.MsgRef_, dat.IsRt_, TSmpMsgRef());
         SesFix_->DispFixMessage(dfmInfo);
      
         LastSeqNo_ = seqNo;

         //===========================================
         // ���U���ʤ@��
         iterTmp = iter;
         ++iter;

         RsSeqNo_.erase(iterTmp);
         if (iter == RsSeqNo_.end())
            break;
      }

      //===========================================
      // �p�G�o���٦��o�{�^�� Queue �٦����, ���N�Ұ� Timer �B�z�o
      if (!RsSeqNo_.empty()) {
         msgLen = sprintf(msgBuf, "<CheckRcvEnd> �^�� Queue �|���B�z��, �Ұ� Timer, LastSeqNo[%d], EndSeqNo[%d], RsBeg[%d]",
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
      logLen = sprintf(logMsg, "<TApaFixRecover::OnTimer> �B�z�Ǹ�[%d]", begSeqNo.get_orig());
      LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      // } log

      SDispFixMsgInfo dfmInfo(ApaConnSesID_, dat.MsgType_, dat.MsgRef_, dat.IsRt_, TSmpMsgRef());
      SesFix_->DispFixMessage(dfmInfo);
      if (dat.MsgType_.is_app())
         ++execCnt;
      
      LastSeqNo_ = begSeqNo;
      
      TRsSeqNoI iterTmp = iter;

      //===========================================
      // ���U���ʤ@��
      ++iter;
      RsSeqNo_.erase(iterTmp);
      if (iter == RsSeqNo_.end())
         break;

      //===========================================
      // �@���u�n�B�z 10 �� app msg �N�n�F
      if (execCnt >= 10)
         break;
   
      //===========================================
      // �ǳƤU�@�����
      ++begSeqNo;
   }
}
//---------------------------------------------------------------------------
