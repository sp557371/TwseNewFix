//---------------------------------------------------------------------------
#ifndef FIPCGroupsVAH
#define FIPCGroupsVAH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
/////////////////////////////////////////////////////////////////////////////
#include "FIPCGroupVAForm.h"
class TIPCGroupMgr;
/////////////////////////////////////////////////////////////////////////////
class TIPCGroupsVA : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelIPCGroupsVA;
   TPanel *PanelButtons;
   TBitBtn *btnSetting;
   TBitBtn *btnGroupExec;
   TPanel *PanelSettingRollback;
   TLabel *Label1;
   TComboBox *cbRollback;
   TButton *bbCompare1;
   TBitBtn *bbCompare;
   void __fastcall bbSettingClick(TObject *Sender);
   void __fastcall btnGroupExecClick(TObject *Sender);
   void __fastcall cbRollbackChange(TObject *Sender);
   void __fastcall bbCompareClick(TObject *Sender);
private:	// User declarations
   TIPCGroupMgr*     IPCGroupMgr_;
   TIPCGroupVAForm*  Viewer_;
   TIPCGroupVAForm*  SettingDlg_;
   TIPCGroupVAForm*  EnabledDlg_;

   void __fastcall OnSettingRestore  (TObject*);
   int  __fastcall SetRollbackGroups (TComboBox*, AnsiString opcode);
public:		// User declarations
   __fastcall  TIPCGroupsVA(TIPCGroupMgr* Owner);
   __fastcall ~TIPCGroupsVA();
};
//---------------------------------------------------------------------------
#endif

