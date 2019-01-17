//---------------------------------------------------------------------------
#ifndef FIPCCenterLogonDlgH
#define FIPCCenterLogonDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TIPCCenterLogonDlg : public TForm
{
__published:	// IDE-managed Components
   TLabel *lbUser;
   TBitBtn *btnLogon;
   TBitBtn *btnCancel;
   TEdit *edUser;
   TLabel *lbPass;
   TEdit *edPass;
   TBitBtn *btnChgPass;
   void __fastcall btnChgPassClick(TObject *Sender);
private:	// User declarations
   AnsiString  MesParam_;
public:		// User declarations
   __fastcall TIPCCenterLogonDlg(TComponent* Owner);

   /// mesParam for change pass
   bool __fastcall Execute (const AnsiString& mesParam);
};
//---------------------------------------------------------------------------
extern PACKAGE TIPCCenterLogonDlg *IPCCenterLogonDlg;
//---------------------------------------------------------------------------
#endif

