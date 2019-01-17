/** @file InnPeeker.h

@brief 數個常用的 InnPeeker<>

*/
// $Id: InnPeeker.h,v 1.2 2005/06/26 11:19:37 fonwin Exp $
//---------------------------------------------------------------------------
#ifndef InnPeekerH
#define InnPeekerH
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
/** @brief 直接將 [整個T] 從 inn 存入、取出的 class.
*/
template <class T, unsigned roomSize = sizeof(T)>
class TSimpleInnPeeker
{
   T  PeekValue_;
public:
   typedef T   value_type;

   /** @brief 儲存時所需佔用的空間(bytes).
   */
   static const int  RoomSize  = roomSize;

   /** @brief [儲存空間]描述物件的數量
   */
   static const int  RoomItems = 1;

   /** @brief 設定[儲存空間]描述物件 - 取出用.
   */
   template <class LivePeek>
      inline K_mf(void)  SetLivePeek  (LivePeek* i)   { i->obj_ptr_  = &PeekValue_;
                                                        i->obj_size_ = RoomSize; }
   /** @brief 取得 [從 inn 取出] 的物件.
   */
   inline K_mf(const T&)  GetPeekValue  (const void*) const  { return PeekValue_;  }

   /** @brief 用 [從 inn 取出] 的物件, 建立新的T.
   */
   template <class LivePeek>
      inline K_mf(T*)  NewPeekValue  (const LivePeek*) const  { return new T(PeekValue_); }

   /** @brief 設定[儲存空間]描述物件 - 儲存用.
   */
   template <class LiveInto>
      static inline K_mf(void)  SetLiveInto  (LiveInto* i, const T& v)
                                                      { i->obj_ptr_  = &v;
                                                        i->obj_size_ = RoomSize; }
};
//---------------------------------------------------------------------------
/** @brief MFC 無法處理 template 預設參數,
   所以 TMapValuePeeker<> 的參數可能需要用 TSimpleInnPeeker1
*/
template <class T>
class TSimpleInnPeeker1 : public TSimpleInnPeeker<T> {
};
//---------------------------------------------------------------------------
/** @brief 將 std::string 從 inn 存入、取出的 class.

若 string 超過 maxLen bytes, 則只存入 maxLen bytes.
*/
template <unsigned maxLen>//原本我想讓maxLen=255,可是gcc在此處不可有預設值.
class TSimpleInnPeeker<std::string, maxLen>
{
   char  PeekValue_[maxLen];

   static const int  SizeOfLen = ((maxLen & 0xff00 == 0) ? 1 : 2);
public:
   typedef std::string  value_type;

   /** @brief 儲存時所需佔用的空間(bytes).
   */
   static const int  RoomSize  = maxLen + SizeOfLen;

   /** @brief [儲存空間]描述物件的數量
   */
   static const int  RoomItems = 2;

   template <class LivePeek>
      inline K_mf(void)  SetLivePeek  (LivePeek* i)
                                 { i->obj_ptr_  = K_GetLo(&i[1].obj_size_, SizeOfLen);
                                   i->obj_size_ = SizeOfLen;
                                   ++i;
                                   i->obj_ptr_  = PeekValue_;
                                   i->obj_size_ = 0; }
   template <class LivePeek>
      K_mf(std::string*)  NewPeekValue  (const LivePeek* i)
                                 { return new std::string(PeekValue_, i[1].obj_size_); }

   template <class LiveInto>
      inline static K_mf(void)  SetLiveInto  (LiveInto* i, const std::string& v)
                                 { i->obj_ptr_  = K_GetLo(&i[1].obj_size_, SizeOfLen);
                                   i->obj_size_ = SizeOfLen;
                                   ++i;
                                   i->obj_ptr_  = v.data();
                                   size_t   len = v.length();
                                   i->obj_size_ = (len > maxLen ? maxLen : len); }
};
/////////////////////////////////////////////////////////////////////////////
template < class T
         , template<class> class keyPeeker
         , template<class> class mappedPeeker >
class TMapValuePeeker
{
   typedef keyPeeker<typename T::key_type>         KeyPeeker;
   typedef mappedPeeker<typename T::mapped_type>   MappedPeeker;

   KeyPeeker      KeyPeeker_;
   MappedPeeker   MappedPeeker_;
public:
   //types.
   typedef typename T::value_type                  value_type;
   typedef T                                       recs_type;

   static const int  RoomSize  = KeyPeeker::RoomSize  + MappedPeeker::RoomSize;
   static const int  RoomItems = KeyPeeker::RoomItems + MappedPeeker::RoomItems;

   template <class LivePeek>
      inline K_mf(void)  SetLivePeek  (LivePeek* i)
         { KeyPeeker_   .SetLivePeek(i);
           MappedPeeker_.SetLivePeek(i+KeyPeeker_.RoomItems); }

   template <class LivePeek>
      K_mf(void)  Insert  (LivePeek* i, T* recs)
         { recs->insert( value_type(KeyPeeker_   .GetPeekValue(i),
                                    MappedPeeker_.GetPeekValue(i+KeyPeeker_.RoomItems)) ); }

   template <class LiveInto>
      static inline K_mf(void)  SetLiveInto  (LiveInto* i, const value_type& v)
         { KeyPeeker   ::SetLiveInto(i, v.first);
           MappedPeeker::SetLiveInto(i+KeyPeeker::RoomItems, v.second); }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

