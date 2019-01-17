//---------------------------------------------------------------------------
#ifndef FIPCGroupMgrH
#define FIPCGroupMgrH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>

#include "MomConnMgr.h"
#include "FIPCGroupsVA.h"
#include "ev_ary.h"
#include <ActnList.hpp>

class TIPCAuditVA;
class TIPCExecResultVA;
class TShellScriptEditor;

using namespace Kway::Ms;
//---------------------------------------------------------------------------
class TIPCAgent {
	TListView*     ListView_;
	int            ListIndex_;
   std::string    Status_;

   K_mf(void) SetStatusImage (TListItem* item, const char* status) {
      if(strstr(status, "Agent disconnected")) {
         item->StateIndex = 0;
      } else
      if(strstr(status, "ERR:[")) {
         item->StateIndex = 3;
      } else
         item->StateIndex = 1;
   }
public:
   K_ctor TIPCAgent () : ListView_(NULL) {
   }
   K_ctor TIPCAgent (TListView* listView, TListItem* item, const std::string& agentName, const std::string& status)
      : ListView_(listView)
      , ListIndex_(item->Index)
      , Status_(status)
   {
      item->Caption = agentName.c_str();
      item->SubItems->Add(status.c_str());
      SetStatusImage(item, status.c_str());
   }
   K_mf(void) SetStatus (const std::string& status) {
      Status_ = status;
      TListItem* item = GetListItem();
      item->SubItems->Strings[0] = status.c_str();
      SetStatusImage(item, status.c_str());
   }
   K_mf(const std::string&) GetStatus () const {
      return Status_;
   }
   K_mf(TListItem*) GetListItem() const {
      return ListView_ ? ListView_->Items->Item[ListIndex_] : NULL;
   }
};
class TIPCAgents : public Kstl::ev_map<std::string, TIPCAgent>    { };

struct TIPCGroupRecsRollback : public TIPCGroupRecs {
   std::string UpdateUserID_;
   std::string OpCode_;//作業單號
};
typedef std::map<TKDateTimeMS, TIPCGroupRecsRollback>   TGroupsRollback;
//---------------------------------------------------------------------------
class TIPCGroupMgr : public TForm
{
__published:	// IDE-managed Components
   TImageList *hostStatusImages;
   TPopupMenu *pmAgentStatus;
   TMenuItem *miRemoveAgent;
   TActionList *Actions;
   TAction *aLogon;
   TAction *aLogout;
   TImageList *ActionImages;
   TAction *aEditSetting;
   TPageControl *pcInvisible;
   TPanel *Panel;
   TPanel *plAgents;
   TListView *lvAgents;
   TPanel *plGroupList;
   TPageControl *pcFunctions;
   TTabSheet *tsIPCCenter;
   TLabel *lbIPCCenterParams;
   TLabel *lbDestNameGroups;
   TLabel *lbDestNameGroupsExec;
   TLabel *lbDestNameExecResult;
   TLabel *lbDestNameAudit;
   TLabel *lbDestNameStatus;
   TLabel *lbIPCCenterMsg;
   TLabel *lbDestGroupsMsg;
   TLabel *lbDestGroupsExecMsg;
   TLabel *lbDestExecResultMsg;
   TLabel *lbDestAuditMsg;
   TLabel *lbDestStatusMsg;
   TLabel *lbDestNameGroupsRollback;
   TLabel *lbDestGroupsRollbackMsg;
   TLabel *lbDestNameTxFile;
   TLabel *lbDestTxFileMsg;
   TEdit *edIPCCenterParams;
   TEdit *edDestNameGroups;
   TEdit *edDestNameGroupsExec;
   TEdit *edDestNameExecResult;
   TEdit *edDestNameAudit;
   TEdit *edDestNameStatus;
   TBitBtn *btnSetting;
   TBitBtn *btnLogon;
   TBitBtn *btnLogout;
   TEdit *edDestNameGroupsRollback;
   TEdit *edDestNameTxFile;
   TTabSheet *tsGroups;
   TTabSheet *tsAudit;
   TTabSheet *tsExecResult;
   TSplitter *splGroup;
   void __fastcall aEditSettingClick(TObject *Sender);
   void __fastcall aLogonClick(TObject *Sender);
   void __fastcall aLogoutClick(TObject *Sender);
   void __fastcall pmAgentStatusPopup(TObject *Sender);
   void __fastcall lvAgentsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall miRemoveAgentClick(TObject *Sender);
private:	// User declarations
   TIPCGroupRecs  Groups_;
   TIPCAgents     Agents_;

