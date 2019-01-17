//---------------------------------------------------------------------------
#ifndef FIPCCenterChgPassDlgH
#define FIPCCenterChgPassDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>

#include "MomConnMgr.h"
#include "FMsgDlg.h"
//---------------------------------------------------------------------------
class TIPCCenterChgPassDlg : public TForm
{
__published:	// IDE-managed Components
   TLabel *lbNewPass1;
   TLabel *lbNewPass2;
   TLabel *lbUser;
   TEdit *edNewPass1;
   TEdit *edNewPass2;
   TBitBtn *bbOK;
   TBitBtn *bbCancel;
   TEdit *edUser;
   TLabel *lbOldPass;
   TEdit *edOldPass;
   void __fastcall bbOKClick(TObject *Sender);
private:	// User declarations
   class TMomConn : public TMomConnMgr, public Kway::Signon::TSignonAckEv {
      typedef TMomConnMgr     inherited;
      TIPCCenterChgPassDlg*   Owner_;
      TMsgDlg*                MsgDlg_;
      TSesCli*                SesCli_;
      std::string             NewPass_;
      bool                    IsNewPassSending_;
      virtual K_mf(void) MSCli_OnNotify       (TConnMgr&, EMSCliResult rCode, const std::string& reason);
      virtual K_mf(void) MSCli_OnPublisherReg (TConnMgr&, const TDestPub&, EMSCliResult rCode)  {}
      virtual K_mf(void) MSCli_OnSubscriberReg(TConnMgr&, const TDestSub&, EMSCliResult rCode)  {}
      virtual K_mf(void) MSCli_OnMessage      (TConnMgr&, TMSCliMessagePtr&)                    {}
      virtual K_mf(void) MSCli_OnMessageAck   (TConnMgr&, const TDestPub&, const TMessageID&, EMSCliResult) {}

      virtual K_mf(void)  OnSignonResult  (Kway::Signon::TSignonClient&, const std::string& result) {}
      virtual K_mf(void)  OnSignonChgPass (Kway::Signon::TSignonClient&, const std::string& result);
   public:
      K_ctor             TMomConn (TIPCCenterChgPassDlg* owner, const std::string& user, const std::string& pass);
      K_mf(TModalResult) Execute  (const std::string& mesParam, const std::string& newPass);
   };
   AnsiString  MesParam_;
public:		// User declarations
   __fastcall TIPCCenterChgPassDlg(TComponent* Owner);

   bool __fastcall Execute (const AnsiString& user, const AnsiString& mesParam);
};
//---------------------------------------------------------------------------
extern PACKAGE TIPCCenterChgPassDlg *IPCCenterChgPassDlg;
//---------------------------------------------------------------------------
#endif
