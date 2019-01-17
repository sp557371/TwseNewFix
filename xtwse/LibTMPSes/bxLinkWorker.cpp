#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxLinkWorker.h"
#include "bxLink.h"
#include "KTime.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_L1010::OnPkCheck  (TbxSes& Ses,const THead& pk,
                                        TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("00") || cmpstr == std::string("01"))
   {
        *PacketSize = CtlHeadSize;
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_L1010::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("00")){           //收到L010,則通知bxSes並回覆 L020
/*
      char buf[128];
      Kway::TKTime Time_;
      Time_.assign(pk.MessageTime_.as_string());
      sprintf(buf,"./settime %02d%02d.%02d",Time_.GetHour(),Time_.GetMin(),Time_.GetSec());
//      system(buf);  // 950309 eric 避免在thread 裏使用system 改用popen
      pclose(popen(buf,"r"));
//      Time_.SetSystem();
*/
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
      Ses.OnLinkWorkerFinished(pc_L010,pk);
   }else if(cmpstr == std::string("01")){      //收到L020,則通知bxSes,不做任何
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
      Ses.OnLinkWorkerFinished(pc_L020,pk);     //動作等候交易所登錄通知訊息
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_L1010::LinkReq    (TbxSes& Ses,TPacketCode code,TApCode apcode)      //開機通知訊息
{
   THead pk;
   Kway::TKTime Time;
   if(Ses.GetMarket() == Kway::Tw::Stk::m_Tse)
      pk.SubSysName_.assign("10");
   else if(Ses.GetMarket() == Kway::Tw::Stk::m_Otc)
      pk.SubSysName_.assign("91");
   pk.FunctionCode_.assign("10");
   Time = Time.Now();           //取得現在時間
   pk.MessageTime_.assign(Time.as_string(false));
   pk.StatusCode_.assign("00");
   switch(code)
   {
        case pc_L010:                                   //送出L010
                pk.MessageType_.assign("00");
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, (&pk),sizeof(THead));
                return  Ses.SendPk(pk,sizeof(pk));
        case pc_L020:
                pk.MessageType_.assign("01");           //送出L020
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, (&pk),sizeof(THead));
                return  Ses.SendPk(pk,sizeof(pk));
        default:
                return false;
   }
}
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_L1020::OnPkCheck  (TbxSes& Ses,const THead& pk,
                                        TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("02"))           //收到L030,BodySize = 3
   {
        *PacketSize = sizeof(TL030);
        return cpr_Continue;
   }
   else if(cmpstr == std::string("03"))      //收到L040,BodySize = 10
   {
        *PacketSize =sizeof(TL040);
        return cpr_Continue;
   }
   else if(cmpstr == std::string("04"))      //收到L050
   {
        *PacketSize = CtlHeadSize;
        return cpr_Continue;
   }
   else if(cmpstr == std::string("05"))      //收到L060
   {
        *PacketSize = CtlHeadSize;
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_L1020::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   TL030        L030 = *(TL030*)&pk;
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("02")){           //收到L030
        Ses.SetAppendNo(L030.AppendNo_);
        Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (TL030*)(&pk),sizeof(TL030));
        Ses.OnLinkWorkerFinished(pc_L030,pk);
   }else if(cmpstr == std::string("03")){      //收到 L040,則通知bxSes,不做任何
        Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (TL040*)(&pk),sizeof(TL040));
        Ses.OnLinkWorkerFinished(pc_L040,pk);     //等候啟動應用子系統通知
   }else if(cmpstr == std::string("04")){      //收到 L050,則通知bxSes,並送出L060
        Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
        Ses.OnLinkWorkerFinished(pc_L050,pk);
   }else if(cmpstr == std::string("05")){      //收到 L060,則通知bxSes,不做任何動作
        Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
        Ses.OnLinkWorkerFinished(pc_L060,pk);
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_L1020::LinkReq    (TbxSes& Ses,TPacketCode code,TApCode apcode)      //登錄通知訊息
{
   TL040        L040;
   TSize        password = Ses.GetPassWord();
   Kway::TKTime Time;
   Time = Time.Now();           //取得現在時間
   if(code == pc_L040)          //送出L040 登錄訊息
   {
        if(Ses.GetMarket() == Stk::m_Tse)
           L040.SubSysName_.assign("10");
        else if(Ses.GetMarket() == Stk::m_Otc)
           L040.SubSysName_.assign("91");
//        L040.SubSysName_.assign("10");
        L040.FunctionCode_.assign("20");
        L040.MessageType_.assign("03");
        L040.MessageTime_.assign(Time.as_string(false));
        L040.StatusCode_.assign("00");
        L040.AppendNo_ = Ses.GetAppendNo();
        L040.BrokerID_.assign(Ses.GetBrokerID().as_string());
//printf("%s",L040.BrokerID_.as_string());
        L040.ApCode_ = apcode;
        long appendno = L040.AppendNo_.as_int();
        char buf[20];
        //KeyValue = appendno * password 取千位與百位數字
        sprintf(buf, "%02d", (appendno * password / 100) % 100);
        L040.KeyValue_.assign(buf);
        Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, (&L040),sizeof(L040));
        return Ses.SendPk(L040,sizeof(L040));
   }
   else if(code == pc_L060)
   {
        THead pk;
        if(Ses.GetMarket() == Stk::m_Tse)
           pk.SubSysName_.assign("10");
        else
           pk.SubSysName_.assign("91");
        pk.FunctionCode_.assign("20");
        pk.MessageTime_.assign(Time.as_string(false));
        pk.StatusCode_.assign("00");
        pk.MessageType_.assign("05");           //送出L060
        Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, (&pk),sizeof(pk));
        return  Ses.SendPk(pk,sizeof(pk));
   }
   else
        return false;

}
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_L1030::OnPkCheck  (TbxSes& Ses,const THead& pk,
                                        TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("06") || cmpstr == std::string("07"))
   {
        *PacketSize = CtlHeadSize;
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_L1030::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("06"))           //收到L070,則通知bxSes並回覆 L080
   {
        Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
        Ses.OnLinkWorkerFinished(pc_L070,pk);
   }
   else if(cmpstr == std::string("07")){      //收到L080,則通知bxSes,不做任何動作
        Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
        Ses.OnLinkWorkerFinished(pc_L080,pk);
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_L1030::LinkReq    (TbxSes& Ses,TPacketCode code,TApCode apcode)      //離線作業
{
   THead pk;
   Kway::TKTime Time;
   Time = Time.Now();           //取得現在時間

   if(Ses.GetMarket() == Stk::m_Tse)
      pk.SubSysName_.assign("10");
   else if(Ses.GetMarket() == Stk::m_Otc)
      pk.SubSysName_.assign("91");
//   pk.SubSysName_.assign("10");
   pk.FunctionCode_.assign("30");
   pk.MessageTime_.assign(Time.as_string(false));
   pk.StatusCode_.assign("00");
   switch(code)
   {
        case pc_L070:                                   //送出L070
                pk.MessageType_.assign("06");
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, (&pk),sizeof(pk));
                return  Ses.SendPk(pk,sizeof(pk));
        case pc_L080:
                pk.MessageType_.assign("07");           //送出L080
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, (&pk),sizeof(pk));
                return  Ses.SendPk(pk,sizeof(pk));
        default:
                return false;
   }
}
//---------------------------------------------------------------------------
