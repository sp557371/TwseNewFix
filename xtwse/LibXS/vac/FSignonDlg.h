//
// vac - ViewArea Client - 簽入對話
//
// 版本: $Id: FSignonDlg.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef FSignonDlgH
#define FSignonDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "FMsgDlg.h"
#include "SignonClient.h"
using namespace Kway::Signon;
//---------------------------------------------------------------------------
class TSignonDlg : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelButtons;
   TPanel *PanelUser;
   TPanel *Panel3;
   TBitBtn *bbOK;
   TBitBtn *bbCancel;
   TBitBtn *bbChgPass;
   TLabel *Label1;
   TLabel *Label2;
   TLabel *Label3;
   TComboBox *cbServers;
   TEdit *edUser;
   TEdit *edPass;
   TTimer *Timer;
   TPanel *PanelChgPass;
   TLabel *Label7;
   TEdit *edNewPass1;
   TEdit *edNewPass2;
   TLabel *Label5;
   TBitBtn *bbChgPassCfm;
   TBitBtn *bbChgPassClose;
   TPanel *PanelGroups;
   TComboBox *cbGroups;
   TLabel *Label4;
   void __fastcall TimerTimer(TObject *Sender);
   void __fastcall FormKeyPress(TObject *Sender, char &Key);
   void __fastcall bbChgPassClick(TObject *Sender);
   void __fastcall bbChgPassCfmClick(TObject *Sender);
   void __fastcall bbChgPassCloseClick(TObject *Sender);
   void __fastcall bbOKClick(TObject *Sender);
private:
   struct TSignonAckEv : public Kway::Signon::TSignonAckEv {
      TSignonDlg* dlg_;
      K_ctor      TSignonAckEv    (TSignonDlg* dlg)  : dlg_(dlg) {}
      K_mf(void)  OnSignonResult  (TSignonClient&, const std::string&);
      K_mf(void)  OnSignonChgPass (TSignonClient&, const std::string&);
   };
   TSignonAckEv   SignonAck_;
   TSignonClient* SignonClient_;
   TaMsgDlg       MsgDlg_;

   friend class TSignonAckEv;
   K_mf(void)  OnSignonResult   (const std::string&);
   K_mf(void)  OnSignonChgPass  (const std::string&);
   K_mf(void)  PanelVisible     (TPanel*, bool setVisible);
   K_mf(std::string)  GetEdUser  ();
public:
        __fastcall TSignonDlg(TComponent* Owner);

   bool __fastcall Execute (TSignonClient&);
};
/////////////////////////////////////////////////////////////////////////////
#endif

