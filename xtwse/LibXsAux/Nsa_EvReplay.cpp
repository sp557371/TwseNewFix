/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_EvReplay.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// struct SEvTrk impl
/////////////////////////////////////////////////////////////////////////////
K_mf(Kway::word) SEvTrk::Pack (Kway::byte evID, Kway::byte subEvID, const void* buffer, Kway::word len)
{
   Kway::word  lenCs     = sizeof(SEvTrkHdr) - 1 + len;
   Kway::byte* chkSum    = (Kway::byte*)(Data_ + len);
   Kway::byte* chkSumBeg = inherited::GetChkSumBeg();
   Kway::byte* chkSumEnd = (chkSumBeg + lenCs);

   inherited::EvID_    = evID;
   inherited::SubEvID_ = subEvID;
   inherited::Length_  = sizeof(SEvTrkHdr) + len + 3; // ChkSum + 0x0d + 0x0a

   *chkSum = 0;
   for (Kway::byte* cur=chkSumBeg; cur!=chkSumEnd; ++cur)
      *chkSum ^= *cur;

   Data_[len+1] = 0x0d;
   Data_[len+2] = 0x0a;

   return inherited::Length_;
}


/////////////////////////////////////////////////////////////////////////////
// class TEvTrack impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TEvTrack::OpenFile (const char* filePath)
{
   return File_.Open(filePath, (TFileMode)(fmCreatePath | fmAppend));
}
//---------------------------------------------------------------------------

K_mf(void) TEvTrack::WriteEvent (Kway::byte evID, Kway::byte subEvID, const void* buffer, Kway::word len)
{
   SEvTrk      evTrk;
   Kway::dword lenWr = evTrk.Pack(evID, subEvID, buffer, len);

   File_.Write(&evTrk, lenWr);
   File_.Flush();
}


/////////////////////////////////////////////////////////////////////////////
// class TEvReplay impl
/////////////////////////////////////////////////////////////////////////////
