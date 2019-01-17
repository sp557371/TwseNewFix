//---------------------------------------------------------------------------
#ifndef PvcTableH
#define PvcTableH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "ViewAreaMgr.h"
#include "StdRecorder.h"
#include "TwStkOrder.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kstl;
//---------------------------------------------------------------------------
typedef char_ary<char,40,' '> TDesp;
//---------------------------------------------------------------------------
enum efType {
   efID,      // pvc id
   efApCode,  // 業務代號
   efPassword,// 密碼
   efTermNo,  // 終端機代號
   efDefault, // 主要檔案接收管道,FT專用
   efNone,
};
//---------------------------------------------------------------------------
typedef Kstl::char_ary<char, 2, ' '>   TPvcID;
typedef Kstl::char_ary<char, 1, ' '>   TApCode;
typedef Kstl::char_ary<char, 4, ' '>   TPwd;
typedef Kstl::char_ary<char, 7, ' '>   TTermNo;
typedef Kstl::char_ary<char, 2, ' '>   TB36Type;
//---------------------------------------------------------------------------
struct SB36{
   char_ary<char, 4,' '> BrokerID_;
   TB36Type              Type_;
   TPvcID                PvcID_;
   TTermNo               TermNo_;
   TPwd                  PassWord_;
   TApCode               ApCode_;
//   char_ary<char, 2,' '> Filler_;
};
//---------------------------------------------------------------------------
struct SB37{
   char_ary<char, 4,' '> BrokerID_;
   TB36Type              Type_;
   TPvcID                PvcID_;
   TPwd                  OrigPwd_;
   TPwd                  NewPwd_;
};
//---------------------------------------------------------------------------
struct SB37E{
   char_ary<char, 4,' '> BrokerID_;
   TB36Type              Type_;
   TPvcID                PvcID_;
   char_ary<char, 2,' '> ErrCode_;
};
//---------------------------------------------------------------------------
struct SBrkMkt{
   TBrokerID    BrokerID_;	
   TMarket      Market_;
   SBrkMkt() {};
   SBrkMkt(const TBrokerID& a, const TMarket& b)
      : BrokerID_(a), Market_(b)
      {}
   K_mf(bool) operator < (const SBrkMkt& a) const
      { return BrokerID_==a.BrokerID_ ? Market_<a.Market_ : BrokerID_<a.BrokerID_; }
   K_mf(std::string) as_string()const {return std::string(BrokerID_.as_string()+"-"+Market_.as_string());}
   K_mf(void)        assign(const char*){};
};
//---------------------------------------------------------------------------
struct SPvcList {
   TApCode        ApCode_;
   TPwd           PassWord_;
   TPwd           OrigPwd_;
   TTermNo        TermNo_;
   bool           Default_;
   bool           OrigDef_;
   char_ary<char,58,' '> Remark_;
   K_mf(void) operator << (const SB36& a) {   ApCode_   = a.ApCode_;
                                              PassWord_ = a.PassWord_;
                                              TermNo_   = a.TermNo_;
                                              Default_  = false;
                                              OrigDef_  = false;
                                          }
};
//---------------------------------------------------------------------------
typedef TDMapValue<SPvcList>                  TPvcRec;       //第2層的 Value
typedef TDMap<SBrkMkt, TPvcID, TPvcRec>       TPvcList;      //第2層的 Map
typedef Kstl::ev_map<SBrkMkt, TPvcList>       TBrkPvcList;   //第1層的 Map
typedef TStdRecorder2d<TBrkPvcList, TPvcList> TPvcRecorder;
//---------------------------------------------------------------------------
struct TPvcFieldMaker
{
   typedef TPvcList                 TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;

   K_mf(const Kway::TFieldDesp&)  GetFieldDesp  (field_n_type n) const
          {
            static Kway::TFieldDesp f[] = {
                     { "PVC 代號"    , 10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Fixed,  },
                     { "業務種類"    ,  8, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Null,},
                     { "密碼"        ,  8, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,   },
                     { "終端機代號"  , 10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Null,},
                     { "主要FT-R管道", 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,},
                     { "備註"        , 40, Kway::ta_xCenter_yCenter, Kway::ta_xLeft_yCenter,   Kway::tfs_Null,}};
            return f[n];
          }
   inline K_mf(field_n_type)  GetFieldCount () const    { return 6; }

