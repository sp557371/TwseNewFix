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
   emlGreen , // N�@��T��
   emlGrey,    // Q�ˮ�
   emlYellow , // Wĵ�i
   emlRed   , // E�Y�����~
   emlNotDef = 99, // ���w�q
};
//---------------------------------------------------------------------------
//extern const char* LevUseTypeStr[3];                    //VA��ܥΪ�STRING
static const char* LevUseTypeStr[]=
{
   "N.���`",
   "Q.�ˮ�",
   "W.ĵ�i",
   "E.���~"
};
//---------------------------------------------------------------------------
K_class TErrLevel : public EnumT<EErrLevel>         //�B�z���Ū�CLASS
{
   typedef EnumT<EErrLevel> inherited;
public:
   inline K_ctor      TErrLevel(EErrLevel a = emlGreen)  : inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
enum EGroupKind{                //�P�@�s�ժ��ƥ󤤬O�Ҧ����T���o�e�ӷ����O���`�~��ܥ��`(�p:����ҳs�u),�٬O�u�n��
   egkAll,                      //�@�Өƥ�o�e�ӷ����`�Y�i(�p:�G�լۦP�����e�^�s�u),�b�o�بϥΤ@�Ӹs�����O���w�q�覡,
   egkOne,                      //�ѨϥΪ̿��
};
//---------------------------------------------------------------------------
static const char* GroupKindStr[]=
{
   "1.ALL/ALL",
   "2.ONE/ALL",
};
//---------------------------------------------------------------------------
K_class TGroupKind : public EnumT<EGroupKind>         //�B�z���Ū�CLASS
{
   typedef EnumT<EGroupKind> inherited;
public:
   inline K_ctor      TGroupKind(EGroupKind a = egkAll)  : inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
typedef char_ary<char,2,' '>             TErrKind; // ���~����    �p'G'�N��g/w �Ҳ��ͪ����~, 'X'��ܥ���t�β��ͪ����~..
typedef char_ary<char,4,' '>             TErrNum;  // ���~�N�X
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
// �o�ةw�q�t�Ϊ��A�����c�����(���x�s),�i�b���ݥX�ݸs�ժ����A�θs�դ��U�o�e�ӷ������A
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
struct SObjStat{                                   //�O���ثe�U���󪬺A�����c
   TErrLevel   ObjErrLevel_;
   TEvMsgMap*  MsgMap_;
   K_ctor SObjStat(){MsgMap_ = new TEvMsgMap();}
   K_dtor ~SObjStat(){delete MsgMap_;}
};
typedef Kstl::ev_map<TObjID, SObjStat>   TObjMap;  //�O���ثe�t�Ϊ��A����ƪ�
typedef TObjMap::iterator                TObjMapi;
typedef TObjMap::updater                 TObjMapu;
//---------------------------------------------------------------------------
K_class TMsgVACr:public Kway::TVACreater            //��2�h��VAC
{
   typedef Kway::TVACreater inherited;
   typedef TMsgVACr         this_type;
   K_class TMsgFieldMaker                              //��2�h��������
   {
   public:
      K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
      K_mf(size_t) GetFieldCount() const { return 4; }
      K_mf(std::string) Get(const TEvMsgMapi&, size_t) const;
      K_mf(void) Put(TEvMsgMap&, TEvMsgMapi&, size_t, const std::string&){};
   };
public:
   typedef Kway::TTViewArea<TEvMsgMap,TMsgFieldMaker> TMsgViewer;    //��2�h��Viewer
   K_ctor TMsgVACr(){};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.set(vco_Erase);
      TMsgFieldMaker a;
      return Kway::TaViewArea(new TMsgViewer(0,a,vcs));
   }
};
//---------------------------------------------------------------------------
K_class TObjVAC:public Kway::TVACreater              //��1�h��VAC
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
   TObjMap*     Recs_;           //��1�h�� Map

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
// �o�ةw�q�s�ղΦX�ƥ󪬺A�覡���x�s���c�����
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
      vcs.set(vco_Insert);             //�]�w�i�s�W
      vcs.set(vco_Erase);              //�]�w�i�R��
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
                              if(1){            // ������updater ���ͩR�g��, �ҥH��if(1) ���覡�ӳB�z
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
                                 ermsg = std::string("���w�q���ƥ�N�X!");
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
