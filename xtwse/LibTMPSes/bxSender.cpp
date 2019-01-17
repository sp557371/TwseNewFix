#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxSender.h"
#include "bxSesMgr.h"
#include "SchedulerBase.cpp"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
//---------------------------------------------------------------------------
TbxSesMgr* Sender_; 
//---------------------------------------------------------------------------
K_ctor TbxSender::TbxSender()
   : Timer_(this)
{
   TSchedulerBase* sb = GetScheduler(); 
   if(sb){
      Sender_ = new TbxSesMgr(sb, sb->GetOrdCenter());
      sb->SetSender((TSenderBase*)Sender_);
   }else{
      Timer_.Start(2*1000, &this_type::OnTimer);
   }
}
//---------------------------------------------------------------------------
K_dtor TbxSender::~TbxSender()
{
   Timer_.Stop();
   TSchedulerBase* sb = GetScheduler(); 
   if(sb){
      sb->SetSender((TSenderBase*)NULL);
      sb->SetLineState(Stk::m_Tse, tk_Normal, 0);
      sb->SetLineState(Stk::m_Otc, tk_Normal, 0);
   }else{
      if(Sender_)
         Sender_->SetScheduler((TSchedulerBase*)0);
   }
   delete Sender_;
}
//---------------------------------------------------------------------------
K_mf(void) TbxSender::OnTimer(TTimerBase*)
{
   TSchedulerBase* sb = GetScheduler(); 
   if(sb){
      Timer_.Stop();
      Sender_ = new TbxSesMgr(sb, sb->GetOrdCenter());
      sb->SetSender((TSenderBase*)Sender_);
   }
}
//---------------------------------------------------------------------------
TbxSender* sd;
//---------------------------------------------------------------------------

K_exportC(void*)  nsModuleStart  ()
{
   sd = new TbxSender();
}
//---------------------------------------------------------------------------
K_exportC(const char*)  nsModuleDesp  ()
{
   return "交易所連線模組. V1.0.6.4";
}
//---------------------------------------------------------------------------
K_exportC(bool)  nsModuleFinish  ()
{
   delete sd;
   return true;
}
/////////////////////////////////////////////////////////////////////////////

