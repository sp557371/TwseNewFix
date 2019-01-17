#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxDRWorker.h"
#include "bxDR.h"
#include "TwStk.h"
#include "bxDRSes.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::DR;
/////////////////////////////////////////////////////////////////////////////
K_mf(int)  TWork_RPT::GetPacketSize(TbxSesBase& aSes, const TbxData& aPkt)
{
  switch(aPkt.GetFun())
  {
    case ckRPTLink:
      switch(aPkt.GetMsg())
      {
        case mgR1:  // same as R2
        case mgR2:  return sizeof(TR1R2);
        case mgR4:  // same as R5
        case mgR5:  return sizeof(TR4R5);
      }
      break;
    case ckRPTData: return sizeof(TControlHead) + sizeof(TLength) + sizeof(TCount);
    case ckRPTEnd:  return sizeof(TR6);
  }
  return 0;
}
//---------------------------------------------------------------------------
K_mf(int)  TWork_RPT::BeforeAPacket(TbxSesBase& aSes, const TbxData& aPkt)
{
  if(aPkt.GetFun() == ckRPTData)
  {
    const TControlHead* head = static_cast<const TControlHead*>(&aPkt);
    const TR3           *r3  = static_cast<const TR3*>(head);
    return szTR3_Head + r3->BodyLength_.as_int();
  }
  return 0;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_RPT::APacket(TbxSesBase& aSes, const TbxData& aPkt)
{
  TbxDRSes* ses = dynamic_cast<TbxDRSes*>(&aSes);
  const TControlHead* head = static_cast<const TControlHead*>(&aPkt);
  size_t sz = GetPacketSize(aSes, aPkt)+
              (aSes.GetSesMgr()->IsCompleteLog()?((TR3&)aPkt).BodyLength_.as_int():0);
  ses->WriteLog(reinterpret_cast<const char*>(&aPkt), sz, ses->GetPvcID(), 1, ses->GetBrokerID());
  return OnRptPkt(*ses, *head);
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_RPT::DoReq(TbxSesBase& aSes, const TBxMsg aMsg, const TControlHead& aPkt)
{
  TbxDRSes* ses = dynamic_cast<TbxDRSes*>(&aSes);
  return DoRptReq(*ses, aMsg, aPkt);
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_R5000::OnRptPkt(TbxDRSes& aSes, const TControlHead& aPkt)
{
  switch(aPkt.GetMsg())
  {
    case mgR2:  // R2 起始作業訊息
      aSes.SetState(TbxDRSes::rpt_RptReply, "R2");
      aSes.StartRptTimer();
      break;
    case mgR4:  // R4 連線作業訊息
      if(aSes.DoWorker(aSes.GetWorkKey(ckRPTLink), mgR5, aPkt))    //送出 R5
      {
        aSes.SetState(TbxDRSes::rpt_LinkChk, "R5");
        aSes.StartRptTimer();
      }
      break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_R5000::DoRptReq(TbxDRSes& aSes, const TBxMsg aMsg, const TControlHead& aPkt)
{
  TR1R2 r1;
  TR4R5 r5;
  switch(aMsg)
  {
    case mgR1:
      r1.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
      r1.BrkID_    = aSes.GetBrokerID();
      r1.StartSeq_.assign("0000000");
      return aSes.SendPkt(r1);
    case mgR5:
      r5.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
      return aSes.SendPkt(r5);
  }
  return false;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_R5010::OnRptPkt(TbxDRSes& aSes, const TControlHead& aPkt)
{
  // 如果STATE是rpt_Starting, 表示在重新要求R1, 此時放棄該成交資料
  if(aSes.GetSesState() == TbxDRSes::rpt_Starting)
    return true;
  if(aPkt.GetMsg() == mgR3)
  {
    aSes.SetState(TbxDRSes::rpt_Data, "R3");
    const TR3* r3 = static_cast<const TR3*>(&aPkt);
    int count = r3->BodyCount_.as_int();
    for(int i=0;i<count;i++)
      aSes.WriteDR(r3->Body_[i]);
    return true;
  }
  return false;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_R5020::OnRptPkt(TbxDRSes& aSes, const TControlHead& aPkt)
{
   // 20111208 eric: DR 回補結束後, 要解除註冊
   if(aPkt.GetFun() == ckRPTEnd)
      aSes.DRFinish();
  return true;
}
//---------------------------------------------------------------------------

