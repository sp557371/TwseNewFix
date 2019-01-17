//---------------------------------------------------------------------------
#ifndef FMainVARMH
#define FMainVARMH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
#include "KwayBase.h"
namespace Kway {
   class TSesMgr;
}
class TevcViewer;
//---------------------------------------------------------------------------
class TMainVARM : public TForm
{
__published:	// IDE-managed Components
   TActionList *ActionList;
   TAction *aLogon;
   TAction *aSetup;
   TImageList *ImageListEn;
   TImageList *ImageListDis;
   TAction *aExit;
   TAction *aSesStatus;
   TMainMenu *MainMenu;
   TMenuItem *miFile;
   TMenuItem *miExit;
   TMenuItem *miLogon;
   TMenuItem *miSetup;
   TMenuItem *miSesStatus;
   TMenuItem *miHosts;
   TMenuItem *miWindows;
   TAction *aHostEv;
   TMenuItem *miHostEv;
   void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
   void __fastcall aExitExecute(TObject *Sender);
   void __fastcall aLogonExecute(TObject *Sender);
   void __fastcall aSetupExecute(TObject *Sender);
   void __fastcall aSesStatusExecute(TObject *Sender);
   void __fastcall aHostEvExecute(TObject *Sender);
protected:
	DYNAMIC void __fastcall DoShow(void);
private:	// User declarations
   Kway::TSesMgr* SesMgr_;
   TevcViewer*    SesStatusForm_;

   class TSesVACCreater;
   TSesVACCreater*   SesVACCreater_;

   void __fastcall OnChildCloseHide (TObject* Sender, TCloseAction &Action);
   void __fastcall AfterSessionsSetting ();
public:		// User declarations
   __fastcall  TMainVARM(TComponent* Owner);
   __fastcall ~TMainVARM();
};
//---------------------------------------------------------------------------
extern PACKAGE TMainVARM *MainVARM;
//---------------------------------------------------------------------------
#endif

