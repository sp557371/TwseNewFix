//---------------------------------------------------------------------------
#ifndef Bool3H
#define Bool3H
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include <string>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
LIBNS_class TBool3 //3種狀態的bool: true,false,unknow
{
   typedef TBool3 this_type;
   enum EBool3 {
      e_false = 0,
      e_true  = 1,
      e_unknow,
   };
   byte  Value_;
public:
            inline K_ctor TBool3 ()                       : Value_(e_unknow) {}
   explicit inline K_ctor TBool3 (bool v)                 : Value_(v)        {}
   explicit inline K_ctor TBool3 (char chr)               { assign(chr); }
   explicit inline K_ctor TBool3 (const std::string& str) { assign(str); }

   inline K_mf(void) operator =  (bool v)  { Value_ = v; }
   inline K_mf(void) assign      (bool v)  { Value_ = v; }
   inline K_mf(void) SetUnknow   ()        { Value_ = e_unknow; }

   /** chr=='Y' or 'y' or 'T' or 't' => 設為 true
          =='N' or 'n' or 'F' or 'f'
            ' ' or 0                 => 設為 false
          其他設為 unknow
   */
          K_mf(void) assign (char chr);
          K_mf(void) assign (const std::string& str);
   /** IsTrue()  傳回 "Y"
       IsFalse() 傳回 "N"
       IsUnknow()傳回 "?"
   */
   K_mf(std::string) as_string () const;

   inline K_mf(bool) operator == (const this_type& a) const { return Value_==a.Value_; }
   inline K_mf(bool) operator == (bool a) const             { return IsTrue()==a;      }
   inline K_mf(bool) IsTrue      () const                   { return Value_==e_true;   }
   inline K_mf(bool) IsFalse     () const                   { return Value_==e_false;  }
   inline K_mf(bool) IsUnknow    () const                   { return Value_==e_unknow; }
};
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
