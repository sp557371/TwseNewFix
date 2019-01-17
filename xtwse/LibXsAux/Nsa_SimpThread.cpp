/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_SimpThread.h"
#include "SyncEv.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TSimpThread impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TSimpThread::TSimpThread ()
   : inherited  ()
   , StopThread_(false)
   , Started_   (false)
{
}
//---------------------------------------------------------------------------

TSimpThread::~TSimpThread ()
{
   Stop();
}
//---------------------------------------------------------------------------

void TSimpThread::run (void)
{
   if (!BfRun()) {
      StopThread_ = true;
   }

   int sleepms = 0;

   while (!StopThread_) {
      sleep(sleepms);

      sleepms = Execute();
      if (sleepms < 0)
         StopThread_ = true;
   }

   AfRun();
   
   StopThread_ = false;
   Started_    = false;
}
//---------------------------------------------------------------------------

K_mf(bool) TSimpThread::Start ()
{
   if (Started_)
      return false;

   Started_ = true;
   inherited::start();

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TSimpThread::Stop ()
{
   StopThread_ = true;

   while (StopThread_ && inherited::isRunning())
      TSyncObj::CheckSync();
}
//---------------------------------------------------------------------------

