/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_FixedNum_h_)
#define _Nsa_FixedNum_h_
//---------------------------------------------------------------------------
#include "fixed_num.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// class TFixedDec
/////////////////////////////////////////////////////////////////////////////
template <class OrgT, unsigned Decimal>
class TFixedDec : public fixed_dec<OrgT, Decimal>
{
   typedef fixed_dec<OrgT, Decimal>  inherited;
   typedef TFixedDec<OrgT, Decimal>  this_type;

public:
   typedef typename inherited::orig_type  orig_type;

public:
   inline          K_ctor TFixedDec ()                           : inherited()         {}
   inline          K_ctor TFixedDec (orig_type n, unsigned decn) : inherited(n, decn)  {}
   inline explicit K_ctor TFixedDec (const char* str)            : inherited(str)      {}
   inline explicit K_ctor TFixedDec (const std::string& s)       : inherited(s)        {}

   // 當小數位數太長的時候，在 ViewArea 中很難看
   // 所以把小數點是 0 的地方刪掉，讓字串可以短一點，比較好閱讀
   K_mf(std::string) as_str () const;

   K_mf(void) assign (float n);

   inline K_mf(void) assign (const char*        str)  { inherited::assign(str); }
   inline K_mf(void) assign (const std::string& str)  { inherited::assign(str); }
   inline K_mf(void) assign (double             val)  { inherited::assign(val); }
      
   inline K_mf(this_type) operator + (const this_type a) const  { return this_type(inherited::get_orig() + a.get_orig(), Decimal); }
   inline K_mf(this_type) operator - (const this_type a) const  { return this_type(inherited::get_orig() - a.get_orig(), Decimal); }
};

/////////////////////////////////////////////////////////////////////////////
// class TDescNum
// 數字反向排序，為了讓流水序號的資料可以在數字越大的時候可以在越上層的 VA 上顯示
// 003  登入成功
// 002  連線中
// 001  程式啟動
/////////////////////////////////////////////////////////////////////////////
template <class OrgT>
class TDescNum : public Kstl::fixed_num<OrgT>
{
   typedef Kstl::fixed_num<OrgT>  inherited;
   typedef TDescNum<OrgT>         this_type;

public:
   inline K_ctor TDescNum ()       : inherited()   {}
   inline K_ctor TDescNum (OrgT n) : inherited(n)  {}
   
   inline K_mf(bool) operator < (const this_type a) const  { return (a.get_orig() < inherited::get_orig()); }
   inline K_mf(bool) operator < (const OrgT      a) const  { return (a            < inherited::get_orig()); }
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TFixedDec impl
/////////////////////////////////////////////////////////////////////////////
#define TFixedDec_Impl(return_type)                                        \
   template <class OrgT, unsigned Decimal>                                 \
   K_mf(return_type) TFixedDec<OrgT, Decimal>::                            \
//---------------------------------------------------------------------------

TFixedDec_Impl(std::string) as_str () const
{
   if (inherited::is_0()) {
      return std::string("");
   }

   char buf[50];
   int  len;

   strcpy(buf, inherited::as_string().c_str());
   len = strlen(buf) - 1;

   for (int i=len; i>=0; --i) {
      if (buf[i] != '0') {
         if (buf[i] == '.') {
            buf[i] = '\0';
         }
         break;
      }
      buf[i] = '\0';
   }

   return std::string(buf);
}
//---------------------------------------------------------------------------

TFixedDec_Impl(void) assign (float n)
{
   orig_type data;

   //#if defined(__NSC__)
   n *= priv::DecimalDivied[inherited::DecN];
   //#else
   //n *= fixed_num_private::DecimalDivied[inherited::DecN];
   //#endif
   n -= (data = static_cast<orig_type>(n));

   if (n > 0.99)
      ++data;
   else if (n < -0.99)
      --data;

   *this = this_type(data, inherited::DecN);
}
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_FixedNum_h_)

