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

   // ��p�Ʀ�ƤӪ����ɭԡA�b ViewArea ��������
   // �ҥH��p���I�O 0 ���a��R���A���r��i�H�u�@�I�A����n�\Ū
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
// �Ʀr�ϦV�ƧǡA���F���y���Ǹ�����ƥi�H�b�Ʀr�V�j���ɭԥi�H�b�V�W�h�� VA �W���
// 003  �n�J���\
// 002  �s�u��
// 001  �{���Ұ�
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

