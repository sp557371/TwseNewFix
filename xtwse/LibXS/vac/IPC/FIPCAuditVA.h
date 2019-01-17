//---------------------------------------------------------------------------
#ifndef FIPCAuditVAH
#define FIPCAuditVAH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

#include "FevcViewerMgr.h"
#include "FIPCGroupMgr.h"
#include "KTime.h"
#include "fixed_num.h"
#include "ev_ary.h"

class TIPCGroupMgr;
class TIPCAgents;
//---------------------------------------------------------------------------
class TIPCAuditAgents : public Kstl::ev_map<std::string, TIPCDetailMessages>, Kstl::container_ev_handler
{
   typedef Kstl::ev_map<std::string, TIPCDetailMessages>  inherited;
   TIPCGroupMgr*  Mgr_;
   TKDate         Date_;

   typedef Kstl::container_ev_handler::size_type   size_type;
   virtual K_mf(void) OnContainerBfDestroy (const Kstl::ev_container&);
   virtual K_mf(void) OnContainerAfClear   (const Kstl::ev_container&);
   virtual K_mf(void) OnContainerAfAssign  (const Kstl::ev_container&);
   virtual K_mf(void) OnContainerAfInsert  (const Kstl::ev_container&, const Kstl::ev_iterator& i, size_type count);
   virtual K_mf(void) OnContainerBfErase   (const Kstl::ev_container&, const Kstl::ev_iterator& i, size_type count);

   K_mf(void) insert (const std::string& agentName);
public:
   K_ctor TIPCAuditAgents ()  : Mgr_() {}
   K_ctor TIPCAuditAgents (const TIPCAuditAgents& r)
      : Mgr_(NULL)
   {
      operator=(r);
   }
   K_dtor ~TIPCAuditAgents ();

   K_mf(void) operator= (const TIPCAuditAgents& r);

   K_mf(void) Initialize (TIPCGroupMgr* mgr, const TKDate& date);
};
//---------------------------------------------------------------------------
class TIPCAuditAgentsVACr : public Kway::TVACreater
{
   TIPCDetailMessagesVACr  MessagesVACr_;
public:
   virtual K_mf(Kway::TaViewArea)  CreateNewVA          ();
   virtual K_mf(size_t)            GetDetails           (const char**& a);
   virtual K_mf(Kway::TVACreater*) GetDetailVACreater   (size_t detailID);
   virtual K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<Kway::TKDate, TIPCAuditAgents>   TIPCAuditMap;

class TIPCAuditVACr : public Kway::TVACreater
{
   TIPCAuditMap         AuditMap_;
   TIPCAuditAgentsVACr  AgentsVACr_;
   TIPCGroupMgr*        IPCGroupMgr_;
public:
   K_ctor TIPCAuditVACr(TIPCGroupMgr*);

   virtual K_mf(Kway::TaViewArea)  CreateNewVA          ();
   virtual K_mf(size_t)            GetDetails           (const char**& a);
   virtual K_mf(Kway::TVACreater*) GetDetailVACreater   (size_t detailID);
   virtual K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);

   K_mf(void) AddAuditDate (const Kway::TKDate&);
   K_mf(void) Logout       ();
};
/////////////////////////////////////////////////////////////////////////////
class TIPCAuditVA : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelVA;
   TPanel *PanelControls;
   TBitBtn *btnStartQuery;
   TDateTimePicker *DatePicker;
   TLabel *lbDateSelect;
   TLabel *lbMsg;
   void __fastcall btnStartQueryClick(TObject *Sender);
private:	// User declarations
   TIPCAuditVACr  AuditVACr_;
   TevcViewerMgr* Viewer_;
public:		// User declarations
   __fastcall  TIPCAuditVA(TIPCGroupMgr* Owner);
   __fastcall ~TIPCAuditVA();

   void __fastcall Logout();
};
//---------------------------------------------------------------------------
#endif

