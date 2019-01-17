/**
�ʺA���Jlib.
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

   ///�T��ƻs.
   K_ctor  TDlLib (const TDlLib&);
   void operator= (const TDlLib&);
public:
   inline  K_ctor  TDlLib ()  : LibInst_(0) {}
   virtual K_dtor ~TDlLib ()  { FreeLib(); }

   /** ���Jlib.
       �Ǧ^���ѰT��,�Y���\�h�Ǧ^ empty().
   */
   virtual K_mf(std::string) LoadLib (const char* libName);

   /** ����lib.
   */
   virtual K_mf(void) FreeLib ();

   /** ���o�禡����.
   */
   typedef void  (*TFuncAds)  ();
   K_mf(TFuncAds) GetFuncAds (const char* procName, bool mustExist);

   /// ����lib�@�����J�N���A�X FreeLib()
   /// �Ҧp: �z�L��lib�إߤF�@�Ǫ���, �Y�o�Ǫ�����򦳮�, �h FreeLib() �|�y�� crash!
   /// ���ɽЩI�s ReleaseLib(), �I�s��mf����, �N�L�k�A���o���� FuncAds �F!
   inline K_mf(void) ReleaseLib() {
      LibInst_ = 0;
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
