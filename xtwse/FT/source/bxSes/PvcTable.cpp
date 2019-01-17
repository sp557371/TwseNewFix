//---------------------------------------------------------------------------
#pragma hdrstop
#include <stdio.h>
#include "PvcTable.h"
#include "FTSysSetting.h"
#include "bxSesMgr.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace Kstl;
using namespace Kway;
//---------------------------------------------------------------------------
K_ctor TBrkPvcVAC::TBrkPvcVAC  ()
{
   Recs_ = new TBrkPvcList();          
//   Recorder_ = new TPvcRecorder(Recs_, "./Settings/PvcTable.rec");
   Recorder_ = new TPvcRecorder(Recs_, (char*)std::string(GetProcConfPath()+"/PvcTable.rec").c_str());
   if(Recs_->size() > 0)
      for(TBrkPvcList::iterator iter = Recs_->begin();iter != Recs_->end(); iter++)
         for(TPvcList::iterator L = iter->second.begin(); L!= iter->second.end(); L++)
            if(!L->second.OrigPwd_.empty()){
               L->second.Remark_.assign("變更尚未完成,請重新要求配置表!"); 
               L->second.OrigPwd_.clear();
            }else  
               L->second.Remark_.clear();
}
//---------------------------------------------------------------------------
K_dtor TBrkPvcVAC::~TBrkPvcVAC  ()
{
   delete Recs_;          
   delete Recorder_;

}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&)  TBrkPvcVAC::TBrkPvcFieldMaker::GetFieldDesp  (field_n_type n) const
{
   static Kway::TFieldDesp f[] =
   {
      {"券商-市場別",  12 , Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null,},
   };
   return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TBrkPvcVAC::TBrkPvcFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
   switch(fieldNo){
      case 0:  return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(size_t) TBrkPvcVAC::GetDetails(const char**& a)
{
   static const char* desp[] = {"券商-市場別"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TBrkPvcVAC::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
      case 0:return &DetailCreater_;
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TBrkPvcVAC::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TBrkPvcViewer& va=static_cast<TBrkPvcViewer&>(masterVA);
   const TBrkPvcViewer::cursor_type *cur=va.GetCurRow();
   TPvcVA *DetailView =static_cast<TPvcVA*>(detailVA);
   if(cur->IsInvalid()){
      DetailView->SetContainer(this, &this_type::SettingApply,0);
   }else{
      DetailView->SetContainer(this, &this_type::SettingApply, &(cur->GetIterator()->second));
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(bool) TBrkPvcVAC::SettingApply(TPvcList* orig, TPvcList* shadow)
{
   Kway::TFile file;
   char buf[256];
   Kway::Tw::Bx::TFTSysSetting& set = const_cast<Kway::Tw::Bx::TFTSysSetting&>(Kway::Tw::Bx::GetSysSetting());
   std::string dstpath = set.GetSetting(Kway::Tw::Bx::ssi_SaveTo);
   sprintf(buf,"%cB37S.%s",orig->get_master_key().Market_.as_string()[0],
                           orig->get_master_key().BrokerID_.as_string().c_str());
   std::string fname = std::string(buf);
   sprintf(buf,"%s/%s", dstpath.c_str(), fname.c_str());
   SB37 B37;
   bool ChgFT = false;
   TPvcList::iterator Ls = shadow->begin();
   for(TPvcList::iterator Lo = orig->begin(); Lo != orig->end(); Lo++){
      TPvcList::updater u   = orig->begin_update(Lo);
      if(Lo->second.PassWord_ != Ls->second.PassWord_){
         if(!file.IsOpened())
            file.Open(buf, TFileMode(fmWrite|fmOpenAlways|fmCreatePath|fmTrunc));
         if(!file.IsOpened())
            return false;
      	 B37.BrokerID_.assign(orig->get_master_key().BrokerID_.as_string());
      	 B37.Type_.assign("02");  // 20080407 二路FT新制, 修改密碼
      	 B37.PvcID_    = Lo->first;
      	 B37.OrigPwd_  = Lo->second.PassWord_;
      	 B37.NewPwd_   = Ls->second.PassWord_;
      	 u->OrigPwd_   = Lo->second.PassWord_;
      	 u->PassWord_  = Ls->second.PassWord_;
         file.Write(&B37, sizeof(SB37));
         Ls->second.Remark_.assign("密碼變更處理中...");
      }
      if(Lo->second.Default_ == false && Ls->second.Default_ == true){
      	 char tmp = Lo->second.TermNo_[3];
      	 if((tmp >= 'D' && tmp <='F') || (tmp >= 'M' && tmp <='O')){
      	 }else
      	    continue;
         if(!file.IsOpened())
            file.Open(buf, TFileMode(fmWrite|fmOpenAlways|fmCreatePath|fmTrunc));
         if(!file.IsOpened())
            return false;
      	 B37.BrokerID_.assign(orig->get_master_key().BrokerID_.as_string());
      	 B37.Type_.assign("01");  // 20080407 二路FT新制, 修改預設接收管道
      	 B37.PvcID_.assign((tmp >= 'D' && tmp <='F')?"01":"02"); // 預收FT-R 的組別
      	 B37.OrigPwd_.clear();
      	 B37.NewPwd_.clear();
      	 u->OrigDef_ = Lo->second.Default_;
      	 u->Default_ = Ls->second.Default_;
         file.Write(&B37, sizeof(SB37));
         Ls->second.Remark_.assign("變更預設FT-R處理中...");
         ChgFT = true;
      }
      Ls++;
   }
   if(ChgFT){
      for(TPvcList::iterator L = orig->begin(); L != orig->end(); L++){
         char tmp = L->second.TermNo_[3];
         if((tmp >= 'D' && tmp <='F') || (tmp >= 'M' && tmp <='O'))
            if(L->second.Default_ == L->second.OrigDef_){
               TPvcList::updater u   = orig->begin_update(L);
               u->Default_ = !L->second.Default_;
            }
      }
   }
   if(file.GetFileSize() < sizeof(SB37))
      return false;
   Kway::word len = file.GetFileSize();
   file.Close();
   Kway::Tw::Bx::TBrkID brk;
   brk.assign(B37.BrokerID_.as_string());
   Kway::Tw::Bx::GetBxMgr()->SendB37(fname, brk, orig->get_master_key().Market_, len);
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TBrkPvcVAC::ReadB36(std::string& fname, const TBrokerID& brk, TMarket mkt)
{
   Kway::TFile file;
   file.Open(fname.c_str(), TFileMode(Kway::fmRead));
   SB36     B36;
   file.Open(fname.c_str(), TFileMode(Kway::fmRead));
   if(file.IsOpened() && file.GetFileSize() >= sizeof(SB36)){
      file.Read(&B36, sizeof(SB36));
   } else
      return;
   SBrkMkt BrkMkt(TBrokerID(B36.BrokerID_.as_string()),mkt);
   TBrkPvcList::iterator iter = Recs_->fetch(BrkMkt);
   iter->second.clear();
   
   char buf[64];
   int recs;
   if(file.IsOpened()){
      int defT = 0;
      int defN = 0;
      recs = (int)file.GetFileSize() / sizeof(SB36);
      file.Seek(0,fsBegin);
      for(int i=0;i<recs;i++){
      	 if(file.Read(&B36, sizeof(SB36)) == sizeof(SB36)){
            if(B36.Type_.as_int() == 1){
               defN = B36.PvcID_.as_int();
               defT = B36.TermNo_.as_int();
               if(defT && defT != defN)
                  defN = defT;
            }else{
               TPvcRec PvcRec;
               PvcRec << B36;
               iter->second.insert(std::make_pair(B36.PvcID_, PvcRec));
            }
         }
      }
      for(TPvcList::iterator L = iter->second.begin(); L != iter->second.end(); L++){
         char tmp = L->second.TermNo_[3];
         if(((tmp >= 'D' && tmp <='F') && defN == 1) ||
            ((tmp >= 'M' && tmp <='O') && defN == 2)){
            TPvcList::updater u = iter->second.begin_update(L);
            u->Default_ = true;
            u->OrigDef_ = true;
            if(defT > 0 && defT != B36.PvcID_.as_int())
               u->Remark_.assign("明日起為預設FT-R!");
         }
      }
   }
   file.Close();
}
//---------------------------------------------------------------------------
K_mf(void) TBrkPvcVAC::ReadB37E(std::string& fname, const TBrokerID& brk, TMarket mkt)
{
   Kway::TFile file;
   SB37E     B37E;
   file.Open(fname.c_str(), TFileMode(Kway::fmRead));
   if(file.IsOpened() && file.GetFileSize() >= sizeof(SB37E)){
      file.Read(&B37E, sizeof(SB37E));
   } else
      return;
   SBrkMkt BrkMkt(brk,mkt);
   TBrkPvcList::iterator iter = Recs_->fetch(BrkMkt);
   if(B37E.BrokerID_.as_int() == 0 && B37E.PvcID_.as_int() == 0){              //全部變更成功時tse 回覆"00000000"
      for(TPvcList::iterator L = iter->second.begin(); L != iter->second.end(); L++){
         TPvcList::updater u   = iter->second.begin_update(L);
         if( !L->second.OrigPwd_.empty()){
            u->Remark_.assign("密碼變更完成!下次登入請使用新密碼!"); 
            u->OrigPwd_.clear();
         }  
         char tmp = L->second.TermNo_[3];
         if(((tmp >= 'D' && tmp <='F') || (tmp >= 'M' && tmp <='O')) && L->second.OrigDef_ != L->second.Default_){
            if(L->second.OrigDef_ == false){
               u->Remark_.assign("變更完成,明日起為預設FT-R!");
               u->Default_ = true;
            }else{
               u->Remark_.assign("變更完成,明日起取消預設FT-R!");
               u->Default_ = false;
            }
            u->OrigDef_ = L->second.Default_;
         }
      }
      return;
   }
   char buf[64];
   int recs;
   if(file.IsOpened()){
      recs = (int)file.GetFileSize() / sizeof(SB37E);
      file.Seek(0,fsBegin);
      for(int i=0;i<recs;i++)
         if(file.Read(&B37E, sizeof(SB37E)) == sizeof(SB37E)){
            if(B37E.Type_.as_int() == 1){
               for(TPvcList::iterator L = iter->second.begin(); L != iter->second.end(); L++){
                TPvcList::updater u   = iter->second.begin_update(L);
     	          char tmp = L->second.TermNo_[3];
      	          if(((tmp >= 'D' && tmp <='F') && B37E.PvcID_.as_int() == 1) ||
      	             ((tmp >= 'M' && tmp <='O') && B37E.PvcID_.as_int() == 2)){
                     switch(B37E.ErrCode_.as_int()){
                     case 0: u->Remark_.assign("密碼變更完成!下次登入請使用新密碼!"); break;
                     case 1: u->Remark_.assign("無該筆資料,更新失敗!請先向交易所要求重送配置表"); break;
                     case 2: u->Remark_.assign("舊密碼錯誤,更新失敗!請先向交易所要求重送配置表"); break;
                     case 3: u->Remark_.assign("不可輸入文字,更新失敗!舊密碼仍然有效."); break;
                     case 4: u->Remark_.assign("主要接收路徑值錯誤(01或02)，不可為空白."); break;
                     case 5: u->Remark_.assign("非二路證券商不可申請此項作業."); break;
                     }
                  }
      	          if((tmp >= 'D' && tmp <='F') || (tmp >= 'M' && tmp <='O'))
      	             u->Default_ = L->second.OrigDef_;
               }
            }else{
               TPvcList::iterator L = iter->second.fetch(B37E.PvcID_);
               TPvcList::updater u   = iter->second.begin_update(L);
               switch(B37E.ErrCode_.as_int()){
               case 0: u->Remark_.assign("密碼變更完成!下次登入請使用新密碼!"); break;
               case 1: u->Remark_.assign("無該筆資料,更新失敗!請先向交易所要求重送配置表"); break;
               case 2: u->Remark_.assign("舊密碼錯誤,更新失敗!請先向交易所要求重送配置表"); break;
               case 3: u->Remark_.assign("不可輸入文字,更新失敗!舊密碼仍然有效."); break;
               case 4: u->Remark_.assign("主要接收路徑值錯誤(01或02)，不可為空白."); break;
               case 5: u->Remark_.assign("非二路證券商不可申請此項作業."); break;
               }
               if(B37E.ErrCode_.as_int() > 0)
                  u->PassWord_ = L->second.OrigPwd_;
               u->OrigPwd_.clear();
            }
         }
      for(TPvcList::iterator L = iter->second.begin(); L!= iter->second.end(); L++){
         TPvcList::updater u   = iter->second.begin_update(L);
         if(!L->second.OrigPwd_.empty()){
            u->Remark_.assign("密碼變更完成!下次登入請使用新密碼!"); 
            u->OrigPwd_.clear();
         }  
         if(L->second.OrigDef_ != L->second.Default_){
            if(L->second.OrigDef_ == false){
               u->Remark_.assign("變更完成,明日起為預設FT-R!");
               u->Default_ = true;
            }else{
               u->Remark_.assign("變更完成,明日起取消預設FT-R!");
               u->Default_ = false;
            }
            u->OrigDef_ = L->second.Default_;
         }
      }
   }
   file.Close();
}
//---------------------------------------------------------------------------
