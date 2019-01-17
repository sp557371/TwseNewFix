//
// 通訊classes基礎: Session-Messager
//    class TSesTerminal;
//    => 產生一個 Terminal 對話視窗, 可傳送及接收資料
//
// 版本: $Id: SesTerminal.h,v 1.3 2005/12/28 08:47:42 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesTerminalH
#define SesTerminalH
//---------------------------------------------------------------------------
#include "Ses.h"
//---------------------------------------------------------------------------
//#include "FSesTerminal.h"
class TSesTerminalDlg;
namespace System {
class TObject;
};
#if !defined(FormsHPP)
namespace Forms {
#pragma option push -b-
enum TCloseAction { caNone, caHide, caFree, caMinimize };
#pragma option pop
};
#endif
//---------------------------------------------------------------------------
namespace Kway {
class TSesReg; //in SesMgr.h
/////////////////////////////////////////////////////////////////////////////
class TSesTerminal : public TSesBase
{
   typedef TSesBase  inherited;
   K_mf(void)  OnMesLinkReady    (TMesBase*);
   K_mf(void)  OnMesLinkBroken   (TMesBase*, const std::string& reason);
   K_mf(void)  OnMesLinkFail     (TMesBase*, const std::string& reason);
   K_mf(void)  OnMesRecvReady    (TMesBase*, size_t);
   K_mf(void)  OnMesStateChanged (TMesBase*, TMesStateLevel, const std::string&);
   K_mf(void)  OnMesSendEmpty    (TMesBase*);
   K_mf(void)  OnMesLeave        (TMesBase*);
   K_mf(void)  OnMesEnter        (TMesBase*);

   K_mf(bool)  SettingApply     (const std::string&);

   std::auto_ptr<TSesTerminalDlg>   Dlg_;
   void __fastcall OnFormClose  (System::TObject*, Forms::TCloseAction&);

   bool  Opening_;
public:
   K_ctor  TSesTerminal  ();
   K_dtor ~TSesTerminal  ();

   static K_mf(const TSesReg&)  GetReg  ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
