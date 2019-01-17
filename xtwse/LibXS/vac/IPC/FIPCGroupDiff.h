//---------------------------------------------------------------------------
#ifndef FIPCGroupDiffH
#define FIPCGroupDiffH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <OleCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#include "SHDocVw_OCX.h"
#include "IPCGroupProcess.h"
#include "FIPCGroupsVA.h"
#include "Signon/SignonClient.h"
//---------------------------------------------------------------------------
class TIPCGroupDiff : public TForm
{
__published:	// IDE-managed Components
   TPanel *PanelResult;
   TWebBrowser *WebBrowser_;
   TPanel *PanelCtrl;
   TLabel *lbDoubleChecker;
   TLabel *Label1;
   TEdit *edDoubleChecker;
   TEdit *edCheckerPassword;
   TBitBtn *bbCancel;
   TBitBtn *bbApply;
   void __fastcall bbApplyClick(TObject *Sender);
   void __fastcall edDoubleCheckerChange(TObject *Sender);
private:	// User declarations
   class TSignonEvHandler : public Kway::Signon::TSignonAckEv {
      TForm*                        Owner_;
      Kway::Signon::TSignonClient*  Client_;

      K_mf(void) OnSignonResult        (Kway::Signon::TSignonClient&, const std::string& result) {}
      K_mf(void) OnChkTableRightResult (Kway::Signon::TSignonClient&, const std::string& user, const std::string& tableName, const std::string& result) {
         if(result.empty())
            Owner_->ModalResult = mrOk;
         else
            Application->MessageBox((char*)(result.c_str()), "審核失敗", MB_OK|MB_ICONERROR);
      }
      K_mf(void) OnStateChanged (Kway::Signon::TSignonClient& client, Kway::Signon::ESignonClientState oldState) {
         if(Client_ == &client && client.GetState() != Kway::Signon::sc_Ready)
            SetClient(NULL);
      }
   public:
      K_ctor TSignonEvHandler (TForm* owner)
         : Owner_(owner)
         , Client_(NULL)
         { }
      K_mf(void) SetClient(Kway::Signon::TSignonClient* client) {
         if(Client_)
            Client_->Del(*this);
         Client_ = client;
         if(Client_)
            Client_->Add(*this);
      }
      K_mf(void) CheckTableRight(const std::string& user, const std::string& pass, const std::string& tableName) {
         if(Client_)
            Client_->CheckTableRight(user, pass, tableName);
         else
            Application->MessageBox("沒有連線,或斷線了!", "審核失敗", MB_OK|MB_ICONERROR);
      }
   };
   TSignonEvHandler  SignonEvHandler_;
public:		// User declarations
   __fastcall TIPCGroupDiff(TComponent* Owner);

   bool __fastcall ShowDiff (const TIPCGroupRecs& newRecs, const TIPCGroupRecs& oldRecs);
   bool __fastcall Execute  (const TIPCGroupRecs& newRecs, const TIPCGroupRecs& oldRecs, Kway::Signon::TSignonClient* client);
};
//---------------------------------------------------------------------------
#endif
