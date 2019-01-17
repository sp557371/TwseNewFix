//---------------------------------------------------------------------------
#ifndef FIPCSettingDlgH
#define FIPCSettingDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TIPCSettingDlg : public TForm
{
__published:	// IDE-managed Components
   TPanel *plSetting;
   TPanel *plButtons;
   TLabel *lbIPCCenterParams;
   TEdit *edIPCCenterParams;
   TLabel *lbDestNameGroups;
   TEdit *edDestNameGroups;
   TLabel *lbDestNameGroupsExec;
   TEdit *edDestNameGroupsExec;
   TEdit *edDestNameExecResult;
   TLabel *lbDestNameExecResult;
   TEdit *edDestNameAudit;
   TLabel *lbDestNameAudit;
   TLabel *lbDestNameStatus;
   TEdit *edDestNameStatus;
   TBitBtn *btnOK;
   TBitBtn *btnCancel;
   TLabel *lbDestNameGroupsRollback;
   TEdit *edDestNameGroupsRollback;
   TEdit *edDestNameTxFile;
   TLabel *lbDestNameTxFile;
private:	// User declarations
public:		// User declarations
   __fastcall TIPCSettingDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TIPCSettingDlg *IPCSettingDlg;
//---------------------------------------------------------------------------
#endif
