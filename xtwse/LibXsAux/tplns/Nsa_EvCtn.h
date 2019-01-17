/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvCtn_h_)
#define _Nsa_EvCtn_h_
//---------------------------------------------------------------------------
#include "inn_file.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// 型態定義
/////////////////////////////////////////////////////////////////////////////
// 定義存檔方式(檔案最大4G(dword),每筆Rec最大64K(word))
typedef rr_inn_file<dword, word>  TEvCtnInn;


/////////////////////////////////////////////////////////////////////////////
// class TIterCastEvAry
/////////////////////////////////////////////////////////////////////////////
template <class RecsT>
class TIterCastEvAry
{
public:
   typedef          RecsT                  TRecs;
   typedef typename TRecs::key_type        TKey;
   typedef typename TRecs::mapped_type     TMapped;
   typedef typename TRecs::mapped_type     TRec;
   typedef typename TRecs::iterator        TRecsI;
   typedef typename TRecs::const_iterator  TRecsCi;

   static inline K_mf(      TKey)     GetKey    (const TRecsI&  iter)  { return iter.get_idx(); }
   static inline K_mf(      TKey)     GetKey    (const TRecsCi& iter)  { return iter.get_idx(); }
   
   static inline K_mf(      TMapped&) GetMapped (      TRecsI&  iter)  { return *iter; }
   static inline K_mf(const TMapped&) GetMapped (const TRecsI&  iter)  { return *iter; }
   static inline K_mf(const TMapped&) GetMapped (const TRecsCi& iter)  { return *iter; }

   static inline K_mf(      TRec&)    GetRec    (      TRecsI&  iter)  { return *iter; }
   static inline K_mf(const TRec&)    GetRec    (const TRecsI&  iter)  { return *iter; }
   static inline K_mf(const TRec&)    GetRec    (const TRecsCi& iter)  { return *iter; }
};
/////////////////////////////////////////////////////////////////////////////
// class TIterCastEvAryWsPtr
/////////////////////////////////////////////////////////////////////////////
template <class RecsT>
class TIterCastEvAryWsPtr
{
public:
   typedef          RecsT                  TRecs;
   typedef typename TRecs::key_type        TKey;
   typedef typename TRecs::mapped_type     TMapped;
   typedef typename TMapped::ref_obj       TRec;
   typedef typename TRecs::iterator        TRecsI;
   typedef typename TRecs::const_iterator  TRecsCi;

   static inline K_mf(      TKey)     GetKey    (const TRecsI&  iter)  { return iter.get_idx(); }
   static inline K_mf(      TKey)     GetKey    (const TRecsCi& iter)  { return iter.get_idx(); }
   
   static inline K_mf(      TMapped&) GetMapped (      TRecsI&  iter)  { return *iter; }
   static inline K_mf(const TMapped&) GetMapped (const TRecsI&  iter)  { return *iter; }
   static inline K_mf(const TMapped&) GetMapped (const TRecsCi& iter)  { return *iter; }

   static inline K_mf(      TRec&)    GetRec    (      TRecsI&  iter)  { return *(iter->get()); }
   static inline K_mf(const TRec&)    GetRec    (const TRecsI&  iter)  { return *(iter->get()); }
   static inline K_mf(const TRec&)    GetRec    (const TRecsCi& iter)  { return *(iter->get()); }
};
/////////////////////////////////////////////////////////////////////////////
// class TIterCastEvMap
/////////////////////////////////////////////////////////////////////////////
template <class RecsT>
class TIterCastEvMap
{
public:
   typedef          RecsT                  TRecs;
   typedef typename TRecs::key_type        TKey;
   typedef typename TRecs::mapped_type     TMapped;
   typedef typename TRecs::mapped_type     TRec;
   typedef typename TRecs::iterator        TRecsI;
   typedef typename TRecs::const_iterator  TRecsCi;

   static inline K_mf(const TKey&)    GetKey    (const TRecsI&  iter)  { return iter->first;  }
   static inline K_mf(const TKey&)    GetKey    (const TRecsCi& iter)  { return iter->first;  }
   
   static inline K_mf(      TMapped&) GetMapped (      TRecsI&  iter)  { return iter->second; }
   static inline K_mf(const TMapped&) GetMapped (const TRecsI&  iter)  { return iter->second; }
   static inline K_mf(const TMapped&) GetMapped (const TRecsCi& iter)  { return iter->second; }

   static inline K_mf(      TRec&)    GetRec    (      TRecsI&  iter)  { return iter->second; }
   static inline K_mf(const TRec&)    GetRec    (const TRecsI&  iter)  { return iter->second; }
   static inline K_mf(const TRec&)    GetRec    (const TRecsCi& iter)  { return iter->second; }
};


/////////////////////////////////////////////////////////////////////////////
// 巨集
/////////////////////////////////////////////////////////////////////////////
#define Nsa_EvCtn_RoomKey_Size  sizeof(Kway::Nsa::TEvCtnInn::rr_key)
//---------------------------------------------------------------------------
#define Nsa_EvCtn_Default_Operator_Equal(Name)           \
   K_mf(void) operator = (const Name& rhs)               \
   {                                                     \
      Kway::Nsa::TEvCtnInn::rr_key roomKey = RoomKey_;   \
                                                         \
      memcpy(this, &rhs, sizeof(Name));                  \
      RoomKey_ = roomKey;                                \
   }                                                     \
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// struct SEvCtnDatNull
/////////////////////////////////////////////////////////////////////////////
struct SEvCtnDatNull
{
};
/////////////////////////////////////////////////////////////////////////////
// struct SEvCtnDatNullInn
/////////////////////////////////////////////////////////////////////////////
struct SEvCtnDatNullInn
{
   TEvCtnInn::rr_key mutable  RoomKey_;
      
   Nsa_EvCtn_Default_Operator_Equal(SEvCtnDatNullInn)
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvCtn_h_)
