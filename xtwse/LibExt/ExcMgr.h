//---------------------------------------------------------------------------
#ifndef ExcMgrH
#define ExcMgrH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "ViewAreaMgr.h"
#include "ExcMgrBase.h"
#include "TimerThread.h"
#include "EvSesMgr.h"
//---------------------------------------------------------------------------
using namespace Kway;
//---------------------------------------------------------------------------
struct SEvent                                           //��2�h Value �� Structure
{
   TErrLevel                     EvLevel_;               //����
   TObjID                        ObjID_;                 //�ƥ�s�էO
   char_ary<char,120,' '>        EvMsg_;                 //�T��
   char_ary<char,120,' '>        Action_;                //�Ĩ����I
   char_ary<char, 39,' '>        Remark_;                //�Ƶ�
   bool                          Alert_;                 //�o�eĵ�ܤ��T�����x
};
//---------------------------------------------------------------------------
typedef TDMapValue<SEvent>                    TDEvent;  //��2�h�� Value
typedef TDMap<TErrKind, TErrNum, TDEvent>     TEvMapD;  //��2�h�� Map
typedef Kstl::ev_map<TErrKind, TEvMapD>       TEvMap;   //��1�h�� Map
typedef TEvMap::iterator                      TEvi;     //��1�h Map �� iterator
typedef TEvMap::updater                       TEvu;     //��1�h Map �� updater
typedef TEvMapD::iterator                     TEvDi;    //��2�h Map �� iterator
typedef TEvMapD::updater                      TEvDu;    //��2�h Map �� updater
//=============================================================================
//�o�@�q�O�B�z��ܳ���
//=============================================================================
K_class TErrMsgDFieldMaker                              //��2�h��������
{
private:
   TGroupDef*                      GDef_;
   TVAEditProp_GetKeys<TGroupDef>  Keys_;
public:
   K_ctor TErrMsgDFieldMaker(TGroupDef* gdef):GDef_(gdef),Keys_(*GDef_){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 7; }
   K_mf(std::string) Get(const TEvDi&, size_t) const;
   K_mf(void) Put(TEvMapD&, TEvDi&, size_t, const std::string&);
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TEvMapD,TErrMsgDFieldMaker> TErrMsgDViewer;    //��2�h��Viewer
//---------------------------------------------------------------------------
K_class TErrMsgDVACr:public Kway::TVACreater            //��2�h��VAC
{
 typedef Kway::TVACreater inherited;
 typedef TErrMsgDVACr     this_type;
private:
   TGroupDef*  GDef_;
public:
   K_ctor TErrMsgDVACr(TGroupDef* gdef):GDef_(gdef){};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.set(vco_Insert);             //�]�w�i�s�W
      vcs.set(vco_Erase);              //�]�w�i�R��
      TErrMsgDFieldMaker a(GDef_);
      return Kway::TaViewArea(new TErrMsgDViewer(0,a,vcs));
   }
};
//---------------------------------------------------------------------------
K_class TErrMsgVAC:public Kway::TVACreater              //��1�h��VAC
{
   typedef Kway::TVACreater        inherited;
   typedef TErrMsgVAC              this_type;
   TErrMsgDVACr                    DetailCreater_;

   K_class TErrMsgFieldMaker
   {
     typedef TEvMap                   TEvContainer;
     typedef TEvContainer::iterator   iterator;
     typedef size_t                   field_n_type;
     public:
       K_mf(const Kway::TFieldDesp&) GetFieldDesp(field_n_type n) const;
       inline K_mf(field_n_type) GetFieldCount() const {return 1;}
       K_mf(std::string) Get (const iterator& i, field_n_type fieldNo) const;
       K_mf(void) Put (TEvContainer& c,iterator& iter,field_n_type fieldNo,const std::string& str){};
   };
   TEvMap*     Recs_;           //��1�h�� Map

 public:
   K_ctor TErrMsgVAC(TEvMap& map,TGroupDef* gdef):Recs_(&map),DetailCreater_(gdef){}
   typedef Kway::TTViewArea<TEvMap,TErrMsgFieldMaker> TMsgViewer;
   K_mf(Kway::TaViewArea) CreateNewVA()
   {
      TViewControls   vcs;
      vcs.set(vco_Insert);
      vcs.set(vco_Erase);
      TErrMsgFieldMaker a;
      return Kway::TaViewArea(new TMsgViewer(Recs_, a, vcs));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
//�ƥ�O����
//---------------------------------------------------------------------------
struct SEvLog
{
   TKDateTimeMS                  Time_;
   TObjID                        ObjID_;                 //�ƥ�s�էO
   TErrCode                      EvCode_;                //�ƥ�N��
   TErrLevel                     EvLevel_;               //����
   char_ary<char, 20,' '>        SubSys_;                //�l�t��
   char_ary<char, 20,' '>        Object_;                //�ӷ�����
   char_ary<char,120,' '>        EvMsg_;                 //�T��
   char_ary<char,120,' '>        Action_;                //�Ĩ����I
   char_ary<char, 60,' '>        ExtMsg_;                //���[�T��
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<Kway::dword, SEvLog>       TEvLogs;
typedef TEvLogs::iterator                       TEvLogi;
typedef TStdRecorder<TEvLogs, SEvLog>           TLogRecorder;
//---------------------------------------------------------------------------
K_class TEvLogFieldMaker                              //��2�h��������
{
private:
public:
   K_ctor TEvLogFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 9; }
   K_mf(std::string) Get(const TEvLogi&, size_t) const;
   K_mf(void) Put(TEvLogs&, TEvLogi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
struct TEvKeyMaker
{
   inline K_mf(void) operator()(Kway::dword& akey, const char* ckey) const
                               {akey = atoi(ckey);}
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TEvLogs,TEvLogFieldMaker,TEvKeyMaker> TEvLogViewer;    
//---------------------------------------------------------------------------
K_class TEvLogVACr:public Kway::TVACreater            //��2�h��VAC
{
 typedef Kway::TVACreater inherited;
 typedef TEvLogVACr     this_type;
private:
   TEvLogs*             EvLogs_;
public:
   K_ctor TEvLogVACr(TEvLogs* evlog):EvLogs_(evlog){};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      return Kway::TaViewArea(new TEvLogViewer(EvLogs_, TEvLogFieldMaker(), vcs));
   }
};
//---------------------------------------------------------------------------
K_class TExcMgr : public TExcMgrBase
{
private:
   typedef TExcMgrBase                          inherited;
   typedef TExcMgr                              this_type;
   typedef TStdRecorder2d<TEvMap, TEvMapD>      TEvRecorder;    //���� 2d Map �� Reorder
   typedef Kway::TTimer<this_type>              TExcTimer;

   TEvMap*                         ErrMap_;
   TEvRecorder*                    Recorder_;
   TErrMsgVAC*                     ErrVA_;
   TErrCode                        HB_;
   TEvLogs*                        EvLogs_;
   bool                            IsAgent_;
   TEvLogVACr*                     EvLogVA_;
   TLogRecorder*                   EvLogRecorder_;
   TFile                           EvLogFile_;
   TEvSesMgr*                      EvSesMgr_;
//   TViewAreaMgr                    MainVAMgr_;
   TViewAreaMgr                    EvSesVAMgr_;
   TViewAreaMgr                    LocalVAMgr_;
   fixed_num<Kway::word>           Count_;
   TEvCenter*                      EvCenter_;
   TEvAlert                        EvAlert_;
   TExcTimer*                      Timer_;
   K_mf(void)           OnTimer (TTimerBase*);
   K_mf(std::string)    GetCount(){Count_++; return Count_.as_string();}
protected:
public:
   K_ctor               TExcMgr(bool isHB = false, bool doLog = true, bool agent = true);
   K_dtor               ~TExcMgr();
   K_mf(bool)           GetErrDef(TErrCode& ercode, TErrLevel& erlevel, TObjID& objid, std::string& ermsg, std::string& action, bool& alert);
   K_mf(void)           WriteEvLog(TObjID, TErrCode, TErrLevel, std::string&, std::string&, std::string&, std::string&);
//   K_mf(void)           Alert(const char* ec, std::string obj = "");
   K_mf(void)           Alert(const char* ec, std::string obj = "", std::string s = "");
   K_mf(void)           Alert(TErrCode ercode, std::string obj, std::string s = "");
   K_mf(void)           CmdReq(std::string& procname, std::string& objname, Kway::word cmdid, std::string msg="");  
};
//---------------------------------------------------------------------------
#endif
