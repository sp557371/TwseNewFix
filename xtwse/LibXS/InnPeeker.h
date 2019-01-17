/** @file InnPeeker.h

@brief �ƭӱ`�Ϊ� InnPeeker<>

*/
// $Id: InnPeeker.h,v 1.2 2005/06/26 11:19:37 fonwin Exp $
//---------------------------------------------------------------------------
#ifndef InnPeekerH
#define InnPeekerH
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
/** @brief �����N [���T] �q inn �s�J�B���X�� class.
*/
template <class T, unsigned roomSize = sizeof(T)>
class TSimpleInnPeeker
{
   T  PeekValue_;
public:
   typedef T   value_type;

   /** @brief �x�s�ɩһݦ��Ϊ��Ŷ�(bytes).
   */
   static const int  RoomSize  = roomSize;

   /** @brief [�x�s�Ŷ�]�y�z���󪺼ƶq
   */
   static const int  RoomItems = 1;

   /** @brief �]�w[�x�s�Ŷ�]�y�z���� - ���X��.
   */
   template <class LivePeek>
      inline K_mf(void)  SetLivePeek  (LivePeek* i)   { i->obj_ptr_  = &PeekValue_;
                                                        i->obj_size_ = RoomSize; }
   /** @brief ���o [�q inn ���X] ������.
   */
   inline K_mf(const T&)  GetPeekValue  (const void*) const  { return PeekValue_;  }

   /** @brief �� [�q inn ���X] ������, �إ߷s��T.
   */
   template <class LivePeek>
      inline K_mf(T*)  NewPeekValue  (const LivePeek*) const  { return new T(PeekValue_); }

   /** @brief �]�w[�x�s�Ŷ�]�y�z���� - �x�s��.
   */
   template <class LiveInto>
      static inline K_mf(void)  SetLiveInto  (LiveInto* i, const T& v)
                                                      { i->obj_ptr_  = &v;
                                                        i->obj_size_ = RoomSize; }
};
//---------------------------------------------------------------------------
/** @brief MFC �L�k�B�z template �w�]�Ѽ�,
   �ҥH TMapValuePeeker<> ���Ѽƥi��ݭn�� TSimpleInnPeeker1
*/
template <class T>
class TSimpleInnPeeker1 : public TSimpleInnPeeker<T> {
};
//---------------------------------------------------------------------------
/** @brief �N std::string �q inn �s�J�B���X�� class.

�Y string �W�L maxLen bytes, �h�u�s�J maxLen bytes.
*/
template <unsigned maxLen>//�쥻�ڷQ��maxLen=255,�i�Ogcc�b���B���i���w�]��.
class TSimpleInnPeeker<std::string, maxLen>
{
   char  PeekValue_[maxLen];

   static const int  SizeOfLen = ((maxLen & 0xff00 == 0) ? 1 : 2);
public:
   typedef std::string  value_type;

   /** @brief �x�s�ɩһݦ��Ϊ��Ŷ�(bytes).
   */
   static const int  RoomSize  = maxLen + SizeOfLen;

   /** @brief [�x�s�Ŷ�]�y�z���󪺼ƶq
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

