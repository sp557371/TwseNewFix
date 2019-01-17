//---------------------------------------------------------------------------
#pragma hdrstop
#include "MsgCenter.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace Kstl;
using namespace Kway;
//---------------------------------------------------------------------------
static TMsgTable* MsgCenter_;
//---------------------------------------------------------------------------
K_fn(TMsgTable*) GetMsgCenter  ()
{
   return MsgCenter_;
}
//---------------------------------------------------------------------------
K_fn(std::string) GetErrMsg     (TMsgKey keya, TMsgKey keyb, const char* remark)
{
   bool ErrCnt = false;
   return GetErrMsg(keya, keyb, ErrCnt, remark);
}
//---------------------------------------------------------------------------
K_fn(std::string) GetErrMsg     (TMsgKey keya, TMsgKey keyb, bool& ErrCnt, const char* remark)
{
   if(MsgCenter_)
      return MsgCenter_->GetMsg(keya, keyb, ErrCnt, remark);
   return std::string(keya.as_string()+keyb.as_string()+"-�L�T����Ӫ�!");
}
//---------------------------------------------------------------------------
K_fn(std::string) GetErrMsg     (const char* key, const char* remark)
{
   bool ErrCnt = false;
   return GetErrMsg(key, ErrCnt, remark);
}
//---------------------------------------------------------------------------
K_fn(std::string) GetErrMsg     (const char* key, bool& ErrCnt, const char* remark)
{
   TMsgKey keya, keyb;
   keya.assign(key);
   keyb.assign(key+2);
   if(MsgCenter_)
      return MsgCenter_->GetMsg(keya, keyb, ErrCnt, remark);//.c_str();
   return std::string(keya.as_string()+keyb.as_string()+"-�L�T����Ӫ�!");//.c_str();
}
//---------------------------------------------------------------------------
K_fn(void) SetMsgCenter  (TMsgTable* m)
{
   MsgCenter_ = m;
}
//---------------------------------------------------------------------------
// VA section
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TMsgDFieldMaker::GetFieldDesp(size_t n) const
{
static Kway::TFieldDesp FdDsp[]=
{
  { "���A�N�X"    , 10, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null   , },
  { "�T������"    , 60, Kway::ta_xLeft_yCenter  ,  Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit, },
  { "�֭p���~����", 10, Kway::ta_xLeft_yCenter  ,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
};
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TMsgDFieldMaker::Get(const TMsgDi& i, size_t fieldNo) const
{
   char buf[16];
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
      case  1:  return i->second.Msg_.as_string();
      case  2:  return i->second.Error_?"Y":"";
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TMsgDFieldMaker::Put(TMsgMapD& c, TMsgDi& iter, size_t fieldNo, const std::string& str)
{
   if(fieldNo==0)
      return;
   TMsgDu u=c.begin_update(iter);
   switch(fieldNo)
   {
      case  1:  u->Msg_.assign(str);         break;
      case  2:  u->Error_ = (str[0]=='Y' || str[0] == 'y'); break;
   }
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&)  TMsgVAC::TMsgFieldMaker::GetFieldDesp  (field_n_type n) const
{
static Kway::TFieldDesp f[] = {
  { "�l�t�ΥN��",  12 , Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null, }
  };
return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TMsgVAC::TMsgFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
switch(fieldNo) {
  case 0:  switch(i->first.as_int()){
              case 10: return i->first.as_string()+"-TSE�s�u�l�t��";
              // 2013/12/17 miguel add
              case 12: return i->first.as_string()+"-TSE�s�u�l�t��";
              // end 
              case 20: return i->first.as_string()+"-TSE�ɮ׶ǿ�l�t��";
              case 30: return i->first.as_string()+"-TSE�@�����l�t��";
              case 31: return i->first.as_string()+"-TSE�Эɥ���l�t��";
              case 32: return i->first.as_string()+"-TSE�L�����l�t��";
              case 40: return i->first.as_string()+"-TSE�s�ѥ���l�t��";
              case 41: return i->first.as_string()+"-TSE���ʥ���l�t��";
              case 70: return i->first.as_string()+"-TSE������l�t��";
              case 91: return i->first.as_string()+"-OTC�s�u�l�t��";
              case 92: return i->first.as_string()+"-OTC�ɮ׶ǿ�l�t��";
              case 93: return i->first.as_string()+"-OTC�@�����l�t��";
              case 90: return i->first.as_string()+"-OTC�Эɥ���l�t��";
              case 98: return i->first.as_string()+"-OTC�L�����l�t��";
              case 94: return i->first.as_string()+"-OTC�s�ѥ���l�t��";
              case 97: return i->first.as_string()+"-OTC���ʥ���l�t��";
              default: return i->first.as_string();
           }
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(size_t) TMsgVAC::GetDetails(const char**& a)
{
   static const char* desp[] = {"�l�t��-�T���N�X"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TMsgVAC::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return &DetailCreater_;
     }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TMsgVAC::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TMsgViewer& va=static_cast<TMsgViewer&>(masterVA);
   const TMsgViewer::cursor_type *cur=va.GetCurRow();
   TMsgDViewer *DetailView =static_cast<TMsgDViewer*>(detailVA);
   if(cur->IsInvalid())
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(&(cur->GetIterator()->second));
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
// TMsgTable
//
//---------------------------------------------------------------------------
K_ctor TMsgTable::TMsgTable  (bool chgable)
{
   MsgMap_ = new TMsgMap;
   MsgVA_  = new TMsgVAC(MsgMap_);
   Recorder_ = new TMsgRecorder(MsgMap_, (char*)std::string(GetConfPath()+"MsgCenter.rec").c_str());
   Persist_ = chgable; 
   if(!chgable)
      delete Recorder_;
}
//---------------------------------------------------------------------------
K_dtor TMsgTable::~TMsgTable  ()
{
   if(Persist_)
      delete Recorder_;
   delete MsgVA_;
   delete MsgMap_;
}
//---------------------------------------------------------------------------
K_mf(std::string) TMsgTable::GetMsg(const char* key, bool& ErrCnt, const char* remark)
{
   TMsgKey keya, keyb;
   keya.assign(key);
   keyb.assign(key+2);
   return GetMsg(keya, keyb, ErrCnt, remark);
}
//---------------------------------------------------------------------------
K_mf(std::string) TMsgTable::GetMsg(TMsgKey keya, TMsgKey keyb, bool& ErrCnt, const char* remark)
{
   TMsgi     iter  = MsgMap_->find(keya);
   if(iter != MsgMap_->end()){
      TMsgMapD* mapd = ((TMsgMapD*)(&iter->second));
      TMsgDi    Diter = mapd->find(keyb);
      if(Diter != mapd->end()){
         ErrCnt = Diter->second.Error_;
         return std::string(keya.as_string()+keyb.as_string()+"-"+std::string(remark)+Diter->second.Msg_.as_string());
      }
   }
   ErrCnt = false;
   return std::string(keya.as_string()+keyb.as_string()+"-���w�q���T���N�X!");
}
//---------------------------------------------------------------------------
