/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#include <vcl.h>
#endif
//---------------------------------------------------------------------------
#include "Nsa_FileWriter.h"
#include <stdio.h>
#include <stdarg.h>  // for va_start, va_end, va_list
#include "TimerThread.h"
#include "Nsa_SysInfo.h"
#include "Nsa_SimpThread.h"
#include "Nsa_UtilComm.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TFileWriterUnitBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFileWriterUnitBase::TFileWriterUnitBase (const std::string& fileName)
   : File_      (fileName.c_str(), (TFileMode)(fmCreatePath|fmAppend))
   , WriteCount_(200)
   , MemQueue_  (0) // 資料不合併
{
}
//---------------------------------------------------------------------------

K_mf(size_t) TFileWriterUnitBase::Polling ()
{
   size_t      doCnt = 0;
   size_t      size;
   const void* mem;

   while (MemQueue_.IsEmpty() == false && doCnt < WriteCount_) {
      size = MemQueue_.GetMemBlock(mem);
      if (size == 0)
         break;

      File_.Write(mem, (Kway::dword)size);
      MemQueue_.SetMemUsed(size);

      ++doCnt;
   }

   // 代表資料還沒寫完
   if (doCnt >= WriteCount_) {
      ++WriteCount_;
      if (WriteCount_ >= 1000)
         WriteCount_ = 1000;
   }
   else {
      WriteCount_ = 200;
   }

   return doCnt;
}


