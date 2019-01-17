/**
動態載入lib.
*/
//---------------------------------------------------------------------------
#ifndef DlLibH
#define DlLibH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include <string>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TDlLib
{
#ifndef K_WINDOWS
   typedef void*  HINSTANCE;
#endif
   HINSTANCE      LibInst_;

   ///禁止複製.
   K_ctor  TDlLib (const TDlLib&);
   void operator= (const TDlLib&);
public:
   inline  K_ctor  TDlLib ()  : LibInst_(0) {}
   virtual K_dtor ~TDlLib ()  { FreeLib(); }

   /** 載入lib.
       傳回失敗訊息,若成功則傳回 empty().
   */
   virtual K_mf(std::string) LoadLib (const char* libName);

   /** 釋放lib.
   */
   virtual K_mf(void) FreeLib ();

   /** 取得函式指標.
   */
   typedef void  (*TFuncAds)  ();
   K_mf(TFuncAds) GetFuncAds (const char* procName, bool mustExist);

   /// 有些lib一旦載入就不適合 FreeLib()
   /// 例如: 透過此lib建立了一些物件, 若這些物件持續有效, 則 FreeLib() 會造成 crash!
   /// 此時請呼叫 ReleaseLib(), 呼叫此mf之後, 就無法再取得任何 FuncAds 了!
   inline K_mf(void) ReleaseLib() {
      LibInst_ = 0;
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
