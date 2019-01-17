/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_Per_h_)
#define _Nsa_Per_h_
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "Nsa_Time.h"
#include "kpersistent.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kstl {
   using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// 我自己定義的 persistent_save persistent_load
/////////////////////////////////////////////////////////////////////////////
struct persistent_save_nsa
{
   template <class DestType>
   static inline K_mf(bool) store (persistent& per, const DestType& a)
      { return psave(per, a); }

   static inline K_mf(bool) store (persistent& per, const void* ptr, unsigned sz)
      { return per.save(ptr, sz, false); }
};
//---------------------------------------------------------------------------
struct persistent_load_nsa
{
   template <class DestType>
   static inline K_mf(bool) store (const persistent& per, DestType& a)
      { return pload(per, a); }
   
   static inline K_mf(bool) store (const persistent& per, void* ptr, unsigned sz)
      { return per.load(ptr, sz, false); }
};

/////////////////////////////////////////////////////////////////////////////
// char_ary1
/////////////////////////////////////////////////////////////////////////////
template <class CharT, CharT NullChar>
inline K_mf(bool) pload (persistent& p, char_ary1<CharT, 1>& a)
   { return p.loadBytes(a); }

template <class CharT, CharT NullChar>
inline K_mf(bool) psave (persistent& p, const char_ary1<CharT, 1>& a)
   { return p.saveBytes(a); }

/////////////////////////////////////////////////////////////////////////////
// TTimeStamp
/////////////////////////////////////////////////////////////////////////////
inline bool pload (const Kstl::persistent& p, TTimeStamp& a)
   {
      Kway::TKTime t;
      if (!p.loadBytes(t))
         return false;
      
      Kway::TKTimeMS ms;
      if (!p.loadHiLoTrans(ms))
         return false;
         
      a.SetTime  (t);
      a.SetTimeMs(ms);
      return true;
   }
   
inline bool psave (Kstl::persistent& p, const TTimeStamp& a)
   {
      Kway::TKTime t = a.GetTime();
      return p.saveBytes(t) && p.saveHiLoTrans(a.GetTimeMS().GetMS());
   }

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kstl

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_Per_h_)
