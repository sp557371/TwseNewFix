//---------------------------------------------------------------------------
#ifndef FBaseVACH
#define FBaseVACH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "SesVAC.h"
class PACKAGE TevcViewerMgr;
//---------------------------------------------------------------------------
class TBaseVAC : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelVAC;
   TSplitter *Splitter1;
   TPanel *VAPanel;
   TPanel *PanelHost;
   TTreeView *HostTree;
   void __fastcall HostTreeChange(TObject *Sender, TTreeNode *Node);
   void __fastcall HostTreeDeletion(TObject *Sender, TTreeNode *Node);
   void __fastcall HostTreeExpanding(TObject *Sender, TTreeNode *Node,
          bool &AllowExpansion);
   void __fastcall Splitter1Moved(TObject *Sender);
private:	// User declarations
   AnsiString     IniSect_;
   TevcViewerMgr* Viewer_;
   TTreeNode*     ViewerNode_;
   int            HostTreeWidth_;
public:		// User declarations
   __fastcall  TBaseVAC(TComponent* Owner, const AnsiString& iniSect);
   __fastcall ~TBaseVAC();

   inline TevcViewerMgr* __fastcall GetViewer() {
      return Viewer_;
   }

   void __fastcall AddSes   (Kway::Pk::TSesVAC*, bool isSesOwner);
   void __fastcall ClearVAC ();
};
//---------------------------------------------------------------------------
#endif

