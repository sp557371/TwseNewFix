//
// 一些基本的定義:
//    包含: Compiler, OS.. 相關的定義
//
// 版本: $Id: KwayBase.h,v 1.15 2006/04/03 14:33:12 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef KwayBaseH
#define KwayBaseH
//---------------------------------------------------------------------------
#if defined(__GNUC__)
#  include <sys/types.h>
#  include <cstddef>
#endif
#ifdef _MSC_VER
   //這裡面也有宣告 memcpy, 避免底下的 #define memcpy 造成問題, 所以先 include.
#  include <intrin.h>
#  include <wchar.h>
#endif

#include <string.h>
//---------------------------------------------------------------------------
namespace Kway {

   /// fonwin: 20140126
   /// The memory areas should not overlap.  Use memmove(3) if the memory areas do overlap.
   /// 不只是 memcpy(dst+1, dst, sz); 會有問題, 即使是 memcpy(dst, dst+1, sz); 也可可能會有問題!!
   /// 所以在此強制將 memcpy() 改成 memmove().
   /// 可參考: http://blog.csdn.net/yasi_xi/article/details/9223241
   #ifdef memcpy
   #undef memcpy
   #endif
   #define memcpy    memmove
//---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning(disable : 4251 4355 4503)
#endif
//---------------------------------------------------------------------------
#define __NSX__
#if defined(_MSC_VER)
   #define HANDLE_PRAGMA_PACK_PUSH_POP    //WIN32 style pack
   #define K_FASTCALL        //__fastcall 在CLR(Managed)環境,不可使用 __fastcall
   #define K_FASTCALL_CTOR
   #define K_IMPORT          __declspec(dllimport)
   #define K_EXPORT          __declspec(dllexport)
#elif defined(__BORLANDC__)
   #define HANDLE_PRAGMA_PACK_PUSH_POP    //WIN32 style pack
   #define K_FASTCALL        __fastcall
   #define K_FASTCALL_CTOR   K_FASTCALL
   #define K_IMPORT          __declspec(package)
   #define K_EXPORT          __declspec(package)
#else
   #define K_IMPORT
   #define K_EXPORT
   #define K_FASTCALL
   #define K_FASTCALL_CTOR
#endif

#define K_ctor               K_FASTCALL_CTOR
#define K_dtor               K_FASTCALL_CTOR
#define K_mf(return_type)    return_type K_FASTCALL

#if defined(LIBNS_NODLL)
  #define LIBNSDLL
#elif defined(LIBNS_EXPORTS)
  #define LIBNSDLL           K_EXPORT
#else
  #define LIBNSDLL           K_IMPORT
#endif

#define LIBNS_class              class  LIBNSDLL
#define K_class                  class  LIBNSDLL
#define LIBNS_struct             struct LIBNSDLL
#define K_struct                 struct LIBNSDLL
// functions
#define LIBNS_fn(return_type)    extern LIBNSDLL return_type K_FASTCALL
#define K_fn(return_type)        extern LIBNSDLL return_type K_FASTCALL
// global datas
#define LIBNS_gd(data_type)      extern LIBNSDLL data_type
#define K_gd(data_type)          extern LIBNSDLL data_type
// extern "C" export
#define LIBNS_exportC(data_type) extern "C" LIBNSDLL data_type
#define K_exportC(data_type)     extern "C" LIBNSDLL data_type

#define LIBNS_EXTERN_TEMPLATE_CLASS     template class LIBNSDLL
#define K_EXTERN_TEMPLATE_CLASS         template class LIBNSDLL

// macros, 因為 gcc 使用 ((s*)0) 會有警告, 所以改用 ((s*)1)
#define numofele(ary)      (sizeof(ary) / sizeof(ary[0]))
#define fldoffset(s,f)	   ((size_t)(&(((s*)1)->f))-1)
#define fldsize(s,f)       (size_t)(sizeof(((s*)1)->f))
//---------------------------------------------------------------------------
// types
#if defined(__BORLANDC__) || defined(_MSC_VER)
   typedef __int8             int8;
   typedef __int16            int16;
   typedef __int32            int32;
   typedef __int64            int64;
   typedef unsigned __int8    uint8;
   typedef unsigned __int16   uint16;
   typedef unsigned __int32   uint32;
   typedef unsigned __int64   uint64;
   #define K_EnumPacked
   #define K_StructPacked
   #ifdef _MSC_VER
      #define K_EnumPacked_Bug
   #endif
#elif defined(__GNUC__)
   #ifdef __alpha
      #define KWAY_INT64_IS_LONG
      typedef char            int8;
      typedef short           int16;
      typedef int             int32;
      typedef long            int64;
      typedef unsigned char   uint8;
      typedef unsigned short  uint16;
      typedef unsigned int    uint32;
      typedef unsigned long   uint64;
   #else
      typedef int8_t          int8;
      typedef int16_t         int16;
      typedef int32_t         int32;
      typedef int64_t         int64;
      #ifdef __linux__
         typedef u_int8_t     uint8;
         typedef u_int16_t    uint16;
         typedef u_int32_t    uint32;
         typedef u_int64_t    uint64;
      #else
         typedef uint8_t      uint8;
         typedef uint16_t     uint16;
         typedef uint32_t     uint32;
         typedef uint64_t     uint64;
      #endif
      #if __SIZEOF_LONG__ == 8
      #define KWAY_INT64_IS_LONG
      #endif
   #endif // __alpha..else
   //↓gcc使用 packed 的 enum 時, 應該加上: -fshort-enums, 可是加上此選項會造成更多的問題
   //     理論上應該使用 enum mytype {} __attribute__((packed));
   //     可是 gcc 3.2 -- 3.3有 bug,會造成 compile 的 error 訊息!
   #define K_EnumPacked_Bug
   #define K_EnumPacked     //__attribute__((packed))
   //↓一般來說使用 pragma pack() 即可,但在 template<> 之中 gcc 無法正確的處理
   #define K_StructPacked     __attribute__((packed))
#else
#  error "Unknow compiler!"
#endif
typedef uint8                 byte;
typedef uint16                word;
typedef uint32                dword;
//---------------------------------------------------------------------------
//OS macros define
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
#  define   K_WINDOWS
#  define   K_WINDOWS32
#else
#  define   K_UNIXLIKE
#endif

#if defined(__sparc)           //Sparc系列 (PowerPC...?)
#   define K_CPU_WordAlign     //有 word alignment 的需求!
#   define K_LoByte_Tail       //LoByte 在最後
#endif
#if defined(__alpha)
#   define K_CPU_WordAlign     //有 word alignment 的需求!
//為了讓 Symbol 長度能減少, 所以重新定義一些常用的名稱
#define TSimpleMap_AddRoomKey  TTSiMa
#define TSimpleRecorder_Detail TTSiRd
#define map2d_iterator_base    m2ItrB
#define map2d_maker_key_find   m2MkKF
#define TSimpleRecorder        TTSiR
#define map2d_maker_base       m2MkB
#define ev_container_t         ec_t
#define ev_container_t1        ec_t1
#define ev_container_t2        ec_t2
#define ev_container_u         ec_u
#define ev_container_i         ec_i
#define rec_updater_impl       rupi
#define pair_updater_impl      pupi
#define TModuleRecKey          TMLK
#endif
#if defined(__hpux)            //ia64?
#   define K_CPU_WordAlign     //有 word alignment 的需求!
#   define K_LoByte_Tail
#endif
#if defined(_AIX)
#   define K_LoByte_Tail
#endif
//---------------------------------------------------------------------------
#define K_template_ctor(Derive,Base)                  \
   template <class T1>                                \
      inline K_ctor  Derive  (const T1& a1)           \
                   : Base(a1)  {}                     \
   template <class T1, class T2>                      \
      inline K_ctor  Derive  (const T1& a1, const T2& a2) \
                   : Base(a1,a2)  {}                  \
   template <class T1, class T2, class T3>            \
      inline K_ctor  Derive  (const T1& a1, const T2& a2, const T3& a3)  \
                   : Base(a1,a2,a3)  {}
//--- template的參數太長,會讓 gcc 在 alpha DEC 上, 發生問題, 所以只要分開,
//    真的有此需要時, 再另外想辦法囉.
#define K_template_ctor4(Derive,Base)                 \
   template <class T1, class T2, class T3, class T4>  \
      inline K_ctor  Derive  (const T1& a1, const T2& a2, const T3& a3, const T4& a4)  \
                   : Base(a1,a2,a3,a4)  {}            \
   template <class T1, class T2, class T3, class T4, class T5>  \
      inline K_ctor  Derive  (const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5)  \
                   : Base(a1,a2,a3,a4,a5)  {}         \
   template <class T1, class T2, class T3, class T4, class T5, class T6>  \
      inline K_ctor  Derive  (const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6)  \
                   : Base(a1,a2,a3,a4,a5,a6)  {}         
// 因為 alpha 的 loader 在 template<> 太過複雜時,
// 會出現「unresolvable WEAK symbol」的錯誤,
// 所以要想辦法解決 template<> 解析之後太過複雜的問題:
// 原來寫法:  typedef ev_map<>               TOrgType;
//            typedef TTViewArea<TOrgType>   TVA;
//            可能會造成 Symbol 太長的問題(要等到執行時才會出現)
// 請改成:    typedef ev_map<>               TOrgType_t;
//            K_typedef(TOrgType_t,          TOrgType);
//            typedef TTViewArea<TOrgType>   TVA;
// 理論上應該可以直接寫 K_typedef(ev_map<>,  TOrgType);
// 可是大多數的 compiler 巨集的參數不可使用 template,
// 所以才有個中介的 TOrgType_t 出現.
#ifdef __alpha
#define K_typedef(TL,TR)      \
   struct TR : public TL {    \
     K_ctor  TR  ()  {}       \
     K_template_ctor(TR,TL)   \
   }
//----------
#define K_typedef_NoDefCtor(TL,TR)  \
   struct TR : public TL {          \
     K_template_ctor(TR,TL)         \
   }
#else
#define K_typedef(TL,TR)                       typedef TL  TR
#define K_typedef_NoDefCtor(TL,TR)             typedef TL  TR
#endif
//---------------------------------------------------------------------------
#ifdef K_LoByte_Tail
      #define K_GetLo(p,n)   ((Kway::byte*)(p) + sizeof(*p) - n)//取得最低位元組
#else
      #define K_GetLo(p,n)   ((Kway::byte*)p)
#endif
/////////////////////////////////////////////////////////////////////////////
template <typename T>
struct DeConst {
    typedef T  R;
};
//---------------------------------------------------------------------------
#ifndef __BORLANDC__
template <typename T>
struct DeConst<const T> {
   typedef T   R;
};
#endif
/////////////////////////////////////////////////////////////////////////////
#ifdef K_CPU_WordAlign
      template <int Tsz>
         class Tbytea {
         public:
           inline static void copy_bytes (void* dst, const void* src) {
             memcpy(dst, src, Tsz);
           }
         };
      template <>
         class Tbytea<1> {
         public:
           inline static void copy_bytes (void* dst, const void* src) {
             *(byte*)dst = *(const byte*)src;
           }
         };
      template <>
         class Tbytea<2> {
         public:
           inline static void copy_bytes (void* dst, const void* src) {
             ((byte*)dst)[0] = ((const byte*)src)[0];
             ((byte*)dst)[1] = ((const byte*)src)[1];
           }
         };
      template <>
         class Tbytea<4> {
         public:
           inline static void copy_bytes (void* dst, const void* src) {
             ((byte*)dst)[0] = ((const byte*)src)[0];
             ((byte*)dst)[1] = ((const byte*)src)[1];
             ((byte*)dst)[2] = ((const byte*)src)[2];
             ((byte*)dst)[3] = ((const byte*)src)[3];
           }
         };
      template <>
         class Tbytea<8> {
         public:
           inline static void copy_bytes (void* dst, const void* src) {
             ((byte*)dst)[0] = ((const byte*)src)[0];
             ((byte*)dst)[1] = ((const byte*)src)[1];
             ((byte*)dst)[2] = ((const byte*)src)[2];
             ((byte*)dst)[3] = ((const byte*)src)[3];
             ((byte*)dst)[4] = ((const byte*)src)[4];
             ((byte*)dst)[5] = ((const byte*)src)[5];
             ((byte*)dst)[6] = ((const byte*)src)[6];
             ((byte*)dst)[7] = ((const byte*)src)[7];
           }
         };
      template <class T>
         class Ptr2Ref : private Tbytea<sizeof(T)> {
           typedef Ptr2Ref<T>   this_type;
           struct bytea {
             byte buf_[sizeof(T)];
           };
           T*   p_;
         public:
           inline explicit K_ctor         Ptr2Ref   (T* p)   : p_(p) {}
           inline K_mf(const this_type&)  operator= (const T& a) const {
               #ifdef __hpux
                  copy_bytes(p_, &a);
               #else
                  *reinterpret_cast<bytea*>(p_) = *(bytea*)&a;
               #endif
               return *this;
           }
           inline  operator T () const {
               typename DeConst<T>::R v;
               #ifdef __hpux
                  copy_bytes(&v, p_);
               #else
                  *(bytea*)&v = *(bytea*)p_;
               #endif
               return v;
           }
         };
      template <class T>   //若 sizeof(T) > 1 則可能發生 "Bus error" 的錯誤!
         class Ptr1Byte {
            typedef Ptr1Byte<T>  this_type;
         protected:
           T*   p_;
           inline K_ctor                  Ptr1Byte   ()          {}
         public:
           inline explicit K_ctor         Ptr1Byte   (T* p)      : p_(p) {}
           inline K_mf(const this_type&)  operator=  (T a) const { *p_ = a;  return *this; }
           inline                         operator T ()    const { return *p_; }
         };
      template <>
         class Ptr2Ref<byte> : public Ptr1Byte<byte> {
           typedef Ptr2Ref<byte>  this_type;
         public:
           inline explicit K_ctor         Ptr2Ref    (byte* p)       { p_ = p; }
           inline K_mf(const this_type&)  operator=  (byte  a) const { *p_ = a;  return *this; }
         };
      template <>
         class Ptr2Ref<int8> : public Ptr1Byte<int8> {
           typedef Ptr2Ref<int8>  this_type;
         public:
           inline explicit K_ctor         Ptr2Ref    (int8* p)       { p_ = p; }
           inline K_mf(const this_type&)  operator=  (int8  a) const { *p_ = a;  return *this; }
         };

      template <class T>
         inline Ptr2Ref<T>  MakePtr2Ref  (T* p)   { return Ptr2Ref<T>(p); }

      #define K_Ptr2Ref(p)    Kway::MakePtr2Ref(p)
#else
      #define K_Ptr2Ref(p)    (*p)
#endif
//---------------------------------------------------------------------------
} // namespace Kway
#endif
