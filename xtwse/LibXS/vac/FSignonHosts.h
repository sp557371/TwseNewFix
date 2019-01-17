//---------------------------------------------------------------------------
#ifndef FSignonHostsH
#define FSignonHostsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
/////////////////////////////////////////////////////////////////////////////
#include "pod_vector.h"
namespace Kway { namespace Signon {
class TSignonClient;
} }
typedef Kstl::pod_vector<Kway::Signon::TSignonClient*>   TSignonClients;
//---------------------------------------------------------------------------
class TSignonHosts : public TForm
{
__published:
   TPanel *PanelControls;
   TBitBtn *bbSignonAll;
   TBitBtn *bbClose;
   TBitBtn *bbSavePass;
   TBitBtn *bbSaveSetup;
   TBitBtn *bbSignoutAll;
   TScrollBox *ScrollBox;
   TPanel *Panel;
   TLabel *Label2;
   TLabel *Label5;
   TLabel *lbMessage;
   TPanel *lbHostName;
   TComboBox *cbSameAs;
   TPanel *PanelUser;
   TLabel *lbUser;
   TLabel *Label7;
   TPanel *PanelUserGroup;
   TLabel *Label8;
   TComboBox *cbGroups;
   TEdit *edUser;
   TEdit *edPass;
   TBitBtn *bbSignon;
   TBitBtn *bbChgPass;
   TBitBtn *bbSignout;
   TBitBtn *bbClear;
   void __fastcall PanelUserEnter(TObject *Sender);
   void __fastcall PanelUserExit(TObject *Sender);
   void __fastcall cbSameAsChange(TObject *Sender);
private:
public:
   __fastcall TSignonHosts(TComponent* Owner);

   bool __fastcall Execute (const TSignonClients&, const String& iniSect);
};
/////////////////////////////////////////////////////////////////////////////
#endif

