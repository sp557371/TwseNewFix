//----------------------------------------------------------------------------
#ifndef FDoubleCheckerH
#define FDoubleCheckerH
//----------------------------------------------------------------------------
#include <vcl\Buttons.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
//----------------------------------------------------------------------------
class TDoubleCheckerDlg : public TForm
{
__published:
	TLabel *Label1;
   TEdit *edPass;
	TButton *OKBtn;
	TButton *CancelBtn;
   TLabel *Label2;
   TEdit *edUser;
private:
public:
	virtual __fastcall TDoubleCheckerDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
#endif
