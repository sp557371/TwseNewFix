//---------------------------------------------------------------------------
#ifndef FIPCGroupPropEditorH
#define FIPCGroupPropEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
class TIPCGroupPropEditor : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelCtrl;
   TPanel *PanelProp;
   TLabel *lbDailyTime;
   TDateTimePicker *edDailyTime;
   TCheckBox *cbStartDate;
   TDateTimePicker *edStartDate;
   TLabel *lbStartDate;
   TCheckBox *cbEndDate;
   TDateTimePicker *edEndDate;
   TLabel *lbEndDate;
   TBitBtn *bbOK;
   TBitBtn *bbCancel;
   void __fastcall cbStartDateClick(TObject *Sender);
   void __fastcall cbEndDateClick(TObject *Sender);
   void __fastcall bbOKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TIPCGroupPropEditor(TComponent* Owner);

   bool __fastcall Execute (const std::string& groupName, std::string& result);
};
//---------------------------------------------------------------------------
extern PACKAGE TIPCGroupPropEditor *IPCGroupPropEditor;
//---------------------------------------------------------------------------
#endif
