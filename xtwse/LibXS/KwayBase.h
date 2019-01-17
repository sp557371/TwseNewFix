//
// �@�ǰ򥻪��w�q:
//    �]�t: Compiler, OS.. �������w�q
//
// ����: $Id: KwayBase.h,v 1.15 2006/04/03 14:33:12 fonwin Exp $
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
   //�o�̭��]���ŧi memcpy, �קK���U�� #define memcpy �y�����D, �ҥH�� include.
#  include <intrin.h>
#  include <wchar.h>
#endif

#include <string.h>
//---------------------------------------------------------------------------
namespace Kway {

   /// fonwin: 20140126
   /// The memory areas should not overlap.  Use memmove(3) if the memory areas do overlap.
   /// ���u�O memcpy(dst+1, dst, sz); �|�����D, �Y�ϬO memcpy(dst, dst+1, sz); �]�i�i��|�����D!!
   /// �ҥH�b���j��N memcpy() �令 memmove().
   /// �i�Ѧ�: http://blog.csdn.net/yasi_xi/article/details/9223241
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
   #define K_FASTCALL        //__fastcall �bCLR(Managed)����,���i�ϥ� __fastcall
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

// macros, �]�� gcc �ϥ� ((s*)0) �|��ĵ�i, �ҥH��� ((s*)1)
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
   //��gcc�ϥ� packed �� enum ��, ���ӥ[�W: -fshort-enums, �i�O�[�W���ﶵ�|�y����h�����D
   //     �z�פW���Өϥ� enum mytype {} __attribute__((packed));
   //     �i�O gcc 3.2 -- 3.3�� bug,�|�y�� compile �� error �T��!
   #define K_EnumPacked_Bug
   #define K_EnumPacked     //__attribute__((packed))
   //���@��ӻ��ϥ� pragma pack() �Y�i,���b template<> ���� gcc �L�k���T���B�z
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

#if defined(__sparc)           //Sparc�t�C (PowerPC...?)
#   define K_CPU_WordAlign     //�� word alignment ���ݨD!
#   define K_LoByte_Tail       //LoByte �b�̫�
#endif
#if defined(__alpha)
#   define K_CPU_WordAlign     //�� word alignment ���ݨD!
//���F�� Symbol ���ׯ���, �ҥH���s�w�q�@�Ǳ`�Ϊ��W��
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
#   define K_CPU_WordAlign     //�� word alignment ���ݨD!
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
//--- template���ѼƤӪ�,�|�� gcc �b alpha DEC �W, �o�Ͱ��D, �ҥH�u�n���},
//    �u�������ݭn��, �A�t�~�Q��k�o.
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
// �]�� alpha �� loader �b template<> �ӹL������,
// �|�X�{�uunresolvable WEAK symbol�v�����~,
// �ҥH�n�Q��k�ѨM template<> �ѪR����ӹL���������D:
// ��Ӽg�k:  typedef ev_map<>               TOrgType;
//            typedef TTViewArea<TOrgType>   TVA;
//            �i��|�y�� Symbol �Ӫ������D(�n�������ɤ~�|�X�{)
// �Ч令:    typedef ev_map<>               TOrgType_t;
//            K_typedef(TOrgType_t,          TOrgType);
//            typedef TTViewArea<TOrgType>   TVA;
// �z�פW���ӥi�H�����g K_typedef(ev_map<>,  TOrgType);
// �i�O�j�h�ƪ� compiler �������ѼƤ��i�ϥ� template,
// �ҥH�~���Ӥ����� TOrgType_t �X�{.
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
      #define K_GetLo(p,n)   ((Kway::byte*)(p) + sizeof(*p) - n)//���o�̧C�줸��
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
      template <class T>   //�Y sizeof(T) > 1 �h�i��o�� "Bus error" �����~!
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
