//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxFTWorker.h"
#include "bxFT.h"
#include "bxFTSes.h"
#include "FileCtrl.h"
#include "ExcMgrBase.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_FHead::DoReq(TbxSesBase& aSes, const TBxMsg aMsg, const TControlHead& aPkt)
{
        TbxFTSes* ses    = dynamic_cast<TbxFTSes*>(&aSes);
  const TFTHead*  fthead = static_cast <const TFTHead*> (&aPkt);
  return DoFtReq(*ses, aMsg, *fthead);
}
//---------------------------------------------------------------------------
K_mf(int)  TWork_FHead::GetPacketSize(TbxSesBase& aSes, const TbxData& aPkt)
{
  switch(aPkt.GetMsg())
  {
    case mgF010:    case mgF020:
    case mgF030:    case mgF040:
    case mgF050:    case mgF060:
    case mgF210:    case mgF220:
    case mgF070:    case mgF080:
      return sizeof(TFTHead);
    case mgF170:    case mgF180:
      return sizeof(TControlHead);
  }
  return 0;
}
//---------------------------------------------------------------------------
K_mf(int) TWork_FHead::BeforeAPacket(TbxSesBase& aSes, const TbxData& aPkt)
{
  const TControlHead* head = static_cast<const TControlHead*>(&aPkt);
  const TFTHead* fthead    = static_cast<const TFTHead*>(head);
  switch(aPkt.GetMsg())
  {
    case mgF010:    case mgF020:
    case mgF030:    case mgF040:
    case mgF050:    case mgF060:
    case mgF210:    case mgF220:
      return fthead->GetBodyLen() + sizeof(TFTHead);
    case mgF070:    case mgF080:
      return sizeof(TFTHead);
    case mgF170:    case mgF180:
      return sizeof(TControlHead);
  }
  return 0;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_FHead::APacket(TbxSesBase& aSes, const TbxData& aPkt)
{
  TbxFTSes* ses = dynamic_cast<TbxFTSes*>(&aSes);
  if(ses->IsSend() == false)
    ses->StopHB();
  const TControlHead* head   = static_cast<const TControlHead*>(&aPkt);
  const TFTHead*      fthead = static_cast<const TFTHead*>     (head);
  size_t sz;
  if((!ses->IsCompleteLog()) && (aPkt.GetMsg()==mgF030 || aPkt.GetMsg()==mgF210))
    sz = GetPacketSize(aSes, aPkt) + sizeof(TFileCode) + sizeof(TEof);
  else
    sz = BeforeAPacket(aSes, aPkt);
  ses->WriteLog(reinterpret_cast<const char*>(&aPkt), sz, ses->GetPvcID(), 1, ses->GetBrokerID());
  bool ret = true;
  if(ses->CheckStatusCode(aPkt.GetSubSys(),head->GetCode()))
    ret = BodyPacket(*ses, *fthead);
  if(ses->IsSend() == false)
    ses->StartHB();
  return ret;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_F2000::BodyPacket(TbxFTSes& aSes, const TFTHead& aPkt)
{
  const TF010* f010 = static_cast<const TF010*>(&aPkt);
  switch(aPkt.GetMsg())
  {
    case mgF010:    // �_�l�T��, ���������|����
      aSes.SetState(TbxFTSes::efsStart, "����ǿ�_�l");
      aSes.AllocNewFT (*f010);
      aSes.UpdateFTStatus(fft_R_ing, f010->GetCode());
      if(aSes.DoWorker(aSes.GetWorkKey(ckFTStart), mgF020, aPkt))   //�e�XF020
        aSes.StartWaitReplyTimer();
      break;
    case mgF020:    // �^�а_�l�T�� F020, �ǰe�����|����
      aSes.StopWaitReplyTimer();
      aSes.SetState(TbxFTSes::efsStart, "����ǿ�_�l�^��");
      aSes.UpdateFTStatus(fft_S_ing, f010->GetCode());
      if(aSes.DoWorker(aSes.GetWorkKey(ckFTCont), aSes.IsExtend() ? mgF210:mgF030, aPkt))   //�e�XF030/F210
        aSes.StartWaitReplyTimer();
      break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_F2000::DoFtReq(TbxFTSes& aSes, const TBxMsg aMsg, const TFTHead& aPkt)
{
  TFTFileReq* job = dynamic_cast<TFTFileReq*>(aSes.GetJob());
  TF010 f010;
  f010.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
  f010.SourceID_ = job->GetSourceID();
  f010.DestID_   = job->GetDestID();
  f010.SetBodyLen(szCODE_SIZE);
  f010.FileCode_ = job->GetFileCode();
  f010.FileSize_ = job->GetFileSize();
  switch(aMsg)
  {
    case mgF010:
      aSes.SetState(TbxFTSes::efsStart, "�e�X�ǿ�_�l");
      if(GetExcMgr())
         GetExcMgr()->Alert("XF3010", f010.FileCode_.as_string());
      break;
    case mgF020:
      aSes.SetState(TbxFTSes::efsStart, "�^�жǿ�_�l");
      if(GetExcMgr())
         GetExcMgr()->Alert("XF3000",f010.FileCode_.as_string());
      break;
    default:
      return false;
  }
  return aSes.SendPkt(f010);
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_F2001::BodyPacket(TbxFTSes& aSes, const TFTHead& aPkt)
{
  TFTFileReq* job = dynamic_cast<TFTFileReq*>(aSes.GetJob());
  switch(aPkt.GetMsg())
  {
    case mgF030:    // �ǰe��ưT��, ���������|����
    {
      aSes.StopWaitReplyTimer();
      const TF030* f030 = static_cast<const TF030*>(&aPkt);
      if(f030->IsEof()){
        aSes.SetState(TbxFTSes::efsStop, "�ɮפU������");
        if(GetExcMgr())
           GetExcMgr()->Alert("XF3001", f030->FileCode_.as_string());
      }else
        aSes.SetState(TbxFTSes::efsFT, "�ɮפU����");
      job->DownLoad(f030->Data_.begin(), f030->GetBodyLen() - szCODE_EOF);
      if(aSes.DoWorker(aSes.GetWorkKey(ckFTCont), mgF040, aPkt)) //�e�XF040
      {
        if(f030->IsEof()){
          aSes.JobFinish(f030->GetCode());
        }else{
          aSes.StartWaitReplyTimer();
        }
      }
      break;
    }
    case mgF040:    // �^�и�ưT��, �ǰe�����|����
      aSes.StopWaitReplyTimer();
      if(job->IsEof())
      {
        aSes.SetState(TbxFTSes::efsStop, "�ɮפW�ǧ���");
        aSes.JobFinish(aPkt.GetCode());
        if(GetExcMgr())
           GetExcMgr()->Alert("XF3011",job->GetFileCode().as_string());
      }
      else
      {
        aSes.SetState(TbxFTSes::efsFT, "�ɮפW�Ǥ�");
        if(aSes.DoWorker(aSes.GetWorkKey(ckFTCont), mgF030, aPkt)) //�e�XF040
          aSes.StartWaitReplyTimer();
      }
      break;
    case mgF210:
    {
      aSes.StopWaitReplyTimer();
      const TF210* f210 = static_cast<const TF210*>(&aPkt);
   if(f210->IsEof()){
        aSes.SetState(TbxFTSes::efsStop, "�ɮפU������");
        if(GetExcMgr())
           GetExcMgr()->Alert("XF3001",f210->FileCode_.as_string());
      }else
        aSes.SetState(TbxFTSes::efsFT,   "�ɮפU����");
      job->DownLoad(f210->LargeData_.begin(), f210->GetBodyLen() - szCODE_EOF);
      if(aSes.DoWorker(aSes.GetWorkKey(ckFTCont), mgF220, aPkt)) //�e�XF220
      {
        if(f210->IsEof())
          aSes.JobFinish(f210->GetCode());
        }else{
          aSes.StartWaitReplyTimer();
        }
      break;
    }
    case mgF220:
      aSes.StopWaitReplyTimer();
      if(job->IsEof())
      {
        aSes.SetState(TbxFTSes::efsStop, "�ɮפW�ǧ���");
        aSes.JobFinish(aPkt.GetCode());
      }
      else
      {
        aSes.SetState(TbxFTSes::efsFT, "�ɮפW�Ǥ�");
        if(aSes.DoWorker(aSes.GetWorkKey(ckFTCont), mgF210, aPkt)) //�e�XF040
          aSes.StartWaitReplyTimer();
      }
      break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_F2001::DoFtReq(TbxFTSes& aSes, const TBxMsg aMsg, const TFTHead& aPkt)
{
  TStCode code = aPkt.GetCode();
  TFTFileReq* job = dynamic_cast<TFTFileReq*>(aSes.GetJob());
  switch(aMsg)
  {
    case mgF030:
    {
      TF030 f030;
      int len = job->UpLoad(f030.Data_.begin(), sizeof(f030.Data_));
      f030.SetHead   (GetWorkKey(), aMsg, code);
      f030.SourceID_ = job->GetSourceID();
      f030.DestID_   = job->GetDestID();
      f030.SetBodyLen(len + szCODE_EOF);
      f030.FileCode_ = job->GetFileCode();
//      f030.SetEof    (len != sizeof(f030.Data_));
      f030.SetEof    (job->IsEof());
      aSes.SetState  (TbxFTSes::efsFT, "�ɮפW�Ǥ�");
      return aSes.SendPkt(f030);
    }
    case mgF040:
    case mgF220:
    {
      TF040 f040;
      const TF040* c_f040 = static_cast<const TF040*>(&aPkt);
      f040.SetHead   (GetWorkKey(), aMsg, code);
      f040.SourceID_ = c_f040->DestID_;
      f040.DestID_   = c_f040->SourceID_;
      f040.SetBodyLen(szCODE_EOF);
      f040.FileCode_ = c_f040->FileCode_;
      f040.Eof_      = c_f040->Eof_;
      if(f040.IsEof())
        aSes.SetState(TbxFTSes::efsStop, "�^���ɮפU������");
      else
        aSes.SetState(TbxFTSes::efsFT,   "�^���ɮפU��");
      return aSes.SendPkt(f040);
    }
    case mgF210:
    {
      TF210 f210;
      int len = job->UpLoad(f210.LargeData_.begin(), sizeof(f210.LargeData_));
      f210.SetHead   (GetWorkKey(), aMsg, code);
      f210.SourceID_ = job->GetSourceID();
      f210.DestID_   = job->GetDestID();
      f210.SetBodyLen(len + szCODE_EOF);
      f210.FileCode_ = job->GetFileCode();
      f210.SetEof    (job->IsEof());
//      f210.SetEof    ( len != sizeof(f210.LargeData_));
      aSes.SetState  (TbxFTSes::efsFT, "�ɮפW�Ǥ�");
      return aSes.SendPkt(f210);
    }
  }
  return false;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_F2002::BodyPacket(TbxFTSes& aSes, const TFTHead& aPkt)
{
  const TF050* f050 = static_cast<const TF050*>(&aPkt);
  TFTMsgReq* job;
  size_t sz = f050->GetBodyLen() - sizeof(f050->FileCode_);
  const_cast<TMsgData&>(f050->Msg_).set(sz, 0);
  switch(aPkt.GetMsg())
  {
    case mgF050:    // �����浧�T�� F050, ���������|����
      aSes.SetState(TbxFTSes::efsMsg, "����浧�T��");
      aSes.AllocNewMsg(*f050);
      aSes.DoWorker(aSes.GetWorkKey(ckFTMsg), mgF060, aPkt); //�e�XF060
      aSes.JobFinish(aPkt.GetCode()); // 20080416 �ѨM����S10 ����L�k�e�X�ƶ����u�@�����D
      break;
    case mgF060:    // �����^�г浧�_�l�T�� F060, �ǰe�����|����
      aSes.StopWaitReplyTimer();
      aSes.SetState(TbxFTSes::efsStop, "����浧�T���^��");
      job = dynamic_cast<TFTMsgReq*>(aSes.GetJob());
      job->SetRpyMsg(f050->Msg_);
      aSes.JobFinish(aPkt.GetCode());
      break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_F2002::DoFtReq(TbxFTSes& aSes, const TBxMsg aMsg, const TFTHead& aPkt)
{
  TFTMsgReq* job = dynamic_cast<TFTMsgReq*>(aSes.GetJob());
  if(job == NULL)
    return false;

  TF050 f050;
  f050.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
  f050.FileCode_ = job->GetFileCode();
  switch(aMsg)
  {
    case mgF050:    // �ǰe�浧�T��
      f050.SourceID_ = job->GetSourceID();
      f050.DestID_   = job->GetDestID();
      f050.Msg_      = job->GetReqMsg();
      f050.SetBodyLen(strlen(f050.Msg_.begin()) + szCode);
      aSes.UpdateFTStatus(fft_S_ing, aPkt.GetCode());
      aSes.SetState(TbxFTSes::efsMsg,  "�e�X�浧�T��");
      break;
    case mgF060:    // �^�г浧�_�l�T��
      f050.SourceID_ = job->GetDestID();
      f050.DestID_   = job->GetSourceID();
      f050.Msg_      = job->GetRpyMsg();
      f050.SetBodyLen(strlen(f050.Msg_.begin()) + szCode);
      aSes.SetState(TbxFTSes::efsStop, "�^�г浧�T��");
      aSes.JobFinish(aPkt.GetCode());
      break;
    default:
      return false;
  }
  return aSes.SendPkt(f050);
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_F2003::BodyPacket(TbxFTSes& aSes, const TFTHead& aPkt)
{
  switch(aPkt.GetMsg())
  {
    case mgF070:        // �ǰe�����@�~, ���������|����
      aSes.SetState(TbxFTSes::efsStop, "���쵲���@�~");
      aSes.DoWorker(aSes.GetWorkKey(ckFTEnd), mgF080, aPkt);
      break;
    case mgF080:        // �^�е����@�~, �ǰe�����|����
      aSes.StopWaitReplyTimer();
      aSes.SetState(TbxFTSes::efsStop, "���쵲���@�~�^��");
      aSes.UnLink();    // ���u�@�~
      break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_F2003::DoFtReq(TbxFTSes& aSes, const TBxMsg aMsg, const TFTHead& aPkt)
{
  TFTHead data;
  data.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
  data.SourceID_ = aSes.GetBrokerID();
  data.DestID_   = TSEID_;
  data.SetBodyLen(0);
  switch(aMsg)
  {
    case mgF070:    // �ǰe�����@�~
      aSes.SetState(TbxFTSes::efsStop, "�e�X�����@�~");
      break;
    case mgF080:    // �^�е����@�~
      aSes.SetState(TbxFTSes::efsStop, "�^�е����@�~");
      aSes.UnLink();
      break;
    default:
      return false;
  }
  return aSes.SendPkt(aPkt);
}
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TWork_F2004::BodyPacket(TbxFTSes& aSes, const TFTHead& aPkt)
{
  switch(aPkt.GetMsg())
  {
    case mgF170:        // �����s�u�@�~, ���������|����
      aSes.SetState(TbxFTSes::efsHeartbeat, "����s�u�T�{");
      aSes.DoWorker(aSes.GetWorkKey(ckFTLink), mgF180, aPkt);
      break;
    case mgF180:        // �����^�гs�u�@�~, �ǰe�����|����
      aSes.SetState(TbxFTSes::efsStop, "����s�u�^��");
      //20090525 �ˬd���L�b�ƶ������@�~����heartbeat������
      if(aSes.IsSend())
         aSes.FetchNextJob();
      break;
    default:
      return false;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TWork_F2004::DoFtReq(TbxFTSes& aSes, const TBxMsg aMsg, const TFTHead& aPkt)
{
  TbxData data;
  data.SetHead(GetWorkKey(), aMsg, aPkt.GetCode());
  switch(aMsg)
  {
    case mgF170:    // �ǰe�s�u�@�~
      aSes.SetState(TbxFTSes::efsHeartbeat, "�e�X�s�u�T�{");
      break;
    case mgF180:    // �^�гs�u�@�~
      aSes.SetState(TbxFTSes::efsStop, "�^�гs�u�T�{");
      break;
    default:
      return false;
  }
  const TControlHead& head = static_cast<const TControlHead&>(data);
  const TFTHead&        ft = static_cast<const TFTHead&>(head);
  return aSes.SendPkt(ft);
}
//---------------------------------------------------------------------------

