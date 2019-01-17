//---------------------------------------------------------------------------
#ifndef ExcMgrBaseH
#define ExcMgrBaseH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "ViewAreaMgr.h"
#include "kutility.h"
#include "StdRecorder.h"
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
/////////////////////////////////////////////////////////////////////////////
using namespace Kway;
using namespace Kstl;
//---------------------------------------------------------------------------
class TExcMgrBase;
//---------------------------------------------------------------------------
enum EErrLevel{
   emlGreen , // N一般訊息
   emlGrey,    // Q檢核
   emlYellow , // W警告
   emlRed   , // E嚴重錯誤
   emlNotDef = 99, // 未定義
};
//---------------------------------------------------------------------------
//extern const char* LevUseTypeStr[3];                    //VA顯示用的STRING
static const char* LevUseTypeStr[]=
{
   "N.正常",
   "Q.檢核",
   "W.警告",
   "E.錯誤"
};
//---------------------------------------------------------------------------
K_class TErrLevel : public EnumT<EErrLevel>         //處理等級的CLASS
{
   typedef EnumT<EErrLevel> inherited;
public:
   inline K_ctor      TErrLevel(EErrLevel a = emlGreen)  : inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
enum EGroupKind{                //同一群組的事件中是所有的訊息發送來源都是正常才顯示正常(如:交易所連線),還是只要有
   egkAll,                      //一個事件發送來源正常即可(如:二組相同的成委回連線),在這裏使用一個群組類別的定義方式,
   egkOne,                      //供使用者選擇
};
//---------------------------------------------------------------------------
static const char* GroupKindStr[]=
{
   "1.ALL/ALL",
   "2.ONE/ALL",
};
//---------------------------------------------------------------------------
K_class TGroupKind : public EnumT<EGroupKind>         //處理等級的CLASS
{
   typedef EnumT<EGroupKind> inherited;
public:
   inline K_ctor      TGroupKind(EGroupKind a = egkAll)  : inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
typedef char_ary<char,2,' '>             TErrKind; // 錯誤種類    如'G'代表g/w 所產生的錯誤, 'X'表示交易系統產生的錯誤..
typedef char_ary<char,4,' '>             TErrNum;  // 錯誤代碼
//---------------------------------------------------------------------------
struct TErrCode{
   typedef TErrCode        this_type;
   TErrKind      ErrKind_;
   TErrNum       ErrNum_;
   // cooper {
   inline K_ctor TErrCode ()  {}
   inline K_ctor TErrCode (const TErrKind& kind)                     : ErrKind_(kind)                {}
   inline K_ctor TErrCode (const TErrKind& kind, const TErrNum& num) : ErrKind_(kind), ErrNum_(num)  {}
   // } cooper
   inline K_mf(std::string) as_string(){return ErrKind_.as_string()+ErrNum_.as_string();}
   inline K_mf(bool)  operator== (const this_type& a) const {return (ErrKind_ == a.ErrKind_ && ErrNum_ == a.ErrNum_);}
   inline K_mf(void)  assign(const char* a){ErrKind_.assign(a);ErrNum_.assign(a+2);}

};
//---------------------------------------------------------------------------
// 這裏定義系統狀態的結構及顯示(不儲存),可在此看出看群組的狀態及群組內各發送來源的狀態
//
//---------------------------------------------------------------------------
typedef char_ary<char,12,' '>            TObjID;
typedef char_ary<char,40,' '>            TEvMsgKey;
struct SMsgValue{
   TErrLevel    MsgErrLevel_;
   TErrCode     MsgErrCode_;
   Kstl::char_ary<char,120,' '> Msg_;
   TKDateTimeMS MsgTime_;
};
typedef Kstl::ev_map<TEvMsgKey, SMsgValue> TEvMsgMap;
typedef TEvMsgMap::iterator                  TEvMsgMapi;
typedef TEvMsgMap::updater                   TEvMsgMapu;
struct SObjStat{                                   //記錄目前各物件狀態的結構
   TErrLevel   ObjErrLevel_;
   TEvMsgMap*  MsgMap_;
   K_ctor SObjStat(){MsgMap_ = new TEvMsgMap();}
   K_dtor ~SObjStat(){delete MsgMap_;}
};
typedef Kstl::ev_map<TObjID, SObjStat>   TObjMap;  //記錄目前系統狀態的資料表
typedef TObjMap::iterator                TObjMapi;
typedef TObjMap::updater                 TObjMapu;
//---------------------------------------------------------------------------
K_class TMsgVACr:public Kway::TVACreater            //第2層的VAC
{
   typedef Kway::TVACreater inherited;
   typedef TMsgVACr         this_type;
   K_class TMsgFieldMaker                              //第2層的顯示欄位
   {
   public:
      K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
      K_mf(size_t) GetFieldCount() const { return 4; }
      K_mf(std::string) Get(const TEvMsgMapi&, size_t) const;
      K_mf(void) Put(TEvMsgMap&, TEvMsgMapi&, size_t, const std::string&){};
   };
public:
   typedef Kway::TTViewArea<TEvMsgMap,TMsgFieldMaker> TMsgViewer;    //第2層的Viewer
   K_ctor TMsgVACr(){};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.set(vco_Erase);
      TMsgFieldMaker a;
      return Kway::TaViewArea(new TMsgViewer(0,a,vcs));
   }
};
//---------------------------------------------------------------------------
K_class TObjVAC:public Kway::TVACreater              //第1層的VAC
{
   typedef Kway::TVACreater                         inherited;
   typedef TObjVAC                                  this_type;
   TMsgVACr                        DetailCreater_;
   K_class TObjFieldMaker
   {
     typedef TObjMap                  TEvContainer;
     typedef TEvContainer::iterator   iterator;
     typedef size_t                   field_n_type;
     public:
       K_mf(const Kway::TFieldDesp&) GetFieldDesp(field_n_type n) const;
       inline K_mf(field_n_type) GetFieldCount() const {return 2;}
       K_mf(std::string) Get (const iterator& i, field_n_type fieldNo) const;
       K_mf(void) Put (TEvContainer& c,iterator& iter,field_n_type fieldNo,const std::string& str){};
   };
   TObjMap*     Recs_;           //第1層的 Map

 public:
   typedef Kway::TTViewArea<TObjMap,TObjFieldMaker> TObjViewer;
   K_ctor TObjVAC(TObjMap* map):Recs_(map){};
   K_mf(Kway::TaViewArea) CreateNewVA()
   {
      TViewControls   vcs;
      vcs.set(vco_Erase);
      TObjFieldMaker a;
      return Kway::TaViewArea(new TObjViewer(Recs_,a,vcs));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
// 這裏定義群組統合事件狀態方式的儲存結構及顯示
//
//---------------------------------------------------------------------------
typedef char_ary<char,40,' '>            TGroupRemark;
struct  SGroupDef{
   TGroupKind   GroupKind_;
   TGroupRemark Remark_;
};
typedef Kstl::ev_map<TObjID, SGroupDef>  TGroupDef;
typedef TGroupDef::iterator              TGroupDefi;
typedef TStdRecorder<TGroupDef, SGroupDef> TGDRecorder;
//---------------------------------------------------------------------------
K_class TGroupDefFieldMaker
{
  typedef TGroupDef                TEvContainer;
  typedef TEvContainer::iterator   iterator;
  typedef size_t                   field_n_type;
private:
public:
  K_mf(const Kway::TFieldDesp&) GetFieldDesp(field_n_type n) const;
  inline K_mf(field_n_type) GetFieldCount() const {return 3;}
  K_mf(std::string) Get (const iterator& i, field_n_type fieldNo) const;
  K_mf(void) Put (TEvContainer& c,iterator& iter,field_n_type fieldNo,const std::string& str);
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TGroupDef,TGroupDefFieldMaker> TGroupDefViewer;
//---------------------------------------------------------------------------
K_class TGroupDefVACr:public Kway::TVACreater
{
 typedef Kway::TVACreater inherited;
 typedef TGroupDefVACr    this_type;
private:
   TGroupDef*    GroupDef_;
public:
   K_ctor TGroupDefVACr(TGroupDef* gd){GroupDef_ = gd;}
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.set(vco_Insert);             //設定可新增
      vcs.set(vco_Erase);              //設定可刪除
      TGroupDefFieldMaker a;
      return Kway::TaViewArea(new TGroupDefViewer(GroupDef_,a,vcs));
   }
};
//---------------------------------------------------------------------------
class TExcMgrObj
{
private:
protected:
public:
           K_ctor          TExcMgrObj(){};
           K_dtor          ~TExcMgrObj(){};
   virtual K_mf(bool)      DoAlert(TErrLevel, TErrCode, TErrLevel, std::string&, 
                                   std::string& obj, std::string& extmsg){return false;};
};
//---------------------------------------------------------------------------
typedef char_ary<char,20,' '> TCmdObjName;
class TCmdMgrObj
{
private:
protected:
   TCmdObjName ObjName_;
public:
           K_ctor          TCmdMgrObj(){ObjName_.clear();}
           K_dtor          ~TCmdMgrObj(){};
   virtual K_mf(bool)      DoCommand(Kway::word cmdid, std::string msg = ""){return false;};
           K_mf(std::string) GetObjName(){return ObjName_.as_string();}
           K_mf(void)      SetObjName(std::string oname){ObjName_.assign(oname);}
};
//---------------------------------------------------------------------------
class TExcMgrBase
{
   typedef std::vector<TExcMgrObj*>            TMgrObjs;
   typedef std::map<TCmdObjName, TCmdMgrObj*>  TCmdObjs;
private:
   TExcMgrObj*             ExcMgrObj_;
   TMgrObjs                MgrObjs_;
   TCmdObjs                CmdObjs_;
protected:
   TErrLevel               SysState_;
   TObjMap*                ObjMap_;
   TObjVAC*                ObjVA_;
   TGroupDef*              GroupDef_;
   TGroupDefVACr*          GroupDefVA_;
   TGDRecorder*            GDRecorder_;
   TViewAreaMgr            VAMgr_;
   bool                    DoLog_;
   virtual K_mf(TErrLevel) GetSysState(TErrCode& ercode, TErrLevel& erlevel, TObjID& objid, std::string& msg,std::string& s){
/*
fprintf(stderr,"step 0.0, this = %p, ObjMap = %p\n", this, ObjMap_);
//                              SObjStat* ObjStat = &(ObjMap_->fetch(objid)->second);
                              SObjStat* ObjStat;
fprintf(stderr,"step 0.1\n");
                              TObjMapi L = ObjMap_->find(objid);
fprintf(stderr,"step 0, map size = %d\n", ObjMap_->size());
                              if(L == ObjMap_->end()){
                                 ObjStat = new SObjStat();
fprintf(stderr,"step 1\n");
                                 ObjMap_->insert(TObjMap::value_type(objid, SObjStat()));
fprintf(stderr,"step 2\n");
                                 L = ObjMap_->find(objid);
fprintf(stderr,"step 3\n");
                              }
fprintf(stderr,"step 4\n");
                              ObjStat = &(L->second);
fprintf(stderr,"step 5\n");
                              TEvMsgKey MsgKey;
                              MsgKey.assign(s);
                              if(1){            // 為限制updater 的生命週期, 所以用if(1) 的方式來處理
                                 TEvMsgMapu u = ObjStat->MsgMap_->begin_update(ObjStat->MsgMap_->fetch(MsgKey));
                                 u->MsgErrLevel_ = erlevel;
                                 u->MsgErrCode_  = ercode;
                                 u->MsgTime_     = TKDateTimeMS::Now();
                                 u->Msg_.assign(msg);
                              }
                              SysState_ = erlevel;
                              TGroupKind gk = GetGroupKind(objid);
                              TErrLevel nErrLevel = erlevel;
                              for(TEvMsgMapi iter = ObjStat->MsgMap_->begin(); iter != ObjStat->MsgMap_->end(); iter++){
                                 if(gk == egkOne && iter->second.MsgErrLevel_ == emlGreen){
                                    nErrLevel = iter->second.MsgErrLevel_;
                                    break;
                                 }
                                 if(nErrLevel < iter->second.MsgErrLevel_ && iter->second.MsgErrLevel_ != emlNotDef)
                                    nErrLevel = iter->second.MsgErrLevel_;
                              }
                              if(nErrLevel != ObjStat->ObjErrLevel_){
                                 TObjMapu ObjMapu = ObjMap_->begin_update(ObjMap_->fetch(objid));
                                 ObjMapu->ObjErrLevel_ = nErrLevel;
                              }
                              for(TObjMapi iter = ObjMap_->begin(); iter != ObjMap_->end(); iter++)
                                 if(SysState_ < iter->second.ObjErrLevel_ && iter->second.ObjErrLevel_ != emlNotDef)
                                    SysState_ = iter->second.ObjErrLevel_;
*/
                              return SysState_;
                           }
   virtual K_mf(TGroupKind) GetGroupKind(TObjID& objid){
                              TGroupKind gk;
                              TGroupDefi iter = GroupDef_->find(objid);
                              if(iter != GroupDef_->end())
                                 gk = iter->second.GroupKind_;
                              return gk;
                           }
   virtual K_mf(void)      WriteEvLog(TObjID, TErrCode, TErrLevel, std::string&, std::string&, std::string&, std::string&){};
public:
           K_ctor          TExcMgrBase(){
                              MgrObjs_.clear();
                              CmdObjs_.clear();
                              ExcMgrObj_ = 0; 
                              ObjMap_    = new TObjMap();
                              GroupDef_  = new TGroupDef();
                           }
           K_dtor          ~TExcMgrBase(){
                              MgrObjs_.clear();
                              CmdObjs_.clear();
                              delete ObjMap_; 
                              delete GroupDef_;
                           }
           K_mf(void)      RegExcMgrObj(TExcMgrObj* obj){
                              if(MgrObjs_.size() > 0)
                                 for(TMgrObjs::iterator L = MgrObjs_.begin(); L != MgrObjs_.end(); L++){
                                    if((*L) == obj)
                                       return;
                                 }
                              MgrObjs_.push_back(obj);
                           }
           K_mf(void)      DelExcMgrObj(TExcMgrObj* obj){
                              if(MgrObjs_.size() > 0)
                                 for(TMgrObjs::iterator L = MgrObjs_.begin(); L != MgrObjs_.end(); L++){
                                    if((*L) == obj){
                                       MgrObjs_.erase(L);
                                       return;
                                    }
                                 }
                           }
           K_mf(void)      RegCmdMgrObj(TCmdMgrObj* obj, std::string oname){
                              obj->SetObjName(oname);
                              TCmdObjName objname(obj->GetObjName());
                              TCmdObjs::iterator L = CmdObjs_.find(objname);
                              if(L != CmdObjs_.end())
                                 return;
                              CmdObjs_.insert(std::make_pair(objname,obj));
                           }
           K_mf(void)      DelCmdMgrObj(TCmdMgrObj* obj){
                              TCmdObjName objname(obj->GetObjName());
                              TCmdObjs::iterator L = CmdObjs_.find(objname);
                              if(L == CmdObjs_.end())
                                 return;
                              CmdObjs_.erase(L);
                           }
/*
   virtual K_mf(void)      Alert(const char* ec, std::string obj = ""){
                              TErrCode ercode;
                              ercode.ErrKind_.assign(ec);
                              ercode.ErrNum_.assign(ec+2);
                              Alert(ercode, obj);
                           }
*/
   virtual K_mf(void)      Alert(const char* ec, std::string obj = "", std::string s = ""){
                              TErrCode ercode;
                              ercode.ErrKind_.assign(ec);
                              ercode.ErrNum_.assign(ec+2);
                              Alert(ercode, obj, s);
                           }
   virtual K_mf(void)      Alert(TErrCode ercode, std::string obj, std::string s = ""){
                              TErrLevel erlevel;
                              TObjID    objid;
                              bool      alert;
                              TErrLevel sysstat;
                              std::string ermsg, action;
                              if(!GetErrDef(ercode, erlevel, objid, ermsg, action, alert)){
                                 erlevel = emlNotDef;
                                 ermsg = std::string("未定義的事件代碼!");
                              }
                              if(ercode.as_string() == "000000")     // alive message
                                 sysstat = SysState_;
                              else
                                 sysstat = GetSysState(ercode, erlevel, objid, ermsg, obj);
                              if(DoLog_)
                                 WriteEvLog(objid, ercode, erlevel, ermsg, action, obj, s);
                              if(MgrObjs_.size() > 0)
                                 for(TMgrObjs::iterator L = MgrObjs_.begin(); L != MgrObjs_.end(); L++)
                                    if(*L)
                                       (*L)->DoAlert(sysstat, ercode, erlevel, ermsg, obj, s);
                           }
   virtual K_mf(void)      CmdReq(std::string& procname, std::string& objname, Kway::word cmdid, std::string msg=""){};  
           K_mf(void)      DoCmd(std::string objn, Kway::word cmdid, std::string msg=""){
                              TCmdObjName objname(objn);
                              TCmdObjs::iterator L = CmdObjs_.find(objname);
                              if(L == CmdObjs_.end())
                                 return;
                              L->second->DoCommand(cmdid, msg);
                           }
           K_mf(TViewAreaMgr*) GetVAMgr() { return &VAMgr_;}
   virtual K_mf(bool)      GetErrDef(TErrCode& ercode, TErrLevel& erlevel, TObjID& objid, std::string& ermsg, std::string& action, bool& alert){return false;}
};
/////////////////////////////////////////////////////////////////////////////
K_fn(TExcMgrBase*) GetExcMgr();
K_fn(void) SetExcMgr(TExcMgrBase* ExcMgr);
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop,1)
#else
   #pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
#endif
