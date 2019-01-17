//---------------------------------------------------------------------------
#ifndef FIPCEditShellScriptH
#define FIPCEditShellScriptH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "MomMgr.h"
#include <Buttons.hpp>
class TIPCGroupMgr;
class TShellScriptEditor;
using namespace Kway::Ms;
//---------------------------------------------------------------------------
class TEditShellScript : public TForm
{
__published:	// IDE-managed Components
   TMemo *EditArea;
   TPanel *PanelCtrl;
   TBitBtn *btnRestore;
   TBitBtn *btnSave;
   TBitBtn *btnClose;
   void __fastcall EditAreaChange(TObject *Sender);
   void __fastcall btnRestoreClick(TObject *Sender);
   void __fastcall btnCloseClick(TObject *Sender);
   void __fastcall btnSaveClick(TObject *Sender);
private:	// User declarations
   TShellScriptEditor*  Owner_;
public:		// User declarations
   __fastcall TEditShellScript(TShellScriptEditor* Owner);
};
//---------------------------------------------------------------------------
class TShellScriptEditor : public TMomMessageHandler
{
   TEditShellScript* Form_;
   TIPCGroupMgr*     Owner_;
   int               SubResultDestID_;
   std::string       AgentName_;
   std::string       ShFileName_;
   std::string       FileContent_;
   bool              IsFileReady_;
   bool              IsSaving_;
private:
   K_mf(void) DetachMom  ();
   K_mf(void) SaveEnabled(bool value);
   K_mf(bool) SubResult  (TDestMessage&, const std::string& auditstr);
public:
   K_ctor  TShellScriptEditor (TIPCGroupMgr*);
   K_dtor ~TShellScriptEditor ();

   virtual K_mf(void) OnMomMessage   (TMSCliMessagePtr& msgptr);
   virtual K_mf(void) OnMomSubscribe (const TDestSub& dest, EMSCliResult rCode);
   virtual K_mf(void) ClearMomMgr    (TMomMgr*);

   K_mf(void) OnMessageAck (EMSCliResult mcr);
   K_mf(void) Restore      ();
   K_mf(void) Save         ();
   K_mf(bool) IsChanged    () const;
   K_mf(void) Execute      (const std::string& agentName, const std::string& shFileName);
};
//---------------------------------------------------------------------------
#endif
