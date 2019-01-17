//---------------------------------------------------------------------------
#ifndef kpersistentH
#define kpersistentH
//---------------------------------------------------------------------------
#include "HiLoTrans.h"
#include "fixed_num.h"
#include "char_ary.h"
#include "comp6.h"
#include <string>
#include <bitset>
//---------------------------------------------------------------------------
namespace Kway {
   LIBNS_class TFile;
}
//---------------------------------------------------------------------------
namespace Kstl {
/* �אּ�ϥ� #include "fixed_num.h" �_�h�b BCB(��L compiler �S�չL), �|�����D :(
template <class>           class fixed_num;
template <class,unsigned>  class fixed_dec;
*/
/* �אּ�ϥ� #include "char_ary.h" �_�h�b BCB(��L compiler �S�չL), �|�����D :(
template <class T,unsigned,T> class char_ary;
template <class,unsigned>     class char_ary_base;
template <unsigned N>         class TComp6;
*/
/////////////////////////////////////////////////////////////////////////////
using Kway::THiLoTransRef;
//---------------------------------------------------------------------------
LIBNS_class persistent
{
   void*                Buf_;
   unsigned             BufSize_;
   unsigned    mutable  RWPos_;
   Kway::THiLoTransRef  HiLoTrans_;

   template <class TBuf>// <void*> or <const void*>
   inline K_mf(TBuf*) GetRWPtr (TBuf* buf, unsigned sz) const
      {  unsigned endPos = RWPos_ + sz;
         if(endPos > BufSize_)
            return 0;
         void* ptr = ((Kway::byte*)(buf)) + RWPos_;
         RWPos_ = endPos;
         return ptr;
      }
   inline K_mf(void*) GetWrPtr (unsigned sz)
      {  void* ptr;
         if((ptr = GetRWPtr(Buf_,sz)) != NULL)
            return ptr;
         unsigned newSz = BufSize_ + sz;
         if((ptr = ReallocWrBuf(newSz)) != NULL) {
            BufSize_ = newSz;
            return GetRWPtr(Buf_ = ptr, sz);
         }
         return NULL;
      }
   virtual K_mf(void*) ReallocWrBuf (unsigned&)
      {  return NULL;
      }
protected:
   inline K_ctor  persistent  (THiLoTransRef hiLoTrans = THiLoTransRef(THiLoTransRef::DwordLocalMachine_))
      : Buf_(NULL)
      , HiLoTrans_(hiLoTrans)
      {
      }
public:
   /** �غc��,���w�x�s��buffer.
   */
   inline K_ctor  persistent  (void* buf, unsigned bufSize, THiLoTransRef hiLoTrans = THiLoTransRef(THiLoTransRef::DwordLocalMachine_))
      :  HiLoTrans_(hiLoTrans)
      {  reset(buf,bufSize);
      }

   /** �Ѻc.
   */
   virtual K_dtor ~persistent ()
      {
      }

   /** �s�J�� persistent ����Ƥj�p(bytes).
   */
   inline K_mf(unsigned)  get_saved_size  () const    { return RWPos_; }

   /** �w�İϳѾl�j�p.
   */
   inline K_mf(unsigned) get_left_size () const   { return BufSize_ - RWPos_; }

   /** �N��Ʊq persistent ���X.
   */
   K_mf(bool) load (void*, unsigned, bool chkTrans) const;

   /** �q persistent ���X/�s�J �@�Ӧr��: (WORD:size)+char[size]
   */
   K_mf(bool) load (std::string&) const;
   K_mf(bool) save (const std::string&);

   template <class POD>
      inline K_mf(bool) loadBytes (POD& a) const
         {  if(const void* ptr = GetRWPtr(Buf_, sizeof(a))) {
               a = K_Ptr2Ref(reinterpret_cast<const POD*>(ptr));
               return true;
            }
            return false;
         }

   template <class POD>
      inline K_mf(bool) loadHiLoTrans (POD& a) const
         {  if(const void* ptr = GetRWPtr(Buf_, sizeof(a))) {
               //K_Ptr2Ref(&a) = HiLoTransT(K_Ptr2Ref(reinterpret_cast<const POD*>(ptr)), HiLoTrans_); �o�˼g�٬O�|crash!
               POD n = K_Ptr2Ref(reinterpret_cast<const POD*>(ptr));
               K_Ptr2Ref(&a) = HiLoTransT(n, HiLoTrans_);
               return true;
            }
            return false;
         }

   /** �N��Ʀs�J persistent.
   */
   virtual K_mf(bool) save (const void*, unsigned, bool chkTrans);

   template <class POD>
      inline K_mf(bool) saveBytes (const POD& a)
         {  if(void* ptr = GetWrPtr(sizeof(a))) {
               K_Ptr2Ref(reinterpret_cast<POD*>(ptr)) = a;
               return true;
            }
            return false;
         }

   template <class POD>
      inline K_mf(bool) saveHiLoTrans (const POD& a)
         {  if(void* ptr = GetWrPtr(sizeof(a))) {
               K_Ptr2Ref(reinterpret_cast<POD*>(ptr)) = HiLoTransT(a, HiLoTrans_);
               return true;
            }
            return false;
         }

   inline K_mf(void) reset (void* buf, unsigned bufSize)
      {  Buf_       = buf;
         BufSize_   = bufSize;
         RWPos_     = 0;
      }

   inline K_mf(Kway::THiLoTransRef) GetHiLoTrans () const   { return HiLoTrans_; }

   inline K_mf(void) resetHiLoTrans (THiLoTransRef hiLoTrans)
      {  HiLoTrans_ = hiLoTrans;
      }
   inline K_mf(void) rewind ()
      {  RWPos_ = 0;
      }
   inline K_mf(void) back (unsigned n) const
      {  if(RWPos_ >  n)
            RWPos_ -= n;
         else
            RWPos_  = 0;
      }

   inline K_mf(const void*)  getbuf () const    { return Buf_; }

   /** �q�ɮ׸��J sz bytes, ��J RWPos_ ��m, �ò��� RWPos_.
   */
   K_mf(bool) LoadFromFile (Kway::TFile&, unsigned sz);
protected:
   inline K_mf(void*)        getbuf ()          { return Buf_;     }
   inline K_mf(unsigned)     getbuf_size() const{ return BufSize_; }
};
//---------------------------------------------------------------------------
LIBNS_class persistent_auto : public persistent
{
   typedef persistent   inherited;

   unsigned MemSize_;

   persistent_auto (const persistent_auto&);//�T��ƻs.
   void operator=  (const persistent_auto&);//�T��ƻs.
   K_mf(void*) ReallocWrBuf (unsigned& sz);
public:
   /** �غc��,���t�O����.
   */
   K_ctor  persistent_auto  (unsigned bufSize, THiLoTransRef hiLoTrans = THiLoTransRef(THiLoTransRef::DwordLocalMachine_));

   /** �Ѻc��,�۰�����ѧڤ��t���O����.
   */
   K_dtor ~persistent_auto  ();

   /** �վ� buffer �j�p.
   */
   K_mf(bool) ResetBufferSize (unsigned sz);

   /** �}�� GetBuf(), ���ϥΪ̥i�H�b�غc���J���.
   */
   using inherited::getbuf;
};
/////////////////////////////////////////////////////////////////////////////
/** ��@�Ǥw���� POD type, �i�����s�J�Ψ��X.
*/
template <class T>
   inline K_mf(bool) pload (const persistent& p, fixed_num<T>& a)       { return p.loadHiLoTrans(a); }

template <class T, unsigned D>
   inline K_mf(bool) pload (const persistent& p, fixed_dec<T,D>& a)     { return p.loadHiLoTrans(a); }

template <class T, unsigned N>
   inline K_mf(bool) pload (const persistent& p, char_ary_base<T,N>& a) { return p.loadBytes(a);     }

template <class T, unsigned N, T NullChar, typename TSize>
   inline K_mf(bool) pload (const persistent& p, char_ary<T,N,NullChar>& a, TSize* psz)
      {  //�ʺA�վ��x�s�Ŷ�.
         TSize sz;
         if(!pload(p, sz))
            return false;
         if(sz >= N)
            sz =  N * sizeof(T);
         else {
            a.set(sz/sizeof(T), 0);
            if(sz <= 0)
               return true;
         }
         if(psz)
            *psz = sz;
         return pload(p, a.begin(), sz);
      }

template <unsigned N>
   inline K_mf(bool) pload (const persistent& p, TComp6<N>& a)          { return p.loadBytes(a);     }

inline K_mf(bool) pload (const persistent& p, bool& a)                  { return p.loadBytes(a);     }
inline K_mf(bool) pload (const persistent& p, Kway::int8& a)            { return p.loadBytes(a);     }
inline K_mf(bool) pload (const persistent& p, Kway::int16& a)           { return p.loadHiLoTrans(a); }
inline K_mf(bool) pload (const persistent& p, Kway::int32& a)           { return p.loadHiLoTrans(a); }
inline K_mf(bool) pload (const persistent& p, Kway::int64& a)           { return p.loadHiLoTrans(a); }
inline K_mf(bool) pload (const persistent& p, Kway::uint64& a)          { return p.loadHiLoTrans(a); }
inline K_mf(bool) pload (const persistent& p, Kway::byte& a)            { return p.loadBytes(a);     }
inline K_mf(bool) pload (const persistent& p, Kway::word& a)            { return p.loadHiLoTrans(a); }
inline K_mf(bool) pload (const persistent& p, Kway::dword& a)           { return p.loadHiLoTrans(a); }
inline K_mf(bool) pload (const persistent& p, char* s, unsigned sz)     { return p.load(s,sz,false); }
inline K_mf(bool) pload (const persistent& p, std::string& s)           { return p.load(s);          }
inline K_mf(bool) psave (persistent& p, const std::string& s)           { return p.save(s);          }
//---------------------------------------------------------------------------
template <class T>
   inline K_mf(bool) psave (persistent& p, const fixed_num<T>& a)       { return p.saveHiLoTrans(a); }

template <class T, unsigned D>
   inline K_mf(bool) psave (persistent& p, const fixed_dec<T,D>& a)     { return p.saveHiLoTrans(a); }

template <class T, unsigned N>
   inline K_mf(bool) psave (persistent& p, const char_ary_base<T,N>& a) { return p.saveBytes(a);     }

template <class T, unsigned N, T NullChar, typename TSize>
   inline K_mf(bool) psave (persistent& p, const char_ary<T,N,NullChar>& a, TSize* psz)
      {  //�ʺA�վ��x�s�Ŷ�.
         const T* msg = a.begin();
         const T* eos = std::find(msg, a.end(), NullChar);
         TSize    sz  = TSize((eos - msg) * sizeof(T));
         if(psz)
            *psz = sz;
         return psave(p, sz)
             && psave(p, msg, sz);
      }

template <unsigned N>
   inline K_mf(bool) psave (persistent& p, const TComp6<N>& a)          { return p.saveBytes(a);     }

inline K_mf(bool) psave (persistent& p, const bool&       a)            { return p.saveBytes(a);     }
inline K_mf(bool) psave (persistent& p, const Kway::byte& a)            { return p.saveBytes(a);     }
inline K_mf(bool) psave (persistent& p, const Kway::word& a)            { return p.saveHiLoTrans(a); }
inline K_mf(bool) psave (persistent& p, const Kway::dword& a)           { return p.saveHiLoTrans(a); }
inline K_mf(bool) psave (persistent& p, const Kway::int8& a)            { return p.saveBytes(a);     }
inline K_mf(bool) psave (persistent& p, const Kway::int16& a)           { return p.saveHiLoTrans(a); }
inline K_mf(bool) psave (persistent& p, const Kway::int32& a)           { return p.saveHiLoTrans(a); }
inline K_mf(bool) psave (persistent& p, const Kway::int64& a)           { return p.saveHiLoTrans(a); }
inline K_mf(bool) psave (persistent& p, const Kway::uint64& a)          { return p.saveHiLoTrans(a); }
inline K_mf(bool) psave (persistent& p, const char* s, unsigned sz)     { return p.save(s,sz,false); }
//---------------------------------------------------------------------------
#ifdef KTimeH
inline K_mf(bool) pload (const persistent& p, Kway::TKTime& a)          { return p.loadBytes(a);     }
inline K_mf(bool) pload (const persistent& p, Kway::TKDate& a)          { return p.loadHiLoTrans(a); }
inline K_mf(bool) pload (const persistent& p, Kway::TKDateTime& a)      { Kway::TKDate d;
                                                                          if(!pload(p,d))
                                                                             return false;
                                                                          Kway::TKTime t;
                                                                          if(!pload(p,t))
                                                                             return false;
                                                                          a.SetDateTime(d,t);
                                                                          return true;
                                                                        }
inline K_mf(bool) pload (const persistent& p, Kway::TKDateTimeMS& a)    { Kway::TKDateTime  dt;
                                                                          if(!pload(p,dt))
                                                                             return false;
                                                                          Kway::TKTimeMS  ms;
                                                                          if(!p.loadHiLoTrans(ms))
                                                                             return false;
                                                                          a.SetDateTime(dt);
                                                                          a.SetMS(ms);
                                                                          return true;
                                                                        }

inline K_mf(bool) psave (persistent& p, const Kway::TKTime& a)          { return p.saveBytes(a);     }
inline K_mf(bool) psave (persistent& p, const Kway::TKDate& a)          { return p.saveHiLoTrans(a); }
inline K_mf(bool) psave (persistent& p, const Kway::TKDateTime& a)      { return psave(p, a.GetDate())
                                                                              && psave(p, a.GetTime());
                                                                        }
inline K_mf(bool) psave (persistent& p, const Kway::TKDateTimeMS& a)    { return psave(p, a.GetDate())
                                                                              && psave(p, a.GetTime())
                                                                              && p.saveHiLoTrans(a.GetMS());
                                                                        }
#endif
/////////////////////////////////////////////////////////////////////////////
struct persistent_save
{
   template <class DestType>
      static inline K_mf(bool) store (Kstl::persistent& per, const DestType& a)
         {  return psave(per,a);
         }
   template <class DestType, typename TSize>
      static inline K_mf(bool) store (Kstl::persistent& per, const DestType& a, TSize* a2)
         {  return psave(per,a,a2);
         }
};
//---------------------------------------------------------------------------
struct persistent_load
{
   template <class DestType>
      static inline K_mf(bool) store (const Kstl::persistent& per, DestType& a)
         {  return pload(per,a);
         }
   template <class DestType, typename TSize>
      static inline K_mf(bool) store (const Kstl::persistent& per, DestType& a, TSize* a2)
         {  return pload(per,a,a2);
         }
};
//---------------------------------------------------------------------------
/** �ϥ� struct persistent_save, struct persistent_load �A�i�H�g�X�o�˪��{���X:
    �p���N�i�� save/load �����@�P������, ���|������������εo��.
*/
class TMyTestObj
{
public:
   Kway::byte  Field_1_;
   Kway::word  Field_2_;
   Kway::dword Field_3_;

