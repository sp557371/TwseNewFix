/////////////////////////////////////////////////////////////////////////////
#if !defined(_ApaSesSmpFix_h_)
#define _ApaSesSmpFix_h_
//---------------------------------------------------------------------------
#include "ApaSesSmp.h"
#include "src/Code/Message.h"
#include "QFixExt/QFixTagType.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace FIX;

/////////////////////////////////////////////////////////////////////////////
// struct SDispFixMsgInfo
/////////////////////////////////////////////////////////////////////////////
struct SDispFixMsgInfo
{
   const TSmpString&    SesID_;
   const TFixMsgType2&  MsgType_;
   TMessageRef          FixMsgRef_;
   bool                 IsRt_;
   TSmpMsgRef           SmpMsgRef_;

   inline K_ctor SDispFixMsgInfo (const TSmpString& sesID, const TFixMsgType2& msgType, TMessageRef& fixMsgRef,
                                  bool              isRt,  const TSmpMsgRef&   smpMsgRef)
      : SesID_    (sesID)
      , MsgType_  (msgType)
      , FixMsgRef_(fixMsgRef)
      , IsRt_     (isRt)
      , SmpMsgRef_(smpMsgRef)
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpFix
/////////////////////////////////////////////////////////////////////////////
K_class TApaSesSmpFix : public TApaSesSmp
{
   typedef TApaSesSmp  inherited;

protected:
   bool  IsAutoFlush_;  // TApaFixRecover::TApaFixSeqNoFile 在 write 後是否要 flush

public:
   K_ctor  TApaSesSmpFix ();
   K_dtor ~TApaSesSmpFix ();

   //==============================================
   // TApaSesSmpFix virtual function
   virtual K_mf(void) DispFixMessage (SDispFixMsgInfo&) = 0;

   //==============================================
   // TApaSesSmpFix function
   inline K_mf(bool) IsAutoFlush ()  { return IsAutoFlush_; }

protected:
   //==============================================
   // overwrite TApaSesSmp virtual function
   // 這邊不主動處理,改由繼承者來發動
   //K_mf(void) OnSmpLogon ();

   //==============================================
   // TApaSesSmpFix virtual function

   //==============================================
   // TApaSesSmpFix function
   K_mf(TFixMsgType2) GetFixMsgType (const SSmpMsgMiddlewareEvent&, TMessageRef, const char* caller);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_ApaSesSmpFix_h_)