   class TMomConn : public TMomConnMgr {
      typedef TMomConnMgr  inherited;
      TIPCGroupMgr*  Owner_;
      int            PubGroupsDestID_;
      int            PubGroupsRollbackDestID_;
      int            PubGroupsExecDestID_;
      int            PubAuditDestID_;
      int            PubStatusDestID_;
      int            PubTxFileDestID_;

      int            SubGroupsDestID_;
      int            SubGroupsRollbackDestID_;
      int            SubGroupsExecDestID_;
      int            SubStatusDestID_;
      int            SubTxFileDestID_;
      int            SubAuditDestID_;
      int            SubExecResultDestID_;
      std::string    ExecResultDestName_;
      TShellScriptEditor*  ShellScriptEditor_;

      virtual K_mf(void) MSCli_OnNotify        (TConnMgr&, EMSCliResult rCode, const std::string& reason);
      virtual K_mf(void) MSCli_OnPublisherReg  (TConnMgr&, const TDestPub&, EMSCliResult rCode);
      virtual K_mf(void) MSCli_OnSubscriberReg (TConnMgr&, const TDestSub&, EMSCliResult rCode);
      virtual K_mf(void) MSCli_OnMessage       (TConnMgr&, TMSCliMessagePtr&);
      virtual K_mf(void) MSCli_OnMessageAck    (TConnMgr&, const TDestPub&, const TMessageID&, EMSCliResult);

      K_mf(int) RegPublisher (const std::string& destName) {
         std::string name = Kway::ChangeYMD(destName.c_str());
         inherited::RegPublisher(name, dt_Dontcare, false);
         return inherited::GetPublishReqID(name, dt_Dontcare);
      }
   public:
      K_ctor  TMomConn(TIPCGroupMgr* owner, const std::string& user, const std::string& pass, const std::string& mesParam);
      K_dtor ~TMomConn();

      inline K_mf(int) GetPubStatusDestID() const {
         return PubStatusDestID_;
      }
      inline K_mf(int) GetPubTxFileDestID() const {
         return PubTxFileDestID_;
      }
      inline K_mf(void) SetShellScriptEditor(TShellScriptEditor* sh) {
         ShellScriptEditor_ = sh;
      }
   };
   TMomConn*         MomConnMgr_;
   TIPCGroupsVA*     IPCGroupsVA_;
   TIPCAuditVA*      IPCAuditVA_;
   TIPCExecResultVA* IPCExecResultVA_;
   Kway::TTimer<TIPCGroupMgr> Timer_;
   TDate             Today_;

   TGroupsRollback   GroupsRollback_;

   K_mf(void) CreateMomConnMgr (std::string user, std::string pass);
   K_mf(void) Logout           ();
   K_mf(void) OnLogoutTimer    (TTimerBase*);
   K_mf(void) OnCheckDateTimer (TTimerBase*);
   K_mf(void) ResetGroupStatus (TDate today);
public:		// User declarations
   __fastcall  TIPCGroupMgr(TComponent* Owner);
   __fastcall ~TIPCGroupMgr();
   K_mf(void)  DelayLogout ();

   K_mf(TIPCAgents&) GetIPCAgents () {
      return Agents_;
   }
   K_mf(void) ClearAgents () {
      lvAgents->Items->Clear();
      Agents_.clear();
   }
   K_mf(void) SetIPCAgentStatus (const std::string& agentName, const std::string& status) {
      TIPCAgents::iterator ifind = Agents_.find(agentName);
      if(ifind == Agents_.end()) {
         TIPCAgent agent(lvAgents, lvAgents->Items->Add(), agentName, status);
         Agents_.insert(TIPCAgents::value_type(agentName, agent));
      } else {
         TIPCAgents::updater  u(Agents_.begin_update(ifind));
         u->SetStatus(status);
      }
   }
   K_mf(void) SetProcessStatus(const std::string& groupName, const std::string& sid, const std::string& status);

