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
    // ���~�B�z...data error
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
        //�h���F n Bytes, �Y w=5, rd=7, ���� w=-2
        memcpy(bufp, bufp + rdsz + WaitSkipBytes_, -(WaitSkipBytes_));
        WaitSkipBytes_ = 0;
      }
    }
    //���p�G Head �٨S����
    if(RPos_ < HeadSize_)
    {
      rdsz  = Mes_->Recv(Buf_ + RPos_, HeadSize_ - RPos_);
      if(rdsz <= 0)
        break; //�S��ƥi��,���}do..while
      RPos_ += rdsz;
    }
    //���P�_HeadCode
    if(HeadCode_ >= 0)
      if(Buf_[0] != (char)HeadCode_)
      {
        //��1�� byte ���O HeadCode, �h���U�@�� HeadCode
        RPos_ = GiveupBuf(Buf_, 0, RPos_);
        continue;
      }
    //���p�G Head �w����
    if(RPos_ >= HeadSize_)
    {
      if(CurrPacketSize_ == 0)
        if(!ChangeCurrSize(vGetPacketSize(Buf_)))
          continue;
      rdsz = CurrPacketSize_ - RPos_;
      if(rdsz > 0)
      {
        rdsz = Mes_->Recv(Buf_ + RPos_, rdsz);
        if(rdsz <= 0) //�S��ƥi��,���}do..while
          break;
        RPos_ += rdsz;
      }
      if(RPos_ < CurrPacketSize_)
        break; // ���쪺��Ƥ����@�� packet,��ܤw�L��ƥi��,���}do..while
      int wsz = vBeforeAPacket(Buf_);
      if(wsz > CurrPacketSize_) //packet �٨S������,
      {
        ChangeCurrSize(wsz);     //�]�w�s�� current packet size,�M���~�򱵦����
        continue;
      }
      int pkSz = CurrPacketSize_;
      rdsz = RPos_;
      RPos_ = 0;
      CurrPacketSize_ = 0;
      if(vAPacket(Buf_)) //���]�˳B�z����,���U�ӳB�zAfterAPacket
        vAfterAPacket(Buf_);
      else
        pkSz = 1; //�Y���ʥ]�����~�ʥ],�h Skip 1 byte �����~��B�z
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

