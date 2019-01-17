/////////////////////////////////////////////////////////////////////////////
#if !defined(_QFixStatusViewer_h_)
#define _QFixStatusViewer_h_
//---------------------------------------------------------------------------
#include "KFile.h"
#include "Nsa_GlobObjs.h"
#include "Nsa_Time.h"
#include "QFixType.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TVACreater;
   using namespace Kstl;
   using namespace Nsa;

namespace FIX  {

/////////////////////////////////////////////////////////////////////////////
// class TFixStatusViewer
/////////////////////////////////////////////////////////////////////////////
K_class TFixStatusViewer : public TGlobObj
{
public:
   struct SFsvKey
   {
      SessionID  SessionID_;
      TKDate     TradeDate_;

      inline K_ctor SFsvKey ()                                                                          {}
      inline K_ctor SFsvKey (const SessionID& s, const TKDate& d) : SessionID_(s), TradeDate_(d)  {}

      // 暫時沒用到
      inline K_mf(void) assign (const std::string&)  {}

      K_mf(bool) operator < (const SFsvKey& a) const
      {
         if (SessionID_ < a.SessionID_)
            return true;
         else if (SessionID_ == a.SessionID_) {
            if (TradeDate_ < a.TradeDate_)
               return true;
         }
         return false;
      }
   };

   struct SFsvDat
   {
      typedef fixed_num<int>  TNsInt;

      TTimeStamp    MsnUpdTime_;
      TNsInt        NextSenderMsgSeqNum_;
      TNsInt        NextTargetMsgSeqNum_;
      std::string   FixSesDesc_;
      TFixSesInfo*  FixSesInfo_;

      inline K_ctor SFsvDat ()
         : FixSesInfo_(NULL)
      {}
   };

   typedef ev_map<SFsvKey, SFsvDat>  TRecs;
   typedef TRecs::iterator           TRecsI;
   typedef TRecs::updater            TRecsU;

   //==============================================
   // class TUpdImpl
   class TUpdImpl : public TFixSsuBase
   {
      TRecs*  Recs_;
      TRecsI  Iter_;
      TRecsU  Upd_;

   public:
      K_ctor TUpdImpl (TRecs* recs, TRecsI& iter);

      //===========================================
      // overwrite TFixSsuBase virtual function
      K_mf(void) SenderMsgSeqNumChange (int oldVal, int newVal);
      K_mf(void) TargetMsgSeqNumChange (int oldVal, int newVal);
      K_mf(void) SetDesc               (const char*);
   };

public:
   K_ctor  TFixStatusViewer ();
   K_dtor ~TFixStatusViewer ();

   K_mf(bool) RegSesInfo (TFixSesInfo*);
   K_mf(void) DelSesInfo (TFixSesInfo*);

   K_mf(TVACreater&) GetVAC ();

private:
   class TRecsVAC;

   TRecs       Recs_;
   TRecsVAC*   RecsVAC_;
};
/////////////////////////////////////////////////////////////////////////////
// class TFixStatusViewerGoh
/////////////////////////////////////////////////////////////////////////////
class TFixStatusViewerGoh : public TGlobObjHnd<TFixStatusViewer>
{
   typedef TGlobObjHnd<TFixStatusViewer>  inherited;

public:
   inline K_ctor TFixStatusViewerGoh (bool autoQuery = false)
      : inherited("FIX::FixStatusViewer", autoQuery)
   {}
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_QFixStatusViewer_h_)

