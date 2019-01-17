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
      //這裏只有在判斷是屬於自己的連線id 的狀況才做寫檔動作,只為了避免重新啟動時
      //不要重複執行到 reset 的動作, 以及減少盤中IO的次數
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

