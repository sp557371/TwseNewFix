//---------------------------------------------------------------------------
#ifndef FHostEvDlgH
#define FHostEvDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class THostEvDlg : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelControls;
   TBitBtn *bbClose;
   TMemo *Memo;
   void __fastcall bbCloseClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall THostEvDlg(TComponent* Owner);

   void __fastcall AddEvMsg  (const AnsiString&);
};
//---------------------------------------------------------------------------
extern PACKAGE THostEvDlg* HostEvDlg;
//---------------------------------------------------------------------------
#endif
