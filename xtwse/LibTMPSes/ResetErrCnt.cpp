//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ResetErrCnt.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kway::Tw::bx;
//---------------------------------------------------------------------------
K_ctor TResetCntSes::TResetCntSes(TSesMgr* sm)
   :inherited(1024,(char*)std::string("\n").c_str())
{
   SesMgr_   = (TbxSesMgr*)sm;
}
//---------------------------------------------------------------------------
K_mf(void) TResetCntSes::OnPktRecv(void* vbuf, int sz)
{
   CurSeqNo_++;
   if(CurSeqNo_ <= LastSeqNo_)
      return;
   char buf[512];
   TResetCnt RstCnt;
   memcpy(&RstCnt, vbuf, sizeof(TResetCnt));
   if(GetProcBrkID() == TBrokerID(RstCnt.BrokerID_.as_string())){ 
      //�o�إu���b�P�_�O�ݩ�ۤv���s�uid �����p�~���g�ɰʧ@,�u���F�קK���s�Ұʮ�
      //���n���ư���� reset ���ʧ@, �H�δ�ֽL��IO������
      LastSeqNo_ = CurSeqNo_;
      SesMgr_->SetMsgCnt(LastSeqNo_);
      SesMgr_->ResetErrCnt(GetSubSystemAP(GetProcMkt(),RstCnt.ApCode_[0]));
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TResetCntSes::SettingApply(const std::string& str)
{
   LastSeqNo_ = SesMgr_->GetMsgCnt();
   CurSeqNo_  = 0;
   return true;
}
//---------------------------------------------------------------------------

