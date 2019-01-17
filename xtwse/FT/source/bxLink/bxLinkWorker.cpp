#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxLinkWorker.h"
#include "bxLink.h"
#include "KTime.h"
#include "bxLinkSes.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_Link::APacket(TbxSesBase& aSes, const TbxData& aPkt)
{
  TbxLinkSes* ses = dynamic_cast<TbxLinkSes*>(&aSes);
  if(!ses)
    return false;
  ses->WriteLog(reinterpret_cast<const char*>(&aPkt), GetPacketSize(aSes, aPkt), ses->GetPvcID(), 1, ses->GetBrokerID());
  DoJob(*ses, aPkt);
  return true;
}
//---------------------------------------------------------------------------
K_mf(int)  TWork_Link::GetPacketSize(TbxSesBase& aSes, const TbxData& aPkt)
{
  switch(aPkt.GetMsg())
  {
    case mgL010:    case mgL020:
    case mgL050:    case mgL060:
    case mgL070:    case mgL080:
      return szTControlHead;
    case mgL030:    return sizeof(TL030);   //����L030,BodySize = 3
    case mgL040:    return sizeof(TL040);   //����L040,BodySize = 10
  }
  return 0;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_Link::DoReq(TbxSesBase& aSes, const TBxMsg aMsg, const TControlHead& aHead)
{
  TbxLinkSes* ses = dynamic_cast<TbxLinkSes*>(&aSes);
  return ses ? DoWorker(*ses, aMsg, aHead):false;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_L1010::DoJob(TbxLinkSes& aSes, const TControlHead& aPkt)
{
  TStCode code = aPkt.GetCode();
  TSubSysName subs = aPkt.GetSubSys();
  switch(aPkt.GetMsg())
  {
    case mgL010:    // ����L010,�h�q��bxSes�æ^�� L020
        //���p�G�w��Mgr���U�FSub System Ses �n������
        aSes.OnSubSysBroken(aPkt.GetSubSys(), code);
        switch(code)
        {
          case lec_Success:
            if(aSes.DoWorker(aSes.GetWorkKey(ckWakeup), mgL020, aPkt))   //�e�XL020
            {
              aSes.SetState(TbxLinkSes::ls_WaitWakeup, "Send L020");
              aSes.StartLinkTimer();
            }
            break;
          case lec_SysNotReady:   // ����ɶ�����,�@������A�s�u
            aSes.SetError(((TControlHead&)aPkt).GetSubSys().as_string(), ((TControlHead&)aPkt).GetStCode().as_string());
            aSes.StartLinkTimer();
            break;
          case lec_TimeErr:       // �䥦���~
          case lec_CallTse:       // Call TSE
            aSes.SetError(((TControlHead&)aPkt).GetSubSys().as_string(), ((TControlHead&)aPkt).GetStCode().as_string());
            break;
          default:
            aSes.SetError(((TControlHead&)aPkt).GetSubSys().as_string(), ((TControlHead&)aPkt).GetStCode().as_string());
            aSes.ReLink();
            break;
        }
        break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_L1010::DoWorker(TbxLinkSes& aSes, const TBxMsg aMsg, const TControlHead& aPkt)
{
  TbxData l010;
  switch(aMsg)
  {
    case mgL010:    //�e�XL010
    case mgL020:    //�e�XL020
      l010.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
      return aSes.Send(l010);
  }
  return false;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_L1020::DoJob(TbxLinkSes& aSes, const TControlHead& aPkt)
{
  const TControlHead* head = static_cast<const TControlHead*>(&aPkt);
  const TL030* l030;
  TStCode code = aPkt.GetCode();
  switch(aPkt.GetMsg())
  {
    case mgL030:    //����L030
      l030 = static_cast<const TL030*>(head);
      aSes.SetAppendNo(l030->AppendNo_);
      switch(code)
      {
        case lec_Success:
          if(aSes.DoWorker(aSes.GetWorkKey(ckLogon), mgL040, aPkt))  //�e�XL040
          {
            aSes.SetState(TbxLinkSes::ls_WaitLogon, "Send L040");
            aSes.StartLinkTimer();
          }
          break;
        case lec_AppNoErr:      //AppendNo ���~
        case lec_BrkIDErr:      //BrokerID ���~
        case lec_ApCodeErr:     //ApCode ���~
        case lec_KeyValueErr:   //KeyValue ���~
          aSes.SetError(((TControlHead&)aPkt).GetSubSys().as_string(), ((TControlHead&)aPkt).GetStCode().as_string());
          break;
        default:
          aSes.SetError(((TControlHead&)aPkt).GetSubSys().as_string(), ((TControlHead&)aPkt).GetStCode().as_string());
          aSes.ReLink();
          break;
      }
      break;
    case mgL050:    //�������� L050 �Ұʤl�t�ΰT,�h�q��bxSes,�ðe�XL060
      switch(code)
      {
        case lec_Success:
          if(aSes.DoWorker(aSes.GetWorkKey(ckLogon), mgL060, aPkt))  // Send L060
          {
            aSes.SetState(TbxLinkSes::ls_LinkReady, "SubSystem Ready");
            // �q��SESSION �l�t�Τw�gREADY
            aSes.OnSubSysReady(aSes.GetApCode());
          }
          break;
        default:
          aSes.SetError(((TControlHead&)aPkt).GetSubSys().as_string(), ((TControlHead&)aPkt).GetStCode().as_string());
          aSes.ReLink();
          break;
      }
      break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_L1020::DoWorker(TbxLinkSes& aSes, const TBxMsg aMsg, const TControlHead& aPkt)
{
  TbxData data;
  TControlHead* head;
  TL040* l040;
  char buf[5];
  switch(aMsg)
  {
    case mgL040:    //�e�XL040 �n���T��
      head = static_cast<TControlHead*>(&data);
      l040 = static_cast<TL040*>(head);
      l040->SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
      l040->AppendNo_ = aSes.GetAppendNo ();
      l040->BrokerID_ = aSes.GetBrokerID ();
      l040->ApCode_   = aSes.GetApCode   ();
      //KeyValue = appendno * password ���d��P�ʦ�Ʀr
      sprintf(buf, "%02d", (l040->AppendNo_.as_int() * aSes.GetPassword() / 100) % 100);
      l040->KeyValue_.assign(buf);
      return aSes.Send(data);
    case mgL060:    //�e�XL060 �T�{���Τl�t�αҰʰT��
      data.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
      return aSes.Send(data);
  }
  return false;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_L1030::DoJob(TbxLinkSes& aSes, const TControlHead& aPkt)
{
  TStCode code = aPkt.GetCode();
  switch(aPkt.GetMsg())
  {
    case mgL070:    //��������L070�����@�~�T��,�h�q��bxSes�æ^�� L080
      switch(code)
      {
        case lec_Success:
          if(aSes.DoWorker(aSes.GetWorkKey(ckLogout), mgL080, aPkt))
            aSes.SetState(TbxLinkSes::ls_Logout, "Send L080");
        default:
          aSes.SetError(((TControlHead&)aPkt).GetSubSys().as_string(), ((TControlHead&)aPkt).GetStCode().as_string());
          aSes.ReLink();
          break;
      }
      break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_L1030::DoWorker(TbxLinkSes& aSes, const TBxMsg aMsg, const TControlHead& aPkt)
{
  TbxData data;
  switch(aMsg)
  {
    case mgL080:    //�e�XL080
      data.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
      return aSes.Send(data);
  }
  return false;
}
//---------------------------------------------------------------------------




