//
// vac - ViewArea Client 主程式 for BCB
//
// 版本: $Id: FMainVAC.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef FMainVACH
#define FMainVACH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
//#include "FevcViewerMgr.h"
class PACKAGE TevcViewerMgr;
class PACKAGE TBaseVAC;
//---------------------------------------------------------------------------
class TMainVAC : public TForm
{
__published:	// IDE-managed Components
   TToolBar *ToolBar1;
   TActionList *ActionList1;
   TAction *aSetting;
   TAction *aLogon;
   TSpeedButton *sbSetting;
   TSpeedButton *sbLogon;
   TStatusBar *StatusBar;
   TEdit *edCmd;
   TAction *aSignout;
   TSpeedButton *sbSignout;
   TPageControl *pages;
   TTabSheet *tsMain;
   TTabSheet *tsSesStatus;
   void __fastcall aSettingExecute(TObject *Sender);
   void __fastcall aLogonExecute(TObject *Sender);
   void __fastcall aSignoutExecute(TObject *Sender);
private:	// User declarations
   TBaseVAC*   BaseVAC_;
   TForm*      SesStatusForm_;

   class TSignonAckEv;
   TSignonAckEv*  SignonAckEv_;

public:		// User declarations
   __fastcall  TMainVAC  (TComponent* Owner);
   __fastcall ~TMainVAC  ();

   TBaseVAC* GetBaseVAC()   { return BaseVAC_; }
};
//---------------------------------------------------------------------------
extern PACKAGE TMainVAC *MainVAC;
//---------------------------------------------------------------------------
#endif
