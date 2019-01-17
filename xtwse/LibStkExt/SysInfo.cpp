//---------------------------------------------------------------------------
#pragma hdrstop
#include "SysInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace Kstl;
using namespace Kway;
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
K_ctor TSysInfoPage::TSysInfoPage  (TScheduleMgr* schm, TOrdCenter* oc)
//   : Recorder_(SysInfo_, "./Settings/SysInfo.rec")
{
   ScheduleMgr_ = schm;
   Date_ = TKDate::Now();
   Reg(0,"�t�Υ����",Date_.as_string().c_str());
   Reg(1,"�t�Υ���ɬq","��ѥ��");
   Reg(2,"TSE �u�����A","�L�i�νu��!");
   Reg(3,"OTC �u�����A","�L�i�νu��!");
   TseReady_     = 0;
   OtcReady_     = 0;
   OtcRReady_    = 0;
   TseOddReady_  = 0;
   OtcOddReady_  = 0;
   OtcROddReady_ = 0;
//   Reg("���d�u�����A","�L�i�νu��");
   UpdateState(1,"��ѥ��");
   MarketPh_   = ts_Full;
   TrMarketPh_ = ts_Early;
   TSysKey id;
   char buf[10];
   sprintf(buf,"%d",0); 
   id.assign(buf);
   TSysi i = SysInfo_.find(id);
   if(i->second.State_.as_string() !=  Date_.as_string()){
      UpdateState(0,Date_.as_string().c_str());
   } 
   int oid = 0;
   while (1) {              // �b�إ߮ɭn���N�v�s�b��ordcenter�������L�q�����X
      TOrdCenter::TOrdHnd Hnd = oc->GetOrdHnd(TSysOrdID(++oid),uf_OrdFlow);
      if (Hnd.is_invalid())
         break;
      TStkOrder& Ord =  Hnd.GetModify();
      if(Ord.LastOCmd_ == cid_Bulletin)
         switch(Ord.FlowID_.GetStep()){
            case 1:SetMarketPh(ts_Fix); break;
            case 2:SetMarketPh(ts_Odd); break;
            default:break;
         }
   }
   UpdateState(2,"�L�i�νu��!");
   UpdateState(3,"�L�i�νu��!");
/*
   SchNormalOpen_   = new TSchedObj(this,"��ѥ���}�l","08:30:00",&this_type::NormalOpen);
   SchNormalClose_  = new TSchedObj(this,"��ѥ������","13:30:00",&this_type::NormalClose);
   SchFixedOpen_    = new TSchedObj(this,"�L�����}�l","14:00:00",&this_type::FixedOpen);
   SchFixedClose_   = new TSchedObj(this,"�L��������","14:30:00",&this_type::FixedClose);
   SchOddOpen_      = new TSchedObj(this,"�s�ѥ���}�l","15:00:00",&this_type::OddOpen);
   SchOddClose_     = new TSchedObj(this,"�s�ѥ������","16:00:00",&this_type::OddClose);
   ForceOpen_       = new TSchedObj(this,"�j��o�e�}�L�H��","08:30:00",&this_type::ForceOpen);
   ScheduleMgr_->Reg(SchNormalOpen_);
   ScheduleMgr_->Reg(SchNormalClose_);
   ScheduleMgr_->Reg(SchFixedOpen_);
   ScheduleMgr_->Reg(SchFixedClose_);
   ScheduleMgr_->Reg(SchOddOpen_);
   ScheduleMgr_->Reg(SchOddClose_);
   ScheduleMgr_->Reg(ForceOpen_);
   SysParam_ = new TSettingPage("./Settings/SysParam.rec",
                                std::string(GetProcLogPath()+"/ModifyLog/SysParam.log").c_str()); 
   SysParam_->Reg(&UseMainPath_, "�D�n���|", "./",echar_ary,&MainPath_); 
   SysParam_->Reg(&UseSecPath_, "�ƥ����|", "./Bak/",echar_ary,&SecPath_); 
   SysParam_->Reg(&UseBrkID_, "���b�M���ӥN��", "",echar_ary,&BrkID_);
   SysParam_->Reg(&UseIvacNo_, "���b�M��b��", "",echar_ary,&IvacNo_);
   SysParam_->Reg(&RecLdckLdrk_, "����ldck / ldrk(�e�U/����O����)", "N/A");
   SysParam_->Reg(&RecClOrdID_, "�O��ldck�ɥ]�tClOrdID", "N/A");
   SysParam_->SettingApply();
*/
}
//---------------------------------------------------------------------------
K_dtor TSysInfoPage::~TSysInfoPage  ()
{
/*
   delete SysParam_;
   delete SchNormalOpen_;
   delete SchNormalClose_;
   delete SchFixedOpen_;
   delete SchFixedClose_;
   delete SchOddOpen_;
   delete SchOddClose_;
   delete ForceOpen_;
*/
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::Reg(int id,const char* item, const char* state)
{
   TSysKey Sz_;
   char buf[10];
   sprintf(buf,"%d",id);
   Sz_.assign(buf);
   if(SysInfo_.find(Sz_) != SysInfo_.end())
      return;
   SSysInfo rec;
   rec.Item_.assign(item);
   rec.State_.assign(state);
   SysInfo_.insert(std::make_pair(Sz_,rec));
   return;
}
//---------------------------------------------------------------------------
K_mf(TViewArea*) TSysInfoPage::GetVA  ()
{
   TViewControls   vcs;
   TSysInfoVA* SVA = (new TSysInfoVA(&SysInfo_));
   return (SVA);
}
//---------------------------------------------------------------------------
K_mf(TaViewArea) TSysInfoPage::CreateNewVA  ()
{
   TViewControls   vcs;
   vcs.set(vco_Setting);
   TSysInfoVA* SVA = (new TSysInfoVA(&SysInfo_));
   return TaViewArea(SVA);
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::UpdateState(int seq,const char* state)
{
   TSysKey id;
   char buf[10];
   sprintf(buf,"%d",seq);
   id.assign(buf);
   TSysi i = SysInfo_.find(id);
   if (i == SysInfo_.end())
      return;
   TSysu u = SysInfo_.begin_update(i);
   u->State_.assign(state);
   TKTime Time_;
   u->Memo_.assign(Time_.Now().as_string());
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::SetMarketPh(ETrxSegment m)
{
   std::string mph;
   MarketPh_ = m;
   switch(m){
      case ts_Fix: mph = std::string("�s��+�L��"); break;
      case ts_Odd: mph = std::string("�s�ѥ��"); break;
      default:     mph = std::string("��ѥ��"); break; 
   }
   AlertAll(eMarketPh);
   UpdateState(1,mph.c_str());
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::SetLineState(TMarket mkt, Kway::word s,ETradeKind tk)
{
   if((tk == tk_Normal && MarketPh_ != ts_Full) || (tk == tk_Fix && MarketPh_ != ts_Fix))
      return;
   int id;
   char buf[64];
   switch(mkt){
      case m_Tse:
      default: 
         if(tk == tk_Odd)
            TseOddReady_ = s;
         else
            TseReady_ = s;
         id = 2;
         AlertAll(eTseLine);
         break;
      case m_Otc:
         if(tk == tk_Odd)
            OtcOddReady_ = s;
         else
            OtcReady_ = s;
         id = 3;
         AlertAll(eOtcLine);
         break;
      case m_OtcR:
         return;
   }
   if(mkt == m_Tse){
      switch(MarketPh_){
         case ts_Full: sprintf(buf,"���(%d)",TseReady_); break;
         case ts_Odd : sprintf(buf,"�s��(%d)",TseOddReady_);break;
         case ts_Fix : sprintf(buf,"�s��(%d), �L��(%d)",TseOddReady_,TseReady_);break;
      }
   }else if(mkt == m_Otc){
      switch(MarketPh_){
         case ts_Full: sprintf(buf,"���(%d)",OtcReady_);break;
         case ts_Odd : sprintf(buf,"�s��(%d)",OtcOddReady_);break;
         case ts_Fix : sprintf(buf,"�s��(%d), �L��(%d)",OtcOddReady_,OtcReady_);break;
      }
   }
   UpdateState(id,buf);
}
//---------------------------------------------------------------------------
K_mf(bool) TSysInfoPage::GetLineState(TMarket mkt,TTrxSegment ts)
{
   switch(mkt){
      case m_Tse:  return (ts==ts_Odd?TseOddReady_:TseReady_);
      case m_Otc:  return (ts==ts_Odd?OtcOddReady_:OtcReady_);
      case m_OtcR: return (ts==ts_Odd?OtcROddReady_:OtcRReady_);
      default: return false;
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TSysInfoPage::Add(TSysEvHnd* hnd)
{
   for(std::vector<TSysEvHnd*>::iterator L = HndList_.begin(); L != HndList_.end(); L++){
      if((TSysEvHnd*)(*L) == hnd)
         return false;
   }
   HndList_.push_back(hnd);
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TSysInfoPage::Del(TSysEvHnd* hnd)
{
   for(std::vector<TSysEvHnd*>::iterator L = HndList_.begin(); L != HndList_.end(); L++){
      if((TSysEvHnd*)(*L) == hnd){
         HndList_.erase(L);
         return true;
      }
   }
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::AlertAll(eStateChg esc)
{
   if(HndList_.size()>0)
      for(std::vector<TSysEvHnd*>::iterator L = HndList_.begin(); L != HndList_.end(); L++){
         if(((TSysEvHnd*)(*L)) == NULL){
            continue;
         }
         ((TSysEvHnd*)(*L))->Alert(esc);
      }
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::NormalOpen(TKMemo& Memo)
{
   if(MarketPh_ >=ts_Full  && MarketPh_ < ts_FullLate){
      Memo.assign("��ѥ���}�l!");
      TrMarketPh_ = ts_Full;
      AlertAll(eNormalOpen);
   }else
      Memo.assign("�D��ѥ���ɬq!");
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::NormalClose(TKMemo& Memo)
{
   Memo.assign("��ѥ������!");
   TrMarketPh_ = ts_FullLate;
   AlertAll(eNormalClose);
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::FixedOpen(TKMemo& Memo)
{
   if(MarketPh_ >=ts_Fix  && MarketPh_ < ts_FixLate){
      Memo.assign("�L�����}�l!");
      TrMarketPh_ = ts_Fix;
      AlertAll(eFixedOpen);
   }else
      Memo.assign("�D�L�����ɬq!");
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::FixedClose(TKMemo& Memo)
{
   Memo.assign("�L��������!");
   TrMarketPh_ = ts_FixLate;
   AlertAll(eFixedClose);
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::OddOpen(TKMemo& Memo)
{
   if(MarketPh_ >= ts_Odd && MarketPh_ < ts_OddLate){
      Memo.assign("�s�ѥ���}�l!");
      TrMarketPh_ = ts_Odd;
      AlertAll(eOddOpen);
   }else
      Memo.assign("�D�s�ѥ���ɬq!");
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::OddClose(TKMemo& Memo)
{
   Memo.assign("�s�ѥ������!");
   TrMarketPh_ = ts_OddLate;
   AlertAll(eOddClose);
}
//---------------------------------------------------------------------------
K_mf(void) TSysInfoPage::ForceOpen(TKMemo& Memo)
{
   Memo.assign("�j��o�e�}�L�H��!");
   AlertAll(eForceOpen);
}
//---------------------------------------------------------------------------