/////////////////////////////////////////////////////////////////////////////
// class TFileWriterHnd impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFileWriterHnd::Reg (const char* filePath)
{
   //==============================================
   // 先使用 TFile 來取得完整檔名
   TFile       file(filePath, (TFileMode)(fmRead));
   std::string fileName = file.GetFullName();
   file.Close();

   //==============================================
   // 檢查是否已經註冊了
   TFileWriterUnitBase* unitBase = Unit_.get();
   
   if (unitBase) {
      if (unitBase->GetFileName() == fileName)
         return;
      
      Unreg();
   }

   //==============================================
   // 註冊 hnd
   TFileWriterGoh fileWriterGoh(true);

   if (!fileWriterGoh.IsValid())
      fileWriterGoh.Initialize();

   fileWriterGoh->RegHnd(fileName, *this);

   //==============================================
   // 設定參數
   if (WriteCount_ != 0) {
      SetWriteCount(WriteCount_);
   }

   //TUtilComm::BugOut("<TFileWriterHnd::Reg> this[%p]", this);
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriterHnd::RegDaily (const char* fileName, const char* subDir)
{
   char buf[256];

   if (subDir)  sprintf(buf, "./Logs/%s/%s/%s", TSysInfo::GetTradeDateStr().c_str(), subDir, fileName);
   else         sprintf(buf, "./Logs/%s/%s",    TSysInfo::GetTradeDateStr().c_str(),         fileName);

   Reg(buf);
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriterHnd::Unreg ()
{
   Unit_.reset();
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriterHnd::SetWriteCount (size_t size)
{
   TFileWriterUnitBase* unit = Unit_.get();
   
   WriteCount_ = size;
   if (unit) {
      unit->SetWriteCount(WriteCount_);
   }
}
//---------------------------------------------------------------------------

void TFileWriterHnd::WriteStr (ELogType logType, const char* fmt, ...)
{
   TFileWriterUnitBase* unit = Unit_.get();
   if (!unit)
      return;

   va_list argptr;
   char    buf[5120];
   int     len;

   va_start(argptr, fmt);
   len = vsprintf(buf, fmt, argptr);
   va_end(argptr);

   if (len == EOF)
      return;
   else if (len >= 5119)
      len = 5119;

   buf[len] = '\0';

   TLogMsg logMsg(logType, buf, len);

   Write(unit, logMsg.GetCStr(), logMsg.GetUsedSize());
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriterHnd::WriteMem (ELogType logType, const void* data, size_t size, const TKDateTimeMS* dtms)
{
   TFileWriterUnitBase* unit = Unit_.get();
   if (!unit)
      return;

   TLogMsg logMsg(logType, data, size, dtms);

   Write(unit, logMsg.GetCStr(), logMsg.GetUsedSize());
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriterHnd::WriteMem (const void* data, size_t size)
{
   TFileWriterUnitBase* unit = Unit_.get();
   if (!unit)
      return;

   Write(unit, data, size);
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriterHnd::WriteMsg (const TLogMsg& logMsg)
{
   TFileWriterUnitBase* unit = Unit_.get();
   if (!unit)
      return;

   Write(unit, logMsg.GetCStr(), logMsg.GetUsedSize());
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriterHnd::Write (TFileWriterUnitBase* unit, const void* data, size_t size)
{
   //TUtilComm::BugOut("<TFileWriterHnd::Write> this[%p], size[%d]", this, size);

   if (UseQueue_)
      unit->PutQueue(data, size);
   else
      unit->PutFile(data, size);
}


/////////////////////////////////////////////////////////////////////////////
// class TFileWriter::TImpl
/////////////////////////////////////////////////////////////////////////////
class TFileWriter::TImpl : public TSimpThread
{
   typedef TSimpThread        inherited;
   typedef TImpl              this_type;
   typedef TTimer<this_type>  TTimerType;

   TFileWriter*  FileWriter_;
   TRsUnit*      RsUnit_;
   ost::Mutex*   Mutex_;
   TTimerType    Timer_;

public:
   K_ctor TImpl (TFileWriter*);

   K_mf(void) StartWork ();
   K_mf(void) StopWork  ();

protected:
   //==============================================
   // overwrite TSimpThread virtual function
   K_mf(int)  Execute ();
   K_mf(void) AfRun   ();

   //==============================================
   // TImpl function
   K_mf(size_t) ExecImpl ();
   K_mf(void)   OnTimer  (TTimerBase*);
};
/////////////////////////////////////////////////////////////////////////////
// class TFileWriter::TImpl impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFileWriter::TImpl::TImpl (TFileWriter* fw)
   : inherited  ()
   , FileWriter_(fw)
   , RsUnit_    (&(fw->RsUnit_))
   , Mutex_     (&(fw->Mutex_))
   , Timer_     (this)
{
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriter::TImpl::StartWork ()
{
   if (inherited::Start()) {
      // 一分鐘檢查一次
      Timer_.Start(1000*60, &this_type::OnTimer);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriter::TImpl::StopWork ()
{
   Timer_.Stop();
   inherited::Stop();
}
//---------------------------------------------------------------------------

K_mf(int) TFileWriter::TImpl::Execute ()
{
   ExecImpl();
   return 10;
}
//---------------------------------------------------------------------------

K_mf(size_t) TFileWriter::TImpl::ExecImpl ()
{
   if (RsUnit_->empty()) {
      //TUtilComm::BugOut("<TFileWriter::TImpl::ExecImpl> leave 1");
      return 0;
   }

   ost::MutexLock locker(*Mutex_);

   //size_t         size = RsUnit_->size();
   //TUtilComm::BugOut("<TFileWriter::TImpl::ExecImpl> enter Mutex, size[%d]", size);
   //if (size > 10) {
   //   char buf[128];
   //   sprintf(buf, "<TFileWriter::TImpl::ExecImpl> RsUnit size[%d] 錯誤", size);
   //   throw Exception(AnsiString(buf));
   //}

   // 必須在判斷一次喔
   if (RsUnit_->empty()) {
      //TUtilComm::BugOut("<TFileWriter::TImpl::ExecImpl> leave 2");
      return 0;
   }

   TFileWriterUnitBase* unit;
   size_t               tolSize = 0;
   TRsUnitI             iter    = RsUnit_->begin();

   //==============================================
   // 開始寫檔
   try {

   for (; iter!=RsUnit_->end(); ++iter) {
      unit = iter->second.get();
      if (!unit)
         continue;

      //Mutex_.leaveMutex();

      tolSize += unit->Polling();

      //Mutex_.enterMutex();
   }

   }
   catch (...) {
      //TUtilComm::BugOut("<TFileWriter::TImpl::ExecImpl> leave 4");
      //Mutex_.leaveMutex();
   }

   //TUtilComm::BugOut("<TFileWriter::TImpl::ExecImpl> leave 3, tolSize[%d]", tolSize);

   return tolSize;
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriter::TImpl::AfRun ()
{
   //==============================================
   // 準備離開了,不過先確認是否資料都已經寫完了
   while (true) {
      if (ExecImpl() == 0)
         break;
   }
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriter::TImpl::OnTimer (TTimerBase*)
{
   //==============================================
   // 開始檢查那些是可以移除的
   ost::MutexLock       locker(*Mutex_);
   TRsUnitI             iter, iterTmp;
   TFileWriterUnitBase* unit;
   unsigned             refCount;

   //TUtilComm::BugOut("TFileWriter::TImpl::OnTimer> enter");

   iter = RsUnit_->begin();
   while (iter != RsUnit_->end()) {
      refCount = iter->second.get_ref_count();
      if (refCount <= 1) {
         // 代表都沒有 Hnd 引用了
         std::string name = iter->first;
         unit = iter->second.get();
         if (unit && unit->IsFinish()) {
            iterTmp = iter;
            ++iter;

            //TUtilComm::BugOut("DelHnd: %s", name.c_str());

            RsUnit_->erase(iterTmp);
            continue;
         }
      }

      ++iter;
   }

   //TUtilComm::BugOut("TFileWriter::TImpl::OnTimer> leave");
}


/////////////////////////////////////////////////////////////////////////////
// class TFileWriter impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFileWriter::TFileWriter ()
   : Impl_(NULL)
{
   Impl_ = new TImpl(this);
}
//---------------------------------------------------------------------------

K_dtor TFileWriter::~TFileWriter ()
{
   Impl_->StopWork();
   delete Impl_;
}
//---------------------------------------------------------------------------

K_mf(void) TFileWriter::RegHnd (const std::string& fileName, TFileWriterHnd& hnd)
{
   ost::MutexLock locker(Mutex_);

   //TUtilComm::BugOut("<TFileWriter::RegHnd> enter, fileName[%s]", fileName.c_str());

   TRsUnitI iter = RsUnit_.find(fileName);

   if (iter == RsUnit_.end()) {
      TRsUnitIns ins = RsUnit_.insert(std::make_pair(fileName, TFileWriterUnit()));
      iter = ins.first;
   }

   if (iter->second.get() == NULL) {
      TFileWriterUnitBase* unit = new TFileWriterUnitBase(fileName);

      iter->second.reset(unit);
   }

   hnd.Unit_ = iter->second;

   if (hnd.UseQueue_) {
      if (Impl_->IsStarted() == false) {
         Impl_->StartWork();
      }
   }

   //TUtilComm::BugOut("<TFileWriter::RegHnd> leave");
}


/////////////////////////////////////////////////////////////////////////////
// class TLogFileComm impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TLogFileComm::TLogFileComm (const void* owner)
   : Owner_(owner)
{
   Kway::uint64 timeNow;

   //**********************************************
   #if defined(K_WINDOWS)
   LARGE_INTEGER li;

   if (QueryPerformanceCounter(&li))
      timeNow = li.QuadPart;

   #else //----------------------------------------

//20130103 eric
//   timeNow = gethrtime();
   #endif
   //**********************************************

   RandNum_ = (int)(timeNow%10000);
}
//---------------------------------------------------------------------------

K_mf(void) TLogFileComm::WriteEvent (ELogType lt, const void* data, size_t lenData)
{
   TLogMsg logMsg;
   char    buf[64];
   int     lenBuf = sprintf(buf, "[%04d_%p] ", RandNum_, Owner_);

   logMsg.PutMem(lt, buf, lenBuf, NULL, false);
   logMsg.Append(data, lenData, true);

   inherited::WriteMsg(logMsg);
}
//---------------------------------------------------------------------------