   inline K_mf(AnsiString) GetDestNameAudit() const {
      return edDestNameAudit->Text;
   }
   inline K_mf(AnsiString) GetDestNameExecResult() const {
      return edDestNameExecResult->Text;
   }
   inline K_mf(TGroupsRollback&) GetGroupsRollback() {
      return GroupsRollback_;
   }
   inline K_mf(TIPCGroupRecs&) GetIPCGroups () {
      return Groups_;
   }
   inline K_mf(Kway::Ms::TMomMgr*) GetMomMgr () {
      return MomConnMgr_;
   }

   // opcode = 作業單號
   K_mf(void) ApplyNewIPCGroups      (const TIPCGroupRecs&, const std::string& opcode);
// K_mf(void) ApplyIPCGroupsEnabled  (const TIPCGroupRecs&);
   K_mf(void) ApplyIPCGroupsTodayHold(const TIPCGroupRecs&);

   K_mf(void) SetPubGroupsReady     ();
   K_mf(void) SetPubGroupsExecReady ();
   K_mf(void) SetReadGroupsReady    ();
   K_mf(void) SetReadExecResultReady();

   K_mf(void) ExecuteIPCGroup       (const std::string& groupName);
   K_mf(void) ExecuteIPCGroupProc   (const std::string& groupName, const std::string procid);

   K_mf(int)  SubscribeDetail       (const char* destName, TKDate date, const std::string& subName, TMomMessageHandler& handler);
   K_mf(void) Unsubscribe           (int handleID, TMomMessageHandler&);

   K_mf(void) EditShellScript (const std::string& agentName, const std::string& shFileName);

   inline K_mf(int) GetPubTxFileDestID() const {
      return MomConnMgr_ ? MomConnMgr_->GetPubTxFileDestID() : -1;
   }
};
//---------------------------------------------------------------------------
extern PACKAGE TIPCGroupMgr *IPCGroupMgr;
/////////////////////////////////////////////////////////////////////////////
//使用typedef在 BCB 會有問題! 所以只好用繼承的!
class TMsgSeqNo : public Kstl::fixed_num<Kway::dword> {
   typedef Kstl::fixed_num<Kway::dword>   inherited;
public:
   K_ctor TMsgSeqNo (Kway::dword v = 0)  : inherited(v) {}
};

class TIPCMomMessageHandler : public TMomMessageHandler
{
   TIPCGroupMgr*  Mgr_;
   int            SubHandleID_;
   K_mf(void) ClearMomMgr (TMomMgr*) {
      Mgr_ = NULL;
      SubHandleID_ = -1;
   }
public:
   K_ctor TIPCMomMessageHandler ()
      : Mgr_(NULL)
      , SubHandleID_(-1)
      { }
   K_dtor ~TIPCMomMessageHandler () {
      Unsubscribe();
   }
   K_mf(void) Unsubscribe () {
      if(Mgr_ && SubHandleID_ >= 0) {
         Mgr_->Unsubscribe(SubHandleID_, *this);
         SubHandleID_ = -1;
      }
   }
   K_mf(void) Subscribe (TIPCGroupMgr* mgr, const char* destName, TKDate date, const std::string& subName) {
      Mgr_ = mgr;
      SubHandleID_ = mgr->SubscribeDetail(destName, date, subName, *this);
   }
   inline K_mf(TIPCGroupMgr*) GetMgr()          { return Mgr_; }
   inline K_mf(int)   GetSubHandleID() const    { return SubHandleID_; }
};
class TIPCDetailMessages : public Kstl::ev_ary<TMsgSeqNo, std::string>, public TIPCMomMessageHandler
{
   typedef Kstl::ev_ary<TMsgSeqNo, std::string>  inherited;
protected:
   virtual K_mf(void) OnMomSubscribe (const TDestSub& dest, EMSCliResult rCode);
   virtual K_mf(void) OnMomMessage   (TMSCliMessagePtr& msgptr);
   virtual K_mf(void) ClearMomMgr    (TMomMgr*);
           K_mf(void) push_back      (const std::string& msg);
};
class TIPCDetailMessagesVACr : public Kway::TVACreater
{
public:
   virtual K_mf(Kway::TaViewArea) CreateNewVA ();
};
struct TIPCDetailMessagesFieldMaker
{
   typedef TIPCDetailMessages       TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;