   template <class TPersistent, class TStore, class TDestType>
   static K_mf(bool) Store (TPersistent& per, TStore& storage, TDestType& obj)
      {  return storage.store(per, obj.Field_1_)
             && storage.store(per, obj.Field_2_)
             && storage.store(per, obj.Field_3_);
      }

   /** �s�J persistent.
   */
   K_mf(bool) Serialize (Kstl::persistent& per) const
      {  Kstl::persistent_save saver;
         return Store(per, saver, *this);
      }

   /** �q persistent ���X.
   */
   K_mf(bool) Deserialize (const Kstl::persistent& per)
      {  Kstl::persistent_load loader;
         return Store(per, loader, *this);
      }
};
/////////////////////////////////////////////////////////////////////////////
struct TStoreSizeCounter
{
   size_t   Size_;

   inline K_ctor TStoreSizeCounter ()   : Size_(0)  {}

   inline K_mf(bool) store (const std::string& t)
         {  Kway::dword usz = (Kway::dword)t.size();
            Size_ += sizeof(Kway::word) + usz;
            if(usz >= 0xffff)
               Size_ += sizeof(usz);
            return true;
         }

   template<class TPersistent, class T>
      inline K_mf(bool) store (const TPersistent&, const T& t)
         {  return store(t);
         }

