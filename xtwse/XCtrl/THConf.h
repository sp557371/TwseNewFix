//---------------------------------------------------------------------------
#ifndef THConfH
#define THConfH
//---------------------------------------------------------------------------
#include "ExtDef.h"
#include "ViewAreaMgr.h"
#include "ExcMgrBase.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Twse{
using namespace Kstl;
//---------------------------------------------------------------------------
typedef char_ary<char, 2,' '> TSocketID;
typedef char_ary<char, 2,' '> TSubSys;
typedef char_ary<char, 4,' '> TPasswd;
typedef char_ary<char, 5,' '> TSrcPort;
typedef char_ary<char,10,' '> TFixSesID;
typedef char_ary<char, 4,' '> TBrkID;
typedef char_ary<char,10,' '> TTermID;
//---------------------------------------------------------------------------
struct TPvcConf
{
   TPasswd                       Passwd_;
   TSrcPort                      SrcPort_;
   bool                          Active_;
   TTermID                       TermID_;
   char_ary<char, 40,' '>        Remark_;                //附加訊息
   K_ctor TPvcConf(){Active_ = true;}
   K_mf(bool) operator== (const TPvcConf& a) const{
      return (Passwd_ == a.Passwd_ && SrcPort_ == a.SrcPort_ && TermID_ == a.TermID_);
   }
   K_mf(bool) operator!= (const TPvcConf& a) const{
   return (Passwd_ != a.Passwd_ || SrcPort_ != a.SrcPort_ || TermID_ != a.TermID_);
   }
};
typedef Kstl::ev_map<TSocketID, TPvcConf>      TPvcConfs;
typedef TPvcConfs::iterator                    TPvcConfsi;
//---------------------------------------------------------------------------
K_class TPvcConfFieldMaker                           
{
private:
public:
   K_ctor TPvcConfFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 6; }
   K_mf(std::string) Get(const TPvcConfsi&, size_t) const;
   K_mf(void) Put(TPvcConfs&, TPvcConfsi&, size_t, const std::string&);
};
//---------------------------------------------------------------------------
K_class TPvcConfViewer : public Kway::TTViewArea<TPvcConfs,TPvcConfFieldMaker>
{
   typedef Kway::TTViewArea<TPvcConfs,TPvcConfFieldMaker> inherited;
private:
   TPvcConfs        Shadow_;
   TPvcConfs*       Origin_;
   TFixSesID        FixSesID_;
public:
   K_ctor TPvcConfViewer(TPvcConfs* c, TViewControls vcs)
      :Origin_(c),Shadow_(*c), inherited(c,TPvcConfFieldMaker(),vcs)
   { }
   K_dtor ~TPvcConfViewer()
   { SettingCancel(); }
   K_mf(void) SetContainer(TPvcConfs* c, TFixSesID mid)
   {
      Origin_   = c;
      Shadow_   = *c;
      FixSesID_ = mid;
      inherited::SetContainer(&Shadow_);
   }
   K_mf(bool) SettingCancel()
   {
      Shadow_ = *Origin_;
      return true;
   }
   K_mf(bool) SettingApply();
};
//---------------------------------------------------------------------------
K_class TPvcConfVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TPvcConfVACr     this_type;
private:
   TPvcConfs*             PvcConfs_;