   K_mf(std::string)  Get  (const iterator& i, field_n_type fieldNo) const
         {
            char  buf[128];
            switch(fieldNo) {
            case 0:  return i->first.as_string();
            case 1:  switch(i->second.ApCode_.as_int()){
            	        case 0: return std::string("委託");
            	        case 1: {char tmp = i->second.TermNo_[3];
            	                if(tmp >= 'A' && tmp <='C') return std::string("FT-Send");
            	                if(tmp >= 'D' && tmp <='F') return std::string("FT-Recv"); 
            	                if(tmp >= 'J' && tmp <='L') return std::string("FT-Send-2");
            	                if(tmp >= 'M' && tmp <='O') return std::string("FT-Recv-2"); break;}
            	        case 3: return std::string("成交回報");
            	        default: break;
            	     }
            	     break;
            case 2:  return i->second.PassWord_.as_string();
            case 3:  return i->second.TermNo_.as_string();
            case 4:  {char tmp = i->second.TermNo_[3];
            	        return ((tmp >= 'D' && tmp <='F')||(tmp >= 'M' && tmp <='O'))?(i->second.Default_?"Y":"N"):"";}
            case 5:  return i->second.Remark_.as_string();
            }
            return std::string();
         }
   K_mf(void)  Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str)
         {
            TEvContainer::updater   u = c.begin_update(iter);
            switch(fieldNo){
            case 2: u->PassWord_.assign(str); return;
            case 4: {char tmp = iter->second.TermNo_[3];
            	       if((tmp >= 'D' && tmp <='F')||(tmp >= 'M' && tmp <='O')) 
            	          if(str[0] == 'y' || str[0] == 'Y')
            	             u->Default_ = true;
            	          else 
            	             u->Default_ = false;
            	       return;}
            case 5: u->Remark_.assign(str);   return;
            }
         }
}; // class TSampleFieldMaker
/////////////////////////////////////////////////////////////////////////////
class TBrkPvcVAC;
K_class TPvcVA :public TTViewArea<TPvcList, TPvcFieldMaker>
{
   typedef TTViewArea<TPvcList, TPvcFieldMaker> inherited;
   typedef TBrkPvcVAC   T;
   typedef K_mf(bool)(T::*TSettingApply)(TPvcList*,TPvcList*);
private:
   T*             Obj_;
   TPvcList       Shadow_;
   TPvcList*      Origin_;
   TSettingApply  Apply_;
   TPvcFieldMaker FMaker_;
   K_mf(bool)     SettingApply()
   {
      if (Obj_ && Apply_)
         return (Obj_->*Apply_)(Origin_, &Shadow_);
      return false;
   }
   K_mf(bool)    SettingCancel()
   {
      Shadow_ = *Origin_;
      return true;
   }
public:
   K_ctor TPvcVA(TViewControls vcs)
      :inherited(0,FMaker_,vcs)
   {
   }
   K_mf(void)  SetContainer  (T* obj, TSettingApply ap, TPvcList* c){
      inherited::SetContainer(&Shadow_);
      if(c == 0)
         return;
      Obj_   = obj;
      Apply_ = ap;
      Origin_ = c;
      Shadow_ = *c;
   }
};
//---------------------------------------------------------------------------
K_class TPvcVACr:public Kway::TVACreater            //第2層的VAC
{
 typedef Kway::TVACreater inherited;
 typedef TPvcVACr     this_type;
private:
public:
   K_ctor TPvcVACr(){};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.set(vco_Setting);             //設定
      return Kway::TaViewArea(new TPvcVA(vcs));
   }
};
//---------------------------------------------------------------------------
K_class TBrkPvcVAC:public Kway::TVACreater              //第1層的VAC
{
   typedef Kway::TVACreater        inherited;
   typedef TBrkPvcVAC              this_type;
   TPvcVACr                        DetailCreater_;

   K_class TBrkPvcFieldMaker
   {
     typedef TBrkPvcList              TEvContainer;
     typedef TEvContainer::iterator   iterator;
     typedef size_t                   field_n_type;
     public:
       K_mf(const Kway::TFieldDesp&) GetFieldDesp(field_n_type n) const;
       inline K_mf(field_n_type) GetFieldCount() const {return 1;}
       K_mf(std::string) Get (const iterator& i, field_n_type fieldNo) const;
       K_mf(void) Put (TEvContainer& c,iterator& iter,field_n_type fieldNo,const std::string& str){};
   };
   TBrkPvcList*     Recs_;           //第1層的 Map
   TPvcRecorder*    Recorder_;

public:
   K_ctor TBrkPvcVAC();
   K_dtor ~TBrkPvcVAC();
   typedef Kway::TTViewArea<TBrkPvcList,TBrkPvcFieldMaker> TBrkPvcViewer;
   K_mf(Kway::TaViewArea) CreateNewVA()
   {
      TViewControls   vcs;
      vcs.set(vco_Erase);
      TBrkPvcFieldMaker a;
      return Kway::TaViewArea(new TBrkPvcViewer(Recs_, a, vcs));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
   K_mf(bool)              SettingApply(TPvcList* orig, TPvcList* shadow);
   K_mf(void)              ReadB36(std::string& fname, const TBrokerID& brk, TMarket mkt);
   K_mf(void)              ReadB37E(std::string& fname, const TBrokerID& brk, TMarket mkt);
};
//---------------------------------------------------------------------------
#endif
