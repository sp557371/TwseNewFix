//---------------------------------------------------------------------------
#ifndef PkMLogH
#define PkMLogH
//---------------------------------------------------------------------------
#include "SesPk.h"
//---------------------------------------------------------------------------
// MLog傳輸協定 (底層採用:PkFT)
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
static const TMsgCode   mcSesMLog = 0x040;
/////////////////////////////////////////////////////////////////////////////
class TSesMLogConn : public THead
{
   typedef TSesMLogConn    this_type;

public:
   //----- for client -----
   inline K_ctor  TSesMLogConn  ()
            { SetMsgCode(mcSesMLog);
              SetPkSize(GetExpectSize());
            }

   //----- for server -----
   static inline K_mf(TSize) GetMinSize    ()        { return sizeof(this_type); }
   static inline K_mf(TSize) GetExpectSize ()        { return sizeof(this_type); }
};
//---------------------------------------------------------------------------
class TSesMLogConnAck : public THead
{
   typedef TSesMLogConnAck this_type;
   typedef TPkRec          TFiller;

   dword    ApID_;
   word     DatLen_;
   union {
      char     MLogFileName_[];
      TFiller  Filler_;
   };
public:
   static const dword   InvalidApID = static_cast<dword>(-1);
   //----- for server -----
   inline K_ctor  TSesMLogConnAck   (dword apID, const std::string& fileName)
            : ApID_(apID)
            , DatLen_(static_cast<TSize>(fileName.size()))
            { memcpy(MLogFileName_, fileName.c_str(), DatLen_);
              SetMsgCode(mcSesMLog);
              SetPkSize(static_cast<TSize>(sizeof(*this) - sizeof(TFiller) + DatLen_));
            }

   //----- for client -----
   inline K_mf(dword)        GetApID          (THiLoTransRef ref) const    { return HiLoTransT(ApID_,ref); }
   inline K_mf(std::string)  GetMLogFileName  (THiLoTransRef ref) const    { return std::string(MLogFileName_, HiLoTransT(DatLen_,ref)); }

   static inline K_mf(TSize) GetMinSize    ()                         { return sizeof(this_type) - sizeof(TFiller); }
   inline K_mf(TSize)        GetExpectSize (THiLoTransRef ref) const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + HiLoTransT(DatLen_,ref)); }
};
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