public:
   K_ctor TPvcConfVACr(){PvcConfs_ = new TPvcConfs();};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.set(vco_Setting);
      vcs.set(vco_Insert);
      vcs.set(vco_Erase);
      vcs.set(vco_CopyCurrent);
      vcs.set(vco_DataExport);
      if(PvcConfs_ == 0)
         return Kway::TaViewArea(new TPvcConfViewer(0, vcs));
      else
         return Kway::TaViewArea(new TPvcConfViewer(PvcConfs_, vcs));
   }
   K_mf(void) SetContainer(TPvcConfs* evmp){PvcConfs_ = evmp;}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct TMWConf
{
   std::string                   Conf_;
   char_ary<char, 40,' '>        Remark_;                //附加訊息
};
//---------------------------------------------------------------------------
enum EMWItem{
   emw_Passwd = 0,
   emw_DstPort,
   emw_End,
};
//---------------------------------------------------------------------------
class TMWConfs: public ev_map<fixed_num<Kway::word>, TMWConf>
{
public:
   K_ctor TMWConfs(){
      for(int i = 0; i < emw_End; i++)
         fetch(i);
   }
   K_mf(bool) SetValue(EMWItem i, std::string& val){
      iterator L = fetch(i);
      updater  u = begin_update(L);
      u->Conf_ = val;
   }
};
typedef TMWConfs::iterator                      TMWConfsi;
//---------------------------------------------------------------------------
K_class TMWConfFieldMaker                           
{
private:
public:
   K_ctor TMWConfFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 4; }
   K_mf(std::string) Get(const TMWConfsi&, size_t) const;
   K_mf(void) Put(TMWConfs&, TMWConfsi&, size_t, const std::string&);
};
//---------------------------------------------------------------------------
K_class TMWConfViewer : public Kway::TTViewArea<TMWConfs,TMWConfFieldMaker>
{
   typedef Kway::TTViewArea<TMWConfs,TMWConfFieldMaker> inherited;
private:
   TMWConfs        Shadow_;
   TMWConfs*       Origin_;
   TFixSesID       FixSesID_;
public:
   K_ctor TMWConfViewer(TMWConfs* c, TViewControls vcs)
      :Origin_(c),Shadow_(*c), inherited(c,TMWConfFieldMaker(),vcs)
   { }
   K_dtor ~TMWConfViewer()
   { SettingCancel(); }
   K_mf(void) SetContainer(TMWConfs* c, TFixSesID mid)
   {
      Origin_   = c;
      Shadow_   = *c;
      FixSesID_ = mid;
      inherited::SetContainer(&Shadow_);
   }
   K_mf(bool) SettingCancel()
   {
      Shadow_ = *Origin_;
      return true;
   }
   K_mf(bool) SettingApply();
};
//---------------------------------------------------------------------------
K_class TMWConfVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TMWConfVACr      this_type;
private:
   TMWConfs*             MWConfs_;
