/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_SeqNoGene.h"
#include <cc++/thread.h>
#include "KFile.h"
#include "Nsa_TextSimpReco.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
//---------------------------------------------------------------------------
static const char TSeqNoGene_NewLine[] = { "\r\n" };

/////////////////////////////////////////////////////////////////////////////
// class TSeqNoGene impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TSeqNoGene::TSeqNoGene (const char* fileName, bool isAppendMode, bool isAutoFlush)
   : CurrSeqNo_   (1)
   , IsAppendMode_(isAppendMode)
   , IsAutoFlush_ (isAutoFlush)
{
   //==============================================
   // 先到 file 裡面找到最後一筆資料
   TTextSimpReco_Reader reader(fileName, TSeqNoGene_NewLine);
   std::string          rec = reader.GetLastRec(20);

   if (rec.empty() == false) {
      CurrSeqNo_ = atoi(rec.c_str());
      ++CurrSeqNo_;
   }
   
   //==============================================
   // 建立物件
   Mutex_ = new ost::Mutex;
   if (IsAppendMode_)  File_ = new TFile(fileName, (TFileMode)(fmAppend|fmCreatePath));
   else                File_ = new TFile(fileName, (TFileMode)(fmWrite |fmCreatePath));
}
//---------------------------------------------------------------------------

K_dtor TSeqNoGene::~TSeqNoGene ()
{
   delete File_;
   delete Mutex_;
}
//---------------------------------------------------------------------------

K_mf(size_t) TSeqNoGene::GetNewInt ()
{
   ost::MutexLock locker(*Mutex_);
   
   char   buf[20];
   int    len;
   size_t seqNo = CurrSeqNo_;

   len = sprintf(buf, "%-15u%s", seqNo, TSeqNoGene_NewLine);
   if (!IsAppendMode_)
      File_->Seek(0, fsBegin);

   File_->Write(buf, len);
   if (IsAutoFlush_)
      File_->Flush();
   
   ++CurrSeqNo_;
   return seqNo;
}
//---------------------------------------------------------------------------

K_mf(std::string) TSeqNoGene::GetNewStr (size_t len, char prefiller)
{
   char   buf[50];
   size_t seqNo = GetNewInt();
   
   sprintf(buf, "%0*u", len, seqNo);

   if (prefiller != '0') {
      if (prefiller==0 || isdigit(prefiller))
         prefiller = ' ';

      char* ptr = buf;
      while (*ptr == '0')
         *ptr++ = prefiller;
   }
   
   return std::string(buf);
}
//---------------------------------------------------------------------------
