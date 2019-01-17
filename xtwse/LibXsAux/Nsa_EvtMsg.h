/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvtMsg_h_)
#define _Nsa_EvtMsg_h_
//---------------------------------------------------------------------------
#include <cc++/thread.h>
#include "ev_map.h"
#include "KTime.h"
#include "KFile.h"
#include "ViewAreaMgr.h"
#include "Nsa_GlobObjs.h"
#include "tplns/Nsa_FixedNum.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// base type
/////////////////////////////////////////////////////////////////////////////
typedef TDescNum<Kway::uint32>  TEmID;
typedef std::string             TEmGrpName;
typedef std::string             TEmGrpSubName;
typedef std::string             TEmStr;

/////////////////////////////////////////////////////////////////////////////
// class TEvtMsgKind
/////////////////////////////////////////////////////////////////////////////
enum EEvtMsgKind
{
   emk_None = 0x00,
      emk_Info       = 0x01,
      emk_Warning    = 0x02,
      emk_Error      = 0x04,

      // 因為有些訊息太多了，會佔用許多記憶體
      // 但又需要記錄起來，所以只會寫入 Txt File，不會放到資料表裡面
      emk_OnlyToTxt  = 0x08,

      emk_TxtInfo    = (emk_Info    | emk_OnlyToTxt),
      emk_TxtWarning = (emk_Warning | emk_OnlyToTxt),
      emk_TxtError   = (emk_Error   | emk_OnlyToTxt),
};
//---------------------------------------------------------------------------
LIBNSAUX_class TEvtMsgKind
{
   EEvtMsgKind  Data_;

public:
   K_ctor TEvtMsgKind () : Data_(emk_None)  {}

   K_mf(void)        assign    (const char* str);
   K_mf(std::string) as_string () const;

   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) assignT (EEvtMsgKind Data)        { Data_ = Data;        }
};

/////////////////////////////////////////////////////////////////////////////
// struct SEmAll
/////////////////////////////////////////////////////////////////////////////
struct SEmAll
{
   TKDateTimeMS   MsgTime_;
   TEmGrpName     GrpName_;
   TEmGrpSubName  GrpSubName1_;
   TEmGrpSubName  GrpSubName2_;
   TEmStr         MsgStr_;
   TEvtMsgKind    MsgKind_;
};

/////////////////////////////////////////////////////////////////////////////
// struct SEmGrpCont
/////////////////////////////////////////////////////////////////////////////
struct SEmGrpCont
{
   TKDateTimeMS   MsgTime_;
   TEmGrpSubName  GrpSubName_;
   TEmStr         MsgStr_;
   TEvtMsgKind    MsgKind_;
};


/////////////////////////////////////////////////////////////////////////////
// class TEvtMsg
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TEvtMsg : public TGlobObj
{
public:
   //==============================================
   // TRecsType
   typedef ev_map<TEmID, SEmAll>  TRecs;
   typedef TRecs::iterator        TRecsI;
   typedef TRecs::updater         TRecsU;

private:
   class TRecsRecorder;
   class TRecsVAC;
   
   TRecs           Recs_;
   TRecsRecorder*  RecsRecorder_;
   TRecsVAC*       RecsVAC_;
   TEmID           NewEmID_;
   TViewAreaMgr    VAMgr_;
   TFile           TxtFile_;
   ost::Mutex      Critical_;

public:
   K_ctor  TEvtMsg ();
   K_dtor ~TEvtMsg ();

   K_mf(void)        Reload (const char* filePath, const char* fileName);
   K_mf(TVACreater&) GetVAC ();

   inline K_mf(void) AddMsg (EEvtMsgKind msgKind, const TEmGrpName& grpName, const char* msgStr)
      { addMsg(msgKind, grpName.c_str(), NULL, NULL, msgStr); }
         
   inline K_mf(void) AddMsg (EEvtMsgKind msgKind, const TEmGrpName& grpName, const TEmGrpSubName& grpSubName1, const char* msgStr)
      { addMsg(msgKind, grpName.c_str(), grpSubName1.c_str(), NULL, msgStr); }

   inline K_mf(void) AddMsg (EEvtMsgKind msgKind, const TEmGrpName& grpName, const TEmGrpSubName& grpSubName1, const TEmGrpSubName& grpSubName2, const char* msgStr)
      { addMsg(msgKind, grpName.c_str(), grpSubName1.c_str(), grpSubName2.c_str(), msgStr); }

protected:
   K_mf(void) addMsg (EEvtMsgKind, const char* grpName, const char* grpSubName1, const char* grpSubName2, const char* msgStr);

public:
   static const TEmGrpName  SysSts;
   static const TEmGrpName  DbgSts;
   static const TEmGrpName  ExptSts;
   static const TEmGrpName  SesSts;
};

/////////////////////////////////////////////////////////////////////////////
// class TEvtMsgGoh
/////////////////////////////////////////////////////////////////////////////
class TEvtMsgGoh : public TGlobObjHnd<TEvtMsg>
{
   typedef TGlobObjHnd<TEvtMsg>  inherited;
   
public:
   inline K_ctor TEvtMsgGoh (bool autoQuery = false)
      : inherited("Kway::Nsa::TEvtMsg", autoQuery)
   {}
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvtMsg_h_)
