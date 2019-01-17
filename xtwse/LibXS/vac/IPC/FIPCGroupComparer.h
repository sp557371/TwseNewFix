//---------------------------------------------------------------------------
#ifndef FIPCGroupComparerH
#define FIPCGroupComparerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "FIPCGroupDiff.h"
//---------------------------------------------------------------------------
class TIPCGroupComparer : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelCtrl;
   TPanel *PanelDiff;
   TComboBox *cbHist1;
   TLabel *Label1;
   TComboBox *cbHist2;
   TPanel *PanelCurrent;
   void __fastcall cbHistChange(TObject *Sender);
private:	// User declarations
   TIPCGroupDiff* IPCGroupDiff_;
public:		// User declarations
   __fastcall TIPCGroupComparer(TComponent* Owner);

   void __fastcall Execute ();
};
//---------------------------------------------------------------------------
#endif
