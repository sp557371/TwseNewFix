//---------------------------------------------------------------------------
#ifndef FIPCExecResultVAH
#define FIPCExecResultVAH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "FevcViewerMgr.h"
#include "FIPCGroupMgr.h"
/////////////////////////////////////////////////////////////////////////////
class TIPCOutMessages;
class TOnIPCOutMessageHandler
{
public:
   virtual K_mf(void) OnIPCOutMessage(const TIPCOutMessages&, TMSCliMessagePtr& msgptr) = 0;
};
class TIPCOutMessages : public TIPCDetailMessages
{
   typedef TIPCDetailMessages inherited;
   std::string                SessionIDName_;//¦æµ{ID
   std::string                SubName_;
   TOnIPCOutMessageHandler*   Handler_;
   std::string                GroupName_;
protected:
   virtual K_mf(void) OnMomMessage (TMSCliMessagePtr& msgptr);
public:
   K_ctor TIPCOutMessages()
      : Handler_(NULL)
      { }
   K_mf(void) Subscribe (const std::string& sessionIDName, TIPCGroupMgr* mgr, const char* destName, TKDate date, const std::string& subName) {
      SessionIDName_ = sessionIDName;
      SubName_ = subName;
      inherited::Subscribe(mgr, destName, date, subName);
   }
   inline K_mf(const std::string&) GetSessionIDName() const {
      return SessionIDName_;
   }
   inline K_mf(const std::string&) GetSubName() const {
      return SubName_;
   }
   inline K_mf(const std::string&) GetGroupName() const {
      return GroupName_;
   }
   inline K_mf(void) SetMessageHandler (TOnIPCOutMessageHandler* handler, const std::string& groupName) {
      Handler_ = handler;
      GroupName_ = groupName;
   }
};
//---------------------------------------------------------------------------
//key = SessionIDName
class TIPCProcessMessages : public Kstl::ev_map<std::string, TIPCOutMessages*>, Kstl::container_ev_handler
{
   typedef Kstl::ev_map<std::string, TIPCOutMessages*>   inherited;

   virtual K_mf(void) OnContainerAfInsert (const Kstl::ev_container& evc, const Kstl::ev_iterator&, Kstl::container_ev_handler::size_type) {
      iterator ifind = find(static_cast<const TIPCOutMessages*>(&evc)->GetSubName());
      if(ifind != end()) {
         updater u = begin_update(ifind);
      }
   }
public:
   std::pair<iterator,bool> insert (const value_type& v) {
      std::pair<iterator,bool> rtn = inherited::insert(v);
      if(rtn.second)
         v.second->add_ev_handler(this);
      return rtn;
   }
};

class TIPCProcessMessagesVACr : public Kway::TVACreater
{
   TIPCDetailMessagesVACr  MessagesVACr_;
public:
   virtual K_mf(Kway::TaViewArea)  CreateNewVA          ();
   virtual K_mf(size_t)            GetDetails           (const char**& a);
   virtual K_mf(Kway::TVACreater*) GetDetailVACreater   (size_t detailID);
   virtual K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
/////////////////////////////////////////////////////////////////////////////
// key = AgentName or ExecSessionID(Time/GroupName)
class TIPCExecDetail : public Kstl::ev_map<std::string, TIPCProcessMessages>
{
public:
   std::string  Why_;
};

class TIPCExecDetailVACr : public Kway::TVACreater
{
   TIPCProcessMessagesVACr ProcessMessagesVACr_;
public:
   virtual K_mf(Kway::TaViewArea)  CreateNewVA          ();
   virtual K_mf(size_t)            GetDetails           (const char**& a);
   virtual K_mf(Kway::TVACreater*) GetDetailVACreater   (size_t detailID);
   virtual K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};

// key = ExecSessionID(Time/GroupName) or AgentName
typedef Kstl::ev_map<std::string, TIPCExecDetail>     TIPCExecMaster;

class TIPCExecMasterVACr : public Kway::TVACreater
{
   TIPCExecDetailVACr   ExecDetailVACr_;
public:
   virtual K_mf(Kway::TaViewArea)  CreateNewVA          ();
   virtual K_mf(size_t)            GetDetails           (const char**& a);
   virtual K_mf(Kway::TVACreater*) GetDetailVACreater   (size_t detailID);
   virtual K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
/////////////////////////////////////////////////////////////////////////////
class TIPCExecSession : public TIPCExecResultSession, public TOnIPCOutMessageHandler
{
   typedef TIPCExecResultSession inherited;
   // key = AgentName/HHMMSS.nnn/GroupName/SessionIDName
   typedef Kstl::ev_map<std::string, TIPCOutMessages> TIPCProcess;
   TIPCProcess    Process_;
   TIPCExecMaster ExecMaster_;
   bool           IsSetStatus_;

   virtual K_mf(void) OnSubscribeFail (const TDestSub& dest, EMSCliResult rCode);
   virtual K_mf(void) OnNewExecResults(const TKTime& exectm, const std::string& groupName, const std::string& agentName, const std::string& sid, const std::string& pid, const std::string& why);
   virtual K_mf(void) OnIPCOutMessage (const TIPCOutMessages&, TMSCliMessagePtr& msgptr);
public:
   K_ctor TIPCExecSession ()
      : IsSetStatus_(false)
      { }
   inline K_mf(TIPCExecMaster&) GetExecMaster () {
      return ExecMaster_;
   }
   inline K_mf(void) SetIsSetStatus (bool value) {
      IsSetStatus_ = value;
   }
};
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::ev_map<Kway::TKDate, TIPCExecSession>   TIPCExecDates;

class TIPCExecDatesVACr : public Kway::TVACreater
{
   TIPCGroupMgr*        IPCGroupMgr_;
   TIPCExecDates        ExecDates_;
   TIPCExecMasterVACr   ExecMasterVACr_;
public:
   K_ctor TIPCExecDatesVACr(TIPCGroupMgr*);

   virtual K_mf(Kway::TaViewArea)  CreateNewVA          ();
   virtual K_mf(size_t)            GetDetails           (const char**& a);
   virtual K_mf(Kway::TVACreater*) GetDetailVACreater   (size_t detailID);
   virtual K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);

   K_mf(void) AddDate        (const Kway::TKDate&);
   K_mf(void) SetIsSetStatus (const Kway::TKDate&);
   K_mf(void) Logout         ();
};
/////////////////////////////////////////////////////////////////////////////
class TIPCExecResultVA : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelVA;
   TPanel *PanelControls;
   TLabel *lbDateSelect;
   TLabel *lbMsg;
   TBitBtn *btnStartQuery;
   TDateTimePicker *DatePicker;
   void __fastcall btnStartQueryClick(TObject *Sender);
private:	// User declarations
   TIPCExecDatesVACr ExecDatesVACr_;
   TevcViewerMgr*    Viewer_;

   K_mf(void) OnSetVACreater (TevcViewerMgr*, TevcViewer*, Kway::TVACrControl*, Kway::TVACreater* vaCr, const std::string& vaIdName, Kway::TViewArea* parent);
public:		// User declarations
   __fastcall  TIPCExecResultVA(TIPCGroupMgr* Owner);
   __fastcall ~TIPCExecResultVA();

   void __fastcall Logout   ();
   void __fastcall ReadDate (TDate date, bool isSetStatus);
};
//---------------------------------------------------------------------------
#endif