   static K_mf(field_n_type)      GetFieldCount ();
   static K_mf(const TFieldDesp&) GetFieldDesp  (field_n_type n);
   K_mf(std::string) Get (const iterator& i, field_n_type fieldNo) const;
   K_mf(void) Put (TEvContainer& evc, iterator& i, field_n_type fieldNo, const std::string& str);
};
typedef TTViewArea<TIPCDetailMessages,TIPCDetailMessagesFieldMaker>   TIPCDetailMessagesVA;

class TIPCExecResultSession : public TIPCMomMessageHandler
{
   typedef TIPCMomMessageHandler inherited;
   // key = AgentName/HHMMSS.nnn/GroupName/SessionIDName
   TKDate   Date_;

   virtual K_mf(void) OnSubscribeFail (const TDestSub& dest, EMSCliResult rCode) = 0;
   virtual K_mf(void) OnMomSubscribe (const TDestSub& dest, EMSCliResult rCode) {
     switch(rCode) {
        case mcr_SubReady:
        case mcr_SubWorking:
        case mcr_SubRecoverEnd:
           break;
        default:
           OnSubscribeFail(dest, rCode);
           break;
     }
   }
   virtual K_mf(void) OnMomMessage (TMSCliMessagePtr& msgptr) {
      std::string execid = msgptr->GetMessageStr();
      //execid = AgentName/HHMMSS.nnn/GroupName/sid1,sid2,sid3...
      //例如: "IPC Win-1/100548.106/1.假開盤/Proc1,Proc2,Proc3"
      //      20090829: 將 AgentName 一律變成空白:
      //                Group的行程內容, 由[主控台]設定,
      //                sid = [行程ID], [行程ID]由主控台設定(不重複)
      std::string::size_type  sidpos = execid.rfind('/');
      if(sidpos == std::string::npos)
         return;
      std::string sids = execid.substr(sidpos+1);
      execid.erase(sidpos);

      std::string::size_type  apos = execid.find('/');
      if(apos == std::string::npos)
         return;
      std::string agentName = execid.substr(0, apos);
      TKTime      exectm;
      exectm.assign(execid.c_str() + apos + 1);
      apos = execid.find('/', apos + 1);
      if(apos == std::string::npos)
         return;
      std::string groupName = execid.substr(apos + 1);
      std::string why;
      apos = sids.find('\t', 0);
      if(apos != std::string::npos) {
         why = sids.substr(apos+1);
         sids.erase(apos);
      }

      if(sids.empty()) {
         OnNewExecResults(exectm, groupName, agentName, std::string(), std::string(), why);
      } else {
         execid.append(1, '/');
         sidpos = 0;
         do {
            apos = sids.find(',', sidpos);
            std::string sid = (apos == std::string::npos ? sids.substr(sidpos) : sids.substr(sidpos, apos - sidpos));
            std::string pid = execid + sid;
            OnNewExecResults(exectm, groupName, agentName, sid, pid, why);
            sidpos = apos + 1;
          } while(apos != std::string::npos);
       }
   }
   virtual K_mf(void) OnNewExecResults ( const TKTime&      exectm
                                       , const std::string& groupName
                                       , const std::string& agentName
                                       , const std::string& sid
                                       , const std::string& pid
                                       , const std::string& why ) = 0;
public:
   inline K_mf(const TKDate&) GetDate () const {
      return Date_;
   }
   inline K_mf(void) Subscribe (TIPCGroupMgr* mgr, TKDate date) {
      inherited::Subscribe(mgr, mgr->edDestNameExecResult->Text.c_str(), Date_ = date, std::string());
   }
};
/////////////////////////////////////////////////////////////////////////////
#endif

