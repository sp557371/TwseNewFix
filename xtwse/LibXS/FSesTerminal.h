//---------------------------------------------------------------------------
#ifndef FSesTerminalH
#define FSesTerminalH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "KFile.h"
//---------------------------------------------------------------------------
typedef int __fastcall (__closure* TOnDataSend) (const void*, size_t);
namespace Kway {
   class TMesBase;
}
//---------------------------------------------------------------------------
class TSesTerminalDlg : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TPanel *Panel2;
   TEdit *edSend;
   TButton *bbSend;
   TMemo *Memo;
   TLabel *lbLinkReady;
   TLabel *lbSendEmpty;
   TCheckBox *cbCrLfBlack;
   TLabel *lbSendBytes;
   TButton *bbSendFile;
   TEdit *edFileSend;
   TButton *bbRecvFile;
   TLabel *Label1;
   TButton *bbEndSend;
   TLabel *lbRecvFileSize;
   TButton *bbClearMemo;
   TComboBox *cbRecvShowSel;
   TLabel *Label2;
   TCheckBox *cbRepeat;
   TButton *bbSendOnce;
   TLabel *Label3;
   TEdit *edFileRecv;
   TButton *bbEndRecv;
   TLabel *Label4;
   TComboBox *cbSendAppend;
   TCheckBox *cbAutoUnlink;
   void __fastcall bbSendClick(TObject *Sender);
   void __fastcall bbSendFileClick(TObject *Sender);
   void __fastcall bbRecvFileClick(TObject *Sender);
   void __fastcall bbEndSendClick(TObject *Sender);
   void __fastcall bbClearMemoClick(TObject *Sender);
   void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
   void __fastcall bbSendOnceClick(TObject *Sender);
   void __fastcall bbEndRecvClick(TObject *Sender);
   void __fastcall edSendKeyPress(TObject *Sender, char &Key);
   void __fastcall cbAutoUnlinkClick(TObject *Sender);
private:
   TOnDataSend    OnDataSend_;
   size_t         SendBytes_;
   Kway::TFile    RecvFile_;
   Kway::TFile    SendFile_;
   bool           FileSending_;
   size_t         SendEmptyCounter_;
   TTimer*        Timer_;
   Kway::TMesBase* Mes_;
   void __fastcall OnTimer(TObject* Sender);

   void __fastcall SetOnDataSend (TOnDataSend);
   void __fastcall CheckSendFile ();
   bool __fastcall OpenFile      (const char* fileName, Kway::TFile&, Kway::TFileMode);
public:
        __fastcall TSesTerminalDlg  (TComponent* Owner);
        __fastcall~TSesTerminalDlg  ();
   void __fastcall DataRecv         (char*, size_t);
   void __fastcall OnSendEmpty      (Kway::TMesBase*);
   void __fastcall OnLinkReady      (Kway::TMesBase*);

   __property TOnDataSend  OnDataSend = { read = OnDataSend_, write = SetOnDataSend };
};
//---------------------------------------------------------------------------
#define clYes  clBlack
#define clNo   clGray
//---------------------------------------------------------------------------
#endif

