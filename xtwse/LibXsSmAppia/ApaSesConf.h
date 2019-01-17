/////////////////////////////////////////////////////////////////////////////
#if !defined(_ApaSesConf_h_)
#define _ApaSesConf_h_
//---------------------------------------------------------------------------
//#include "char_ary.h"
//#include "ev_map.h"
//#include "ViewAreaMgr.h"
#include "ApaSmpType.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
//typedef Kstl::char_ary<char, 20, 0>  TAppiaConfKey;
   

/////////////////////////////////////////////////////////////////////////////
// struct SAppiaConf
/////////////////////////////////////////////////////////////////////////////
struct SAppiaConf
{
   TSmpString  MiddlewareID_;
   TSmpInt     ReliableMode_;
};
/*
struct SAppiaConfBase
{
   Kstl::char_ary<char, 20, 0>  MiddlewareID_;
   Kstl::char_ary<char,  5, 0>  ReliableMode_;
};
//---------------------------------------------------------------------------
struct SAppiaConf : SAppiaConfBase
{
   Kstl::char_ary<char, 128-sizeof(SAppiaConfBase), 0>  Filler_;
};
*/


/////////////////////////////////////////////////////////////////////////////
// class TAppiaConf
/////////////////////////////////////////////////////////////////////////////
/*
K_class TAppiaConf
{
public:
   typedef Kstl::ev_map<TAppiaConfKey, SAppiaConf>  TRecs;
   typedef TRecs::iterator                          TRecsI;
   typedef TRecs::updater                           TRecsU;

public:
   K_ctor  TAppiaConf ();
   K_dtor ~TAppiaConf ();

   K_mf(const SAppiaConf*) FindRec (const TAppiaConfKey&);
   K_mf(Kway::TVACreater&) GetVAC  ();

   inline K_mf(TRecs*) GetRecs ()  { return &Recs_; }

   static K_mf(TAppiaConf&) GetInst ();

private:
   class TRecsRecorder;
   class TRecsVAC;

   TRecs           Recs_;
   TRecsRecorder*  RecsRecorder_;
   TRecsVAC*       RecsVAC_;
};
*/

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_ApaSesConf_h_)