public:
   K_ctor TMWConfVACr(){MWConfs_ = new TMWConfs();};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.set(vco_Setting);
      if(MWConfs_ == 0)
         return Kway::TaViewArea(new TMWConfViewer(0,vcs));
      else
         return Kway::TaViewArea(new TMWConfViewer(MWConfs_, vcs));
   }
   K_mf(void) SetContainer(TMWConfs* evmp){MWConfs_ = evmp;}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct TTmpConf{
   TPvcConfs   PvcConf_;
   TMWConfs    MWConf_;
};
typedef Kstl::ev_map<TFixSesID, TTmpConf>   TTmpConfs;
typedef TTmpConfs::iterator                 TTmpConfsi;
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TBrkID, TTmpConfs>     TBrkConf;
typedef TBrkConf::iterator                  TBrkConfi;
//---------------------------------------------------------------------------
K_class TFixIDFieldMaker                        //FIX ID層的顯示欄位
{
private:
public:
   K_ctor TFixIDFieldMaker(){
   };
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 1; }
   K_mf(std::string) Get(const TTmpConfsi&, size_t) const;
   K_mf(void) Put(TTmpConfs&, TTmpConfsi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TTmpConfs,TFixIDFieldMaker> TTmpConfViewer;    
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_class TTmpConfVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TTmpConfVACr       this_type;
private:
   class TTmpConEvHandler : public container_ev_handler{
   private:
   public:
      K_ctor TTmpConEvHandler(){};   
      K_mf(void) OnContainerBfErase(const ev_container& c, const ev_iterator& i, size_type);
   };
   TTmpConfs*              TmpConfs_;
   TMWConfVACr             MWConfVACr_;
   TPvcConfVACr            PvcConfVACr_;
   TFixIDFieldMaker        FMaker_;
   TFixSesID               CurFixSesID_;
   TTmpConEvHandler*       EvHandler_;
public:
   K_ctor TTmpConfVACr(){
      TmpConfs_ = 0;
      CurFixSesID_ = TFixSesID();
      EvHandler_   = new TTmpConEvHandler();
   };
   K_mf(Kway::TaViewArea) CreateNewVA(){

      TViewControls   vcs;
      vcs.set(vco_Erase);
      vcs.set(vco_Insert);
//      vcs.reset();
//      if(TmpConfs_ == 0)
//         return Kway::TaViewArea(new TTmpConfViewer(0));
//      else
         return Kway::TaViewArea(new TTmpConfViewer(TmpConfs_, FMaker_, vcs));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
   K_mf(void) SetContainer(TTmpConfs* evmp){
      TmpConfs_ = evmp;
      evmp->add_ev_handler((container_ev_handler*)EvHandler_);
   }
};
//---------------------------------------------------------------------------
K_class TBrkConfFieldMaker                              //Broker層的顯示欄位
{
private:
public:
   K_ctor TBrkConfFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 1; }
   K_mf(std::string) Get(const TBrkConfi&, size_t) const;
   K_mf(void) Put(TBrkConf&, TBrkConfi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TBrkConf,TBrkConfFieldMaker> TBrkConfViewer;    
//---------------------------------------------------------------------------
K_class TBrkConfVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TBrkConfVACr       this_type;
private:
   class TBrkConEvHandler : public container_ev_handler{
   private:
   public:
      K_ctor TBrkConEvHandler(){};   
      K_mf(void) OnContainerBfErase(const ev_container& c, const ev_iterator& i, size_type);
   };
   TBrkConf*              BrkConf_;
   TTmpConfVACr*          DetailCreater_;
   TBrkConEvHandler*      EvHandler_;
public:
   K_ctor TBrkConfVACr(TBrkConf* bc){
      BrkConf_ = bc; 
      DetailCreater_ = new TTmpConfVACr();
      EvHandler_=new TBrkConEvHandler();
      BrkConf_->add_ev_handler((container_ev_handler*)EvHandler_);
   };
   K_mf(Kway::TaViewArea) CreateNewVA();
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//   ErrCntMgr
/////////////////////////////////////////////////////////////////////////////

struct TErrCnt
{
   Kway::word                 ErrCnt_;
   Kway::word                 ErrLimit_;
   char_ary<char, 40,' '>     Remark_;                //附加訊息
   K_ctor TErrCnt(){};
   K_mf(bool) operator== (const TErrCnt& a) const{
      return (ErrCnt_ == a.ErrCnt_);
   }
   K_mf(bool) operator!= (const TErrCnt& a) const{
   return (ErrCnt_ != a.ErrCnt_);
   }
};
typedef Kstl::ev_map<TSubSys, TErrCnt>       TSubSysErrCnt;
typedef TSubSysErrCnt::iterator              TSubSysErrCnti;
//---------------------------------------------------------------------------
K_class TErrCntFieldMaker                           
{
private:
public:
   K_ctor TErrCntFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 4; }
   K_mf(std::string) Get(const TSubSysErrCnti&, size_t) const;
   K_mf(void) Put(TSubSysErrCnt&, TSubSysErrCnti&, size_t, const std::string&);
};
//---------------------------------------------------------------------------
K_class TErrCntViewer : public Kway::TTViewArea<TSubSysErrCnt,TErrCntFieldMaker>
{
   typedef Kway::TTViewArea<TSubSysErrCnt,TErrCntFieldMaker> inherited;
private:
   TSubSysErrCnt    Shadow_;
   TSubSysErrCnt*   Origin_;
   TFixSesID        FixSesID_;
public:
   K_ctor TErrCntViewer(TSubSysErrCnt* c, TViewControls vcs)
      :Origin_(c),Shadow_(*c), inherited(c,TErrCntFieldMaker(),vcs)
   { }
   K_dtor ~TErrCntViewer()
   { SettingCancel(); }
   K_mf(void) SetContainer(TSubSysErrCnt* c, TFixSesID mid)
   {
      Origin_   = c;
      Shadow_   = *c;
      FixSesID_ = mid;
      inherited::SetContainer(&Shadow_);
   }
   K_mf(bool) SettingCancel()
   {
      Shadow_ = *Origin_;
      return true;
   }
   K_mf(bool) SettingApply();
};
//---------------------------------------------------------------------------
K_class TErrCntVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TErrCntVACr      this_type;
private:
   TSubSysErrCnt*         SubSysErrCnt_;
public:
   K_ctor TErrCntVACr(){SubSysErrCnt_ = new TSubSysErrCnt();};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.set(vco_Setting);
      vcs.set(vco_Insert);
      vcs.set(vco_Erase);
      vcs.set(vco_CopyCurrent);
      vcs.set(vco_DataExport);
      if(SubSysErrCnt_ == 0)
         return Kway::TaViewArea(new TErrCntViewer(0, vcs));
      else
         return Kway::TaViewArea(new TErrCntViewer(SubSysErrCnt_, vcs));
   }
   K_mf(void) SetContainer(TSubSysErrCnt* evmp){SubSysErrCnt_ = evmp;}
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TFixSesID, TSubSysErrCnt>   TTmpErrCnts;
typedef TTmpErrCnts::iterator                    TTmpErrCntsi;
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TBrkID, TTmpErrCnts>     TBrkErrCnt;
typedef TBrkErrCnt::iterator                  TBrkErrCnti;
//---------------------------------------------------------------------------
K_class TFixIDErrCntFieldMaker                        //FIX ID層的顯示欄位
{
private:
public:
   K_ctor TFixIDErrCntFieldMaker(){
   };
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 1; }
   K_mf(std::string) Get(const TTmpErrCntsi&, size_t) const;
   K_mf(void) Put(TTmpErrCnts&, TTmpErrCntsi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TTmpErrCnts,TFixIDErrCntFieldMaker> TTmpErrCntViewer;    
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_class TTmpErrCntVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater     inherited;
 typedef TTmpErrCntVACr       this_type;
private:
   TTmpErrCnts*            TmpErrCnts_;
   TErrCntVACr             ErrCntVACr_;
   TFixIDErrCntFieldMaker  FMaker_;
   TFixSesID               CurFixSesID_;
public:
   K_ctor TTmpErrCntVACr(){
      TmpErrCnts_  = 0;
      CurFixSesID_ = TFixSesID();
   };
   K_mf(Kway::TaViewArea) CreateNewVA(){

      TViewControls   vcs;
      vcs.set(vco_Erase);
      vcs.set(vco_Insert);
//      vcs.reset();
//      if(TmpConfs_ == 0)
//         return Kway::TaViewArea(new TTmpConfViewer(0));
//      else
         return Kway::TaViewArea(new TTmpErrCntViewer(TmpErrCnts_, FMaker_, vcs));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
   K_mf(void) SetContainer(TTmpErrCnts* evmp){
      TmpErrCnts_ = evmp;
   }
};
//---------------------------------------------------------------------------
K_class TBrkErrCntFieldMaker                              //Broker層的顯示欄位
{
private:
public:
   K_ctor TBrkErrCntFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 1; }
   K_mf(std::string) Get(const TBrkErrCnti&, size_t) const;
   K_mf(void) Put(TBrkErrCnt&, TBrkErrCnti&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TBrkErrCnt,TBrkErrCntFieldMaker> TBrkErrCntViewer;    
//---------------------------------------------------------------------------
K_class TBrkErrCntVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TBrkErrCntVACr       this_type;
private:
   TBrkErrCnt*           BrkErrCnt_;
   TTmpErrCntVACr*       DetailCreater_;
public:
   K_ctor TBrkErrCntVACr(TBrkErrCnt* bc){
      BrkErrCnt_ = bc; 
      DetailCreater_ = new TTmpErrCntVACr();
   };
   K_mf(Kway::TaViewArea) CreateNewVA();
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
K_class TBrkConfPage : public TCmdMgrObj
{
private:
   TBrkConfVACr*    BrkConfVACr_;
   TBrkConf*        BrkConf_;
   TBrkErrCntVACr*  BrkErrCntVACr_;
   TBrkErrCnt*      BrkErrCnt_;
protected:
public:
   K_ctor          TBrkConfPage();
   K_dtor          ~TBrkConfPage();
   K_mf(TBrkConfVACr*) GetVA(){return BrkConfVACr_;}
   K_mf(TBrkErrCntVACr*) GetErrCntVA(){return BrkErrCntVACr_;}
   K_mf(bool)            DoCommand(Kway::word cmdid, std::string msg = "");
};
//---------------------------------------------------------------------------
};//namespace Twse
};//namespace Kway
#endif
