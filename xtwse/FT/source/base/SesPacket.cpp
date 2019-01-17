#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "SesPacket.h"
#include <exception>
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
/////////////////////////////////////////////////////////////////////////////
K_ctor TSesPacket::TSesPacket (Kway::TSesMgr* aOwner, int aMaxPacketSize, int aHeadSize, void* aBuf)
   : inherited      (aOwner),
     HeadSize_      (aHeadSize),
     WaitSkipBytes_ (0),
     MaxPacketSize_ (aMaxPacketSize),
     CurrPacketSize_(0),
     RPos_          (0),
     Buf_((char*)aBuf)
{
}
//---------------------------------------------------------------------------
K_mf(bool) TSesPacket::ChangeCurrSize(int aNewSz)
{
  if(aNewSz < HeadSize_ || aNewSz > MaxPacketSize_)
  {
    // 錯誤處理...data error
    CurrPacketSize_ = 0;
    RPos_ = GiveupBuf(Buf_, 1, RPos_);
    return false;
  }
  CurrPacketSize_ = aNewSz;
  return true;
}
//---------------------------------------------------------------------------
K_mf(void) TSesPacket::CheckRecv ()
{
  if(Buf_ == NULL)
    return;
  TMesBase* Mes_;
  Mes_ = GetMes();
  do{
    if(Mes_ == NULL)
      break;
    int rdsz;
    if(WaitSkipBytes_ > 0)
    {
      char*  bufp = Buf_ + RPos_;
      rdsz = Mes_->Recv(bufp, WaitSkipBytes_);
      if(rdsz <= 0)
        break;
      WaitSkipBytes_ -= rdsz;
      if(WaitSkipBytes_ > 0)
        break;
      if(WaitSkipBytes_ < 0)
      {
        //多收了 n Bytes, 若 w=5, rd=7, 此時 w=-2
        memcpy(bufp, bufp + rdsz + WaitSkipBytes_, -(WaitSkipBytes_));
        WaitSkipBytes_ = 0;
      }
    }
    //↓如果 Head 還沒收滿
    if(RPos_ < HeadSize_)
    {
      rdsz  = Mes_->Recv(Buf_ + RPos_, HeadSize_ - RPos_);
      if(rdsz <= 0)
        break; //沒資料可收,離開do..while
      RPos_ += rdsz;
    }
    //↓判斷HeadCode
    if(HeadCode_ >= 0)
      if(Buf_[0] != (char)HeadCode_)
      {
        //第1個 byte 不是 HeadCode, 則找到下一個 HeadCode
        RPos_ = GiveupBuf(Buf_, 0, RPos_);
        continue;
      }
    //↓如果 Head 已收完
    if(RPos_ >= HeadSize_)
    {
      if(CurrPacketSize_ == 0)
        if(!ChangeCurrSize(vGetPacketSize(Buf_)))
          continue;
      rdsz = CurrPacketSize_ - RPos_;
      if(rdsz > 0)
      {
        rdsz = Mes_->Recv(Buf_ + RPos_, rdsz);
        if(rdsz <= 0) //沒資料可收,離開do..while
          break;
        RPos_ += rdsz;
      }
      if(RPos_ < CurrPacketSize_)
        break; // 收到的資料不滿一個 packet,表示已無資料可收,離開do..while
      int wsz = vBeforeAPacket(Buf_);
      if(wsz > CurrPacketSize_) //packet 還沒有收完,
      {
        ChangeCurrSize(wsz);     //設定新的 current packet size,然後繼續接收資料
        continue;
      }
      int pkSz = CurrPacketSize_;
      rdsz = RPos_;
      RPos_ = 0;
      CurrPacketSize_ = 0;
      if(vAPacket(Buf_)) //此包裝處理完畢,接下來處理AfterAPacket
        vAfterAPacket(Buf_);
      else
        pkSz = 1; //若此封包為錯誤封包,則 Skip 1 byte 之後繼續處理
      RPos_ = GiveupBuf(Buf_, pkSz, rdsz);
    }
  }while(1);
}
//---------------------------------------------------------------------------
K_mf(int) TSesPacket::vBeforeAPacket (void*)
{
  return 0;
}
//---------------------------------------------------------------------------
K_mf(void) TSesPacket::vAfterAPacket (void*)
{
}
/////////////////////////////////////////////////////////////////////////////

