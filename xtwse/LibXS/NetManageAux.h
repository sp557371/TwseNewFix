/*

  ���ѵ� .NET Manage ���Ҫ����U���O

*/

#ifndef NetManageAuxH
#define NetManageAuxH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include <string>
//---------------------------------------------------------------------------
namespace Kstl {
   //���ѵ� managed ���Ҽ��� std::string �ϥ�
   //�]�� #pragma make_public(�����template)
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