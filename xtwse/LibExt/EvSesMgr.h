//---------------------------------------------------------------------------
#ifndef EvSesMgrH
#define EvSesMgrH
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "KFile.h"
#include "ExcMgrBase.h"
#include "PacketSes.h"
#include "TimerThread.h"
#include "OpenApMsg.h"
//---------------------------------------------------------------------------
namespace Kway {
using namespace Kstl;
//---------------------------------------------------------------------------
#define HBTime 30
//---------------------------------------------------------------------------
enum  EFuncCode{
   ef_Login = 1,
   ef_LoginEcho,
   ef_EvAlert,
   ef_Recover,
   ef_EvEcho,
   ef_HB,
   ef_HBEcho,
   ef_CmdReq,
   ef_None,
};
//---------------------------------------------------------------------------
struct THeadStruct{
   char_aryn<char, 3,' '>   Func_;
   char_aryn<char, 4,' '>   Len_;
};
#define HSize  sizeof(THeadStruct);
//---------------------------------------------------------------------------
typedef char_aryn<char, 20,' '> TProcName;
typedef char_aryn<char,4,' '>   TBrkID;
#define Login_sz  sizeof(TProcName)+HSize
//---------------------------------------------------------------------------
struct TEvAlert{
   char_aryn<char, 1,' '>   Market_;
   char_aryn<char,24,' '>   DateTime_;
   char_aryn<char,20,' '>   Host_;                  //主機名稱
   TProcName                ProgName_;
   TProcName                ProcName_;
   TBrkID                   BrkID_;
   char_aryn<char,20,' '>   Object_;
   char_aryn<char, 6,' '>   EvCode_;
   char_aryn<char,60,' '>   Remark_;
};
#define EvAlert_sz  sizeof(TEvAlert)+HSize
//---------------------------------------------------------------------------
struct TCmdReq{
   TProcName                ProcName_;
   TProcName                ObjName_;
   char_aryn<char, 6,' '>   CmdID_;
   char_aryn<char,40,' '>   Msg_;
};
#define CmdReq_sz  sizeof(TCmdReq)+HSize
//---------------------------------------------------------------------------
struct TProgProc{
   TProcName ProgName_;
   TProcName ProcName_;
};
//---------------------------------------------------------------------------
struct TEvPkt : public THeadStruct{
   union{
      TEvAlert              EvAlert_;
      TProgProc             ProgProc_;
      TCmdReq               CmdReq_;
      char                  Data_[256];
   };
   K_ctor TEvPkt(){memset(Data_,0,sizeof(Data_));}
   K_ctor TEvPkt(EFuncCode ef){
      char buf[16];
      memset(Data_,0,sizeof(Data_));
      sprintf(buf,"%03d",ef);
      Func_.assign(buf);
      switch(ef){
         case ef_Login:
         case ef_LoginEcho:
            sprintf(buf,"%04d", sizeof(TProgProc)+sizeof(THeadStruct));
            break;
         case ef_EvAlert:
         case ef_Recover:
            sprintf(buf,"%04d", sizeof(TEvAlert)+sizeof(THeadStruct));
            break;
         case ef_CmdReq:
            sprintf(buf,"%04d", sizeof(TCmdReq)+sizeof(THeadStruct));
            break;
         case ef_EvEcho:
         case ef_HB:
         case ef_HBEcho:
         default:
            sprintf(buf,"%04d", sizeof(THeadStruct));
            break;
      }
      Len_.assign(buf);
   }
};
//---------------------------------------------------------------------------
K_class TEvSesMgr;
//---------------------------------------------------------------------------
typedef TPacketSes<TEvPkt,7,sizeof(TEvPkt)> TEvSes_Aux;
K_class TEvSes_Base : public TEvSes_Aux
{
   typedef TEvSes_Base      this_type;
   typedef TEvSes_Aux       inherited;
   typedef Kway::TTimer<this_type> TEvTimer;
protected:
   TEvSesMgr*         SesMgr_;
   TEvTimer           Timer_;
   std::string        IP_,ProgName_, ProcName_;
   bool               Ready_;
   virtual K_mf(void) OnTimer        (TTimerBase*){};
   K_mf(void)         ResetTimer     (size_t secs);
   K_mf(void)         SendP          (TEvPkt&);
   K_mf(int)          GetPacketSize  (TEvPkt*,bool);
   virtual K_mf(void) OnMesLinkBroken(TMesBase*, const std::string&);
   K_mf(void)         OnMesLinkFail  (TMesBase*, const std::string&){Ready_ = false;}
public:
   K_ctor             TEvSes_Base    (Kway::TSesMgr* sm);
   K_dtor             ~TEvSes_Base   (){};
   K_mf(bool)         IsReady        (){return Ready_;}
   K_mf(std::string)  GetSesProcName (){return ProcName_;}
   K_mf(std::string)  GetSesProgName (){return ProgName_;}
   K_mf(void)         SendCmdReq    (TCmdReq&);
};
//---------------------------------------------------------------------------
K_class TEvCliSes : public TEvSes_Base
{
   typedef TEvSes_Base      inherited;
private:
protected:
   K_mf(void)         OnMesLinkReady(TMesBase*);
   K_mf(void)         OnPktRecv     (TEvPkt&);
   K_mf(void)         OnTimer       (TTimerBase*);
public:
   K_ctor             TEvCliSes     (Kway::TSesMgr* sm);
   K_dtor             ~TEvCliSes    ();
   K_mf(void)         SendAlert     (TEvAlert&, bool recover = false);
};
//---------------------------------------------------------------------------
K_class TEvSvrSes : public TEvSes_Base
{
   typedef TEvSes_Base      inherited;
private:
protected:
   K_mf(void)         OnMesLinkReady(TMesBase*);
   K_mf(void)         OnPktRecv     (TEvPkt&);
   K_mf(void)         OnTimer       (TTimerBase*);
   K_mf(void)         OnMesLinkBroken(TMesBase*, const std::string&);
public:
   K_ctor             TEvSvrSes     (Kway::TSesMgr* sm);
   K_dtor             ~TEvSvrSes    ();
};
//---------------------------------------------------------------------------
K_class TMsgSes : public TLineSes
{
   typedef TLineSes inherited;
   typedef TMsgSes  this_type;
   typedef Kway::TTimer<this_type> TMsgTimer;
private:
   enum EMsgField{
      emf_Time = 0,
      emf_EvCode,
      emf_Market,
//20130304 eric 增加主機欄(Host), 因應多套主機環境
      emf_Host,      
      emf_Process,
      emf_SubSys,
      emf_Object,
      emf_Remark,
      emf_end,
   };
   int             CurSeqNo_;
   int             LastSeqNo_;
   int             LineNo_;
   bool            ByTelnet_;
   TEvSesMgr*      SesMgr_;
   TMsgTimer*      MsgTimer_;
   K_mf(void)      OnTimer       (TTimerBase*);
protected:
   K_mf(void)      OnPktRecv        (void* , int);
   K_mf(void)      OnMesLinkReady   (TMesBase*){};
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
public:
   K_ctor         TMsgSes(Kway::TSesMgr* sm);
// --> 20141022 modify by k288 for 解構時檢查Timer是否已刪除   
   K_dtor         ~TMsgSes(){	if(MsgTimer_) {delete MsgTimer_; MsgTimer_ = NULL;} };
//   K_dtor         ~TMsgSes(){};
// <-- 20141022 modify by k288 for 解構時檢查Timer是否已刪除   
   K_mf(bool)     SettingApply  (const std::string&);
};
//---------------------------------------------------------------------------
K_class TEvCenter;
K_class TEvSesMgr : public TSesMgrR,
                    public TExcMgrObj
{
private:
   typedef std::vector<TEvCliSes*> TEvSess;
   typedef std::vector<TEvSvrSes*> TCmdSess;
   typedef std::vector<TEvAlert>   TEvs;
   typedef Kstl::ev_map<int, int>  TMsgCnt;
   typedef TStdRecorder<TMsgCnt, int>  TMsgCntRec;
   typedef Kstl::ev_map<std::string, TEvAlert> TCurrStat;
   TCurrStat                CurrStat_;
   TEvSess                  EvSess_;
   TCmdSess                 CmdSess_;
   TEvs                     Evs_;
   Kway::TFile              Log_;
   TEvAlert                 EvAlert_;
   TCmdReq                  CmdReq_;
   TEvCenter*               EvCenter_;
   TMsgCnt                  MsgCnt_;
   TMsgCntRec*              MsgCntRec_;
protected:
public:
   K_ctor          TEvSesMgr(TEvCenter*,bool isagent = true);
   K_dtor          ~TEvSesMgr();
   K_mf(bool)      DoAlert(TErrLevel, TErrCode, TErrLevel, std::string&, std::string&, std::string&);
   K_mf(void)      WriteLog(const char*);
   K_mf(void)      CollectEv(TEvAlert& EvAlert, bool recover = false);
   K_mf(void)      RegEvCliSes(TEvCliSes* obj);
   K_mf(void)      DelEvCliSes(TEvCliSes* obj);
   K_mf(void)      RegEvSvrSes(TEvSvrSes* obj);
   K_mf(void)      DelEvSvrSes(TEvSvrSes* obj);
   K_mf(void)      DoRecover(TEvCliSes* obj);
   K_mf(bool)      SetMsgCnt  (int cnt, int lineno = 0);
   K_mf(int)       GetMsgCnt  (int lineno = 0);
   K_mf(void)      SendCmdReq(std::string& procname, std::string& objname, Kway::word cmdid, std::string msg);
   K_mf(void)      LinkStateChg(TProcName, TProcName, bool ready = true);

};
//---------------------------------------------------------------------------
//Event Center
//---------------------------------------------------------------------------
struct TEvData
{
   TProcName                     ProgName_;
   TProcName                     ProcName_;
   TBrkID                        BrkID_;
   char_ary<char,  1,' '>        Market_;
   char_ary<char, 20,' '>        Host_;                  //主機名稱
   TKDateTimeMS                  Time_;
   TObjID                        GrpID_;                 //事件群組別
   TProcName                     Object_;            
   TErrCode                      EvCode_;                //事件代號
   TErrLevel                     EvLevel_;               //等級
   char_ary<char,120,' '>        EvMsg_;                 //訊息
   char_ary<char,120,' '>        Action_;                //採取措施
   char_ary<char, 60,' '>        ExtMsg_;                //附加訊息
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<Kway::dword, TEvData>      TEvDatas;
typedef TEvDatas::iterator                      TEvDatai;
typedef TStdRecorder<TEvDatas, TEvData>         TEvDataRec;
//---------------------------------------------------------------------------
K_class TEvDataFieldMaker                              //第2層的顯示欄位
{
private:
public:
   K_ctor TEvDataFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 11; }
   K_mf(std::string) Get(const TEvDatai&, size_t) const;
   K_mf(void) Put(TEvDatas&, TEvDatai&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
struct TEvDataKeyMaker
{
   inline K_mf(void) operator()(Kway::dword& akey, const char* ckey) const
                               {akey = atoi(ckey);}
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TEvDatas,TEvDataFieldMaker,TEvDataKeyMaker> TEvDataViewer;    
//---------------------------------------------------------------------------
K_class TEvDataVACr:public Kway::TVACreater            //第2層的VAC
{
 typedef Kway::TVACreater inherited;
 typedef TEvDataVACr       this_type;
private:
   TEvDatas*             EvDatas_;
public:
   K_ctor TEvDataVACr(TEvDatas* evd):EvDatas_(evd){};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.reset();
      return Kway::TaViewArea(new TEvDataViewer(EvDatas_, TEvDataFieldMaker(), vcs));
   }
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<Kway::dword, TEvDatai>  TEvDatais;
typedef TEvDatais::iterator                  TEvDataisi;
//---------------------------------------------------------------------------
K_class TEvDataisFieldMaker                              //第2層的顯示欄位
{
private:
public:
   K_ctor TEvDataisFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 7; }
   K_mf(std::string) Get(const TEvDataisi&, size_t) const;
   K_mf(void) Put(TEvDatais&, TEvDataisi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TEvDatais,TEvDataisFieldMaker,TEvDataKeyMaker> TEvDataisViewer;    
//---------------------------------------------------------------------------
K_class TEvDataisVACr:public Kway::TVACreater            //第2層的VAC
{
 typedef Kway::TVACreater inherited;
 typedef TEvDataisVACr     this_type;
private:
   TEvDatais*             EvDatais_;
public:
   K_ctor TEvDataisVACr():EvDatais_(0){};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.reset();
      return Kway::TaViewArea(new TEvDataisViewer(EvDatais_, TEvDataisFieldMaker(), vcs));
   }
   K_mf(void) SetContainer(TEvDatais* evmp){EvDatais_ = evmp;}
};
//---------------------------------------------------------------------------
struct TEvDataExt: public TEvData
{
   TEvDatais*    EvDatais_;   
   K_mf(void) operator= (const TEvData& a)
   {
      memcpy(this, &a, sizeof(TEvData));
   } 
   
};
typedef Kstl::ev_map<TProcName, TEvDataExt>  TObjEvMap;
typedef TObjEvMap::iterator                  TObjEvMapi;
typedef Kstl::ev_map<TObjID, TObjEvMap>      TGrpEvMapBase;
struct TGrpEvMap : public TGrpEvMapBase
{
   bool   Ready_;
   TKTime Time_;
};
typedef TGrpEvMap::iterator                  TGrpEvMapi;
typedef Kstl::ev_map<TProcName, TGrpEvMap>   TProcEvMap;
typedef TProcEvMap::iterator                 TProcEvMapi;
typedef Kstl::ev_map<TBrkID, TProcEvMap>     TBrkEvMap;
typedef TBrkEvMap::iterator                  TBrkEvMapi;
typedef Kstl::ev_map<TProcName, TBrkEvMap>   TProgEvMap;
typedef TProgEvMap::iterator                 TProgEvMapi;
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TProcName, TEvDatais>   THObjEvMap;
typedef THObjEvMap::iterator                 THObjEvMapi;
typedef Kstl::ev_map<TProcName, THObjEvMap>  THProcEvMap;
typedef THProcEvMap::iterator                THProcEvMapi;
typedef Kstl::ev_map<TBrkID, THProcEvMap>    THBrkEvMap;
typedef THBrkEvMap::iterator                 THBrkEvMapi;
typedef Kstl::ev_map<TProcName, THBrkEvMap>  THProgEvMap;
typedef THProgEvMap::iterator                THProgEvMapi;
//---------------------------------------------------------------------------
K_class TObjEvFieldMaker                              //Group層的顯示欄位
{
private:
public:
   K_ctor TObjEvFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 7; }
   K_mf(std::string) Get(const TObjEvMapi&, size_t) const;
   K_mf(void) Put(TObjEvMap&, TObjEvMapi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TObjEvMap,TObjEvFieldMaker> TObjEvViewer;    
//---------------------------------------------------------------------------
K_class TObjEvVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TObjEvVACr       this_type;
private:
   TObjEvMap*             ObjEv_;
   TEvDataisVACr*         DetailCreater_;
public:
   K_ctor TObjEvVACr(){DetailCreater_ = new TEvDataisVACr(); ObjEv_ = 0;};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.reset();
//      if(ObjEv_ == 0)
//         return Kway::TaViewArea(new TObjEvViewer(0));
//      else
         return Kway::TaViewArea(new TObjEvViewer(ObjEv_, TObjEvFieldMaker(), vcs));
   }
   K_mf(void) SetContainer(TObjEvMap* evmp){ObjEv_ = evmp;}
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
K_class TGrpEvFieldMaker                              //Grpess層的顯示欄位
{
private:
public:
   K_ctor TGrpEvFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 1; }
   K_mf(std::string) Get(const TGrpEvMapi&, size_t) const;
   K_mf(void) Put(TGrpEvMap&, TGrpEvMapi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TGrpEvMap,TGrpEvFieldMaker> TGrpEvViewer;    
//---------------------------------------------------------------------------
K_class TGrpEvVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TGrpEvVACr       this_type;
private:
   TGrpEvMap*              GrpEv_;
   TObjEvVACr*             DetailCreater_;
public:
   K_ctor TGrpEvVACr(){DetailCreater_ = new TObjEvVACr(); GrpEv_ = 0;};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.reset();
//      if(GrpEv_ == 0)
//         return Kway::TaViewArea(new TGrpEvViewer(0));
//      else
         return Kway::TaViewArea(new TGrpEvViewer(GrpEv_, TGrpEvFieldMaker(), vcs));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
   K_mf(void) SetContainer(TGrpEvMap* evmp){GrpEv_ = evmp;}
};
//---------------------------------------------------------------------------
K_class TProcEvFieldMaker                              //Process層的顯示欄位
{
private:
public:
   K_ctor TProcEvFieldMaker(){
   };
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 3; }
   K_mf(std::string) Get(const TProcEvMapi&, size_t) const;
   K_mf(void) Put(TProcEvMap&, TProcEvMapi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TProcEvMap,TProcEvFieldMaker> TProcEvViewer;    
//---------------------------------------------------------------------------
K_class TProcEvVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TProcEvVACr       this_type;
private:
   TProcEvMap*             ProcEv_;
   TGrpEvVACr*             DetailCreater_;
   TProcEvFieldMaker       FMaker_;
public:
   K_ctor TProcEvVACr(){DetailCreater_ = new TGrpEvVACr(); ProcEv_ = 0;};
   K_ctor TProcEvVACr(TProcEvMap* evm){DetailCreater_ = new TGrpEvVACr(); ProcEv_ = evm;};
   K_mf(Kway::TaViewArea) CreateNewVA(){

      TViewControls   vcs;
      vcs.reset();
//      if(ProcEv_ == 0)
//         return Kway::TaViewArea(new TProcEvViewer(0));
//      else
         return Kway::TaViewArea(new TProcEvViewer(ProcEv_, FMaker_, vcs));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
   K_mf(void) SetContainer(TProcEvMap* evmp){
      ProcEv_ = evmp;
   }
};
//---------------------------------------------------------------------------
K_class TBrkEvFieldMaker                              //Broker層的顯示欄位
{
private:
public:
   K_ctor TBrkEvFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 1; }
   K_mf(std::string) Get(const TBrkEvMapi&, size_t) const;
   K_mf(void) Put(TBrkEvMap&, TBrkEvMapi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TBrkEvMap,TBrkEvFieldMaker> TBrkEvViewer;    
//---------------------------------------------------------------------------
K_class TBrkEvVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TBrkEvVACr       this_type;
private:
   TBrkEvMap*             BrkEv_;
   TProcEvVACr*           DetailCreater_;
public:
   K_ctor TBrkEvVACr(){DetailCreater_ = new TProcEvVACr();};
   K_mf(Kway::TaViewArea) CreateNewVA();
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
   K_mf(void) SetContainer(TBrkEvMap* evmp){BrkEv_ = evmp;}
};
//---------------------------------------------------------------------------
K_class TProgEvFieldMaker                              //Program層的顯示欄位
{
private:
public:
   K_ctor TProgEvFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 1; }
   K_mf(std::string) Get(const TProgEvMapi&, size_t) const;
   K_mf(void) Put(TProgEvMap&, TProgEvMapi&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TProgEvMap,TProgEvFieldMaker> TProgEvViewer;    
//---------------------------------------------------------------------------
K_class TProgEvVACr:public Kway::TVACreater            
{
 typedef Kway::TVACreater inherited;
 typedef TProgEvVACr       this_type;
private:
   TProgEvMap*             ProgEv_;
   TBrkEvVACr*             DetailCreater_;
public:
   K_ctor TProgEvVACr(TProgEvMap* ovd):ProgEv_(ovd){DetailCreater_ = new TBrkEvVACr();}
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      vcs.reset();
//      if(ProgEv_ == 0)
//         return Kway::TaViewArea(new TProgEvViewer(0));
//      else
         return Kway::TaViewArea(new TProgEvViewer(ProgEv_, TProgEvFieldMaker(), vcs));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
K_class TEvCenter{
private:
   struct TProcProp{
      TProcEvMap   ProcEvMap_;
      TProcEvVACr  ProcVACr_;
   };
   struct TBrkProp{
      TBrkEvMap    BrkEvMap_;
      TBrkEvVACr   BrkVACr_;
   };
   typedef Kstl::ev_map<TProcName, TProcProp> TProgList;
   typedef Kstl::ev_map<TProcName, TBrkProp>  TXTwseList;
   TProgList       ProgList_;
   TXTwseList      XTwseList_;
   TEvDataVACr*    EvDataVACr_;
   TEvDatas*       EvDatas_;
   TEvDataRec*     EvDataRec_;
   TViewAreaMgr    EvDataVA_, SesMgrVA_, EvVa_;
   TOpenSesMgr*    OpenSesMgr_;
   THProgEvMap*    EvIndex_;
   K_mf(void)      AddEvIndex(TEvDatai& edi);
protected:
public:
   K_ctor          TEvCenter();
   K_dtor          ~TEvCenter();
   K_mf(void)      AddEv(TEvAlert& EvAlert, bool recover = false);
   K_mf(TViewAreaMgr*) GetVA(){return &EvDataVA_;}
   K_mf(void)      DoLinkStateChg(TProcName, TProcName, bool ready = true);
};
//---------------------------------------------------------------------------
};//namespace Kway
#endif