   template<class T>
      inline K_mf(bool) store (const T&)
         {  Size_ += sizeof(T);
            return true;
         }

   template <class TPersistent>
      inline K_mf(bool) store (const TPersistent&, const char* a, unsigned sz)
         {  Size_ += sz;
            return true;
         }

   template <class TPersistent, class T, unsigned N, T NullChar, typename TSize>
      K_mf(bool) store (const TPersistent&, char_ary<T,N,NullChar>& a, TSize* psz)
         {  Size_ += sizeof(TSize);
            const T* msg = a.begin();
            const T* eos = std::find(msg, a.end(), NullChar);
            Size_ += TSize((eos - msg) * sizeof(T));
            return true;
         }
};
/////////////////////////////////////////////////////////////////////////////
template <unsigned bitn>
K_mf(bool) psave (Kstl::persistent& p, const std::bitset<bitn>& sets)
{
   Kway::byte  dats[(bitn+7)/8];
   memset(dats, 0, sizeof(dats));
   for(int L = 0;  L < bitn;  ++L)
      if(sets.test(L))
         dats[L/8] |= Kway::byte(1 << (L%8));
   return p.save(dats,sizeof(dats),false);
}
//---------------------------------------------------------------------------
template <unsigned bitn>
K_mf(bool) pload (const Kstl::persistent& p, std::bitset<bitn>& sets)
{
   Kway::byte  dats[(bitn+7)/8];
   if(!p.load(dats,sizeof(dats),false))
      return false;
   sets.reset();
   for(int L = 0;  L < bitn;  ++L)
      if(dats[L/8] & (1 << (L%8)))
         sets.set(L);
   return true;
}
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

