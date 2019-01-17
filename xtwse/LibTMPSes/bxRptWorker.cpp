#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxRptWorker.h"
#include "bxRpt.h"
#include "TwStkOrder.h"
#include "TwStk.h"
#include "KTime.h"
#include "fixed_num.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
using namespace Kway::Tw::Stk;
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_T5000::OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("00") || cmpstr == std::string("01"))       //R1.R2
   {
        *PacketSize = sizeof(TR1);
        return cpr_Continue;
   }
   else if(cmpstr == std::string("04") || cmpstr == std::string("05"))  //R4.R5
   {
        *PacketSize = CtlHeadSize;
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)          TWkr_T5000::OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("00"))           //收到R1,則通知bxSes
        Ses.OnWorkerFinished(pc_R1,pk);
   else if(cmpstr == std::string("01"))      //收到R2,則通知bxSes,不做任何
        Ses.OnWorkerFinished(pc_R2,pk);     //動作
   else if(cmpstr == std::string("04"))      //收到R4,則通知bxSes,並回覆R5
        Ses.OnWorkerFinished(pc_R4,pk);
   else if(cmpstr == std::string("05"))      //收到R5,則通知bxSes
        Ses.OnWorkerFinished(pc_R5,pk);
}
//---------------------------------------------------------------------------
K_mf(bool)          TWkr_T5000::SendReq    (TbxSes& Ses,TPacketCode code,TSize Num)
{
   Kway::TKTime Time;
   Time = Time.Now();           //取得現在時間

   switch(code)
   {
        case pc_R1:                                     //送出R1
                {
                        TR1 R1(Ses.GetSubSys());
                        char buf[16];
                        R1.MessageTime_.assign(Time.as_string(false));
                        R1.BrokerID_.assign(Ses.GetBrokerID().as_string());
// 20080519 變更成交回報起始序號的做法, 原始做法一律傳送'000000
                        sprintf(buf,"%06d",Num);
//                        ltoa(Num,buf,10);
                        R1.StartSeq_.assign(buf);
                        Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, (&R1),sizeof(TR1));
                        
                        return  Ses.SendPk(R1,sizeof(R1));
                }
        case pc_R5:
                {
                        TR5 R5(Ses.GetSubSys());
                        R5.MessageTime_.assign(Time.as_string(false));
                        Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, (&R5),sizeof(TR5));
                        return  Ses.SendPk(R5,sizeof(R5));
                }
        default:
                return false;
   }
   return false;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_T5010::OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("00"))       //R3
   {
        TR3 R3 = *((TR3*)(&pk));
        *PacketSize = CtlHeadSize + 6 + R3.BodyLength_.as_int();
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)          TWkr_T5010::OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr)
{
/* 20080523 移至ses 內部處理
   Kway::Tw::Stk::TOrdCenter::TRpt::TDeal rpt;
   TR3 R3 = *((TR3*)(&pk));
   int BodyCount = R3.BodyCount_.as_int();
   if(Ses.OrdCenterPtr_ != 0)
      for(int i = 0;i < BodyCount;++i){
         rpt.TseOrdKey_.BrokerID_.assign(R3.Body_[i].BrokerID_.as_string());
         rpt.TseOrdKey_.Market_ = Ses.GetMarket();
         rpt.StkNo_.assign(R3.Body_[i].StkNo_.as_string());
         if(R3.Body_[i].ExchageCode_.as_int() == 0)
                rpt.Qty_ = R3.Body_[i].MthQty_.as_int() * 1000;
         else
                rpt.Qty_ = R3.Body_[i].MthQty_.as_int();
         rpt.Pri_.assign((double)(R3.Body_[i].MthPri_.as_int())/100);
         rpt.TseTime_.assign(R3.Body_[i].MthTime_.as_string());
         rpt.BSCode_.assign(R3.Body_[i].BSCode_.as_string());
         rpt.TseOrdKey_.TseOrdID_.assign(R3.Body_[i].OrderNo_.as_string());
         rpt.IvacNo_.assign(R3.Body_[i].IvacNo_.as_string());
         rpt.BSeqNo_.assign(R3.Body_[i].SeqNo_.as_string());
         rpt.MSeqNo_.assign(R3.Body_[i].RecNo_.as_string());
         static EOType  otypeidx[] = { ot_Gn, ot_CrA, ot_DbA, ot_CrS, ot_DbS,
                                 ot_Tse5, ot_Tse6 };
         rpt.OType_.assignT(otypeidx[R3.Body_[i].OrderType_.as_int()]);
         Ses.OrdCenterPtr_->RptDeal(rpt);
      }
*/
   Ses.OnWorkerFinished(pc_R3,pk);
}
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_T5020::OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("00"))       //R6
   {
        *PacketSize = CtlHeadSize + 6;
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)          TWkr_T5020::OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("00"))           //收到R6,則通知bxSes
        Ses.OnWorkerFinished(pc_R6,pk);
}
//---------------------------------------------------------------------------
