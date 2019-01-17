//---------------------------------------------------------------------------
#ifndef FChgPassDlgH
#define FChgPassDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TChgPassDlg : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelChgPass;
   TLabel *Label7;
   TLabel *Label5;
   TEdit *edNewPass1;
   TEdit *edNewPass2;
   TBitBtn *bbOK;
   TBitBtn *bbCancel;
   TLabel *Label2;
   TEdit *edUser;
   TPanel *PanelHosts;
   TLabel *lbHostPrompt;
   TLabel *lbHostList;
   TBevel *Bevel1;
   void __fastcall bbOKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TChgPassDlg(TComponent* Owner);

   bool __fastcall Execute (const AnsiString& user, const AnsiString& hosts);
};
//---------------------------------------------------------------------------
#endif
