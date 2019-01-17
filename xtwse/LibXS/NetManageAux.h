/*

  提供給 .NET Manage 環境的輔助類別

*/

#ifndef NetManageAuxH
#define NetManageAuxH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include <string>
//---------------------------------------------------------------------------
namespace Kstl {
   //提供給 managed 環境模擬 std::string 使用
   //因為 #pragma make_public(不能用template)
   LIBNS_class string : public std::string {
      typedef std::string  base;
   public:
      inline K_ctor string()                        : base()    {}
      inline K_ctor string(const char* str)         : base(str) {}
      inline K_ctor string(const std::string& str)  : base(str) {}

      #if defined(_MSC_VER)
         operator std::string() {
            return *this;
         }
      #endif
   };
} // namespace Kstl
//---------------------------------------------------------------------------
#endif