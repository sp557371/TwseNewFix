//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "SesLibBase.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
/////////////////////////////////////////////////////////////////////////////
K_ctor TSesLibBase::TSesLibBase (Kway::TSesMgr* aOwner)
   : Owner_      (aOwner),
     HeadCode_   (-1),
     GiveupBytes_(0),
     Timer_      (this)
{
  AutoRelinkSecs_ = 5;
}
//---------------------------------------------------------------------------
K_mf(void) TSesLibBase::StartTimer(unsigned aSecs, unsigned ams)
{
  Timer_.Start(aSecs*1000 + ams, &this_type::DoTimeOut);
}
//---------------------------------------------------------------------------
K_mf(void) TSesLibBase::StopTimer ()
{
   Timer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void) TSesLibBase::DoTimeOut (Kway::TTimerBase* aSender)
{
  if(AutoRelinkSecs_ > 0)
  {
    StopTimer();
    if(GetMes() && !GetMes()->IsLinkReady())
      Open();
  }
}
//---------------------------------------------------------------------------
K_mf(int) TSesLibBase::GiveupBuf (char* aBuf, int aSkipSz, int aBufSz)
{
  if((aBufSz -= aSkipSz) <= 0)
    return 0;
  if(HeadCode_ <= -1)
  {
    memcpy(aBuf, aBuf+aSkipSz, aBufSz);
    return aBufSz;
  }
  char* mpos = (char*)memchr(aBuf+aSkipSz, HeadCode_, aBufSz);
  if(mpos == NULL)          //沒有找到HeadCode
  {
    GiveupBytes_ += aBufSz; //表示所有的資料皆為 unknow
    return 0;               //aBuf重頭收起
  }
  int unknows = mpos - (aBuf+aSkipSz);   //unknow 的 bytes 數
  GiveupBytes_ += unknows;
  if((aBufSz -= unknows) <= 0)
    return 0;
  memcpy(aBuf, mpos, aBufSz);
  return aBufSz;
}
//---------------------------------------------------------------------------

