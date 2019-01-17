//---------------------------------------------------------------------------
#ifndef HiLoTransH
#define HiLoTransH
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
LIBNS_struct THiLoTransRef {
   static const dword   DwordLocalMachine_ = 0x12345678;

   inline explicit K_ctor  THiLoTransRef  (dword ref)          { Dword_ = ref; }
   inline explicit K_ctor  THiLoTransRef  (const char ref[4])  { Dword_ = K_Ptr2Ref((const dword*)ref); }
   inline K_mf(bool)       IsEqLocal      () const      { return Dword_==DwordLocalMachine_; }
   union {
      dword    Dword_;               // = 0x12345678
      byte     Bytes_[sizeof(dword)];// x86:       [0]=0x78,[1]=0x56,[2]=0x34,[3]=0x12
                                     // Sun Sparc: [0]=0x12,[1]=0x34,[2]=0x56,[4]=0x78
   };
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
struct THiLoTransDef {
   byte  ByteLen_; // =2,4,8 = sizeof(data)
   word  ByteOfs_;
};
#define MAC_HiLoTransDef(s,f)    { fldsize(s,f), fldoffset(s,f) }
//---------------------------------------------------------------------------
LIBNS_fn(void)  HiLoTrans  (void* rec, THiLoTransRef ref, const THiLoTransDef defs[], int defsCount);
//---------------------------------------------------------------------------
template <class T>
T  HiLoTransT  (const T rec, THiLoTransRef ref)
{
   static const THiLoTransDef  def = { sizeof(T), 0 };
   T  dat = rec;
   HiLoTrans(&dat, ref, &def, 1);
   return dat;
}
//---------------------------------------------------------------------------
} // namespace Kway
#endif

