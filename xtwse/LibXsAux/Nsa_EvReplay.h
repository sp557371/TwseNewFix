/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvReplay_h_)
#define _Nsa_EvReplay_h_
//---------------------------------------------------------------------------
#include "KFile.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// struct SEvTrkHdr
/////////////////////////////////////////////////////////////////////////////
struct SEvTrkHdr
{
   char        FstCode_;   // 'k'
   Kway::byte  EvID_;
   Kway::byte  SubEvID_;
   Kway::word  Length_;

   inline K_ctor SEvTrkHdr ()
      : FstCode_('k')
      , EvID_   (0)
      , SubEvID_(0)
      , Length_ (0)
   {}

   inline K_mf(Kway::byte*) GetChkSumBeg ()  { return &EvID_; }
};
/////////////////////////////////////////////////////////////////////////////
// struct SEvTrk
/////////////////////////////////////////////////////////////////////////////
struct SEvTrk : public SEvTrkHdr
{
   typedef SEvTrkHdr  inherited;

   char  Data_[5120];

   K_mf(Kway::word) Pack (Kway::byte evID, Kway::byte subEvID, const void* buffer, Kway::word len);

   inline K_mf(char*) GetData ()  { return Data_; }
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TEvTrack
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TEvTrack
{
   TFile  File_;

public:
   K_mf(bool) OpenFile   (const char*);
   K_mf(void) WriteEvent (Kway::byte evID, Kway::byte subEvID, const void* buffer, Kway::word len);
};


/////////////////////////////////////////////////////////////////////////////
// class TEvReplay
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TEvReplay
{
public:
};





/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvReplay_h_)
