/////////////////////////////////////////////////////////////////////////////
#if !defined(_UpdInfoList_h_)
#define _UpdInfoList_h_
//---------------------------------------------------------------------------
#include <queue>
#include <map>
#include <cc++\thread.h>

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TUpdInfoList
/////////////////////////////////////////////////////////////////////////////
template <class KeyT, class DatT = KeyT>
class TUpdInfoList
{
   typedef KeyT  TKey;
   typedef DatT  TDat;

   typedef std::map<TKey, TDat>   TListMap;
   typedef TListMap::iterator     TListMapI;
   typedef std::queue<TListMapI>  TListSeq;

   TListSeq    ListSeq_;
   TListMap    ListMap_;
   ost::Mutex  Mutex_;
   bool        IsUpdDat_;

public:
   inline K_ctor TUpdInfoList (bool IsUpdDat = true)
      : IsUpdDat_(IsUpdDat)
   {}

   K_mf(bool) insert    (const TKey& Key, const TDat& Dat);

   K_mf(bool) front_pop (                    );
   K_mf(bool) front_pop (TKey& Key           );
   K_mf(bool) front_pop (           TDat& Dat);
   K_mf(bool) front_pop (TKey& Key, TDat& Dat);

   K_mf(void) clear     ();

   inline K_mf(size_t) size () const  { return ListSeq_.size(); }
};
/////////////////////////////////////////////////////////////////////////////
// class TUpdInfoList impl
/////////////////////////////////////////////////////////////////////////////
#define TUpdInfoList_Impl(return_type)                                     \
   template <class KeyT, class DatT>                                       \
   K_mf(return_type) TUpdInfoList<KeyT, DatT>::                            \
//---------------------------------------------------------------------------

#ifdef __BORLANDC__
   #define TUpdInfoList_Impl_S(return_type)                                \
      template <class KeyT, class DatT>                                    \
      TUpdInfoList<KeyT, DatT>::return_type __fastcall                     \
      TUpdInfoList<KeyT, DatT>::                                           \
   //------------------------------------------------------------------------
#else
   #define TUpdInfoList_Impl_S(return_type)                                \
      template <class KeyT, class DatT>                                    \
      typename TUpdInfoList<KeyT, DatT>::return_type                       \
      TUpdInfoList<KeyT, DatT>::                                           \
   //------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

TUpdInfoList_Impl(bool) insert (const TKey& Key, const TDat& Dat)
{
   ost::MutexLock Locker(Mutex_);
   TListMapI      iter = ListMap_.lower_bound(Key);

   if (iter == ListMap_.end()                ||
       ListMap_.key_comp()(iter->first, Key) ||
       ListMap_.key_comp()(Key, iter->first))
   {
      std::pair<TListMapI, bool> ins = ListMap_.insert(std::make_pair(Key, Dat));

      if (ins.second == false)
         return false;

      // 資料不存在, 要加入異動清單順序裡面
      ListSeq_.push(ins.first);

      return true;
   }

   // 資料已存在, 更新 dat 即可
   if (IsUpdDat_)
      iter->second = Dat;

   return false;

   /*
   // 先去 map 裡面找找看有沒有這筆資料
   std::pair<TListMapI, bool> Ins = ListMap_.insert(std::make_pair(Key, Dat));

   if (Ins.second == false) {
      // 資料已存在, 更新 dat 即可
      if (IsUpdDat_) {
         Ins.first->second = Dat;
      }
      return false;
   }

   // 資料不存在, 要加入異動清單順序裡面
   ListSeq_.push(Ins.first);
   */
}
//---------------------------------------------------------------------------

TUpdInfoList_Impl(bool) front_pop ()
{
   ost::MutexLock Locker(Mutex_);

   if (ListSeq_.empty()) {
      return false;
   }

   TListMapI iter = ListSeq_.front();

   ListSeq_.pop();
   ListMap_.erase(iter);

   return true;
}
//---------------------------------------------------------------------------

TUpdInfoList_Impl(bool) front_pop (TKey& Key)
{
   ost::MutexLock Locker(Mutex_);

   if (ListSeq_.empty()) {
      return false;
   }

   TListMapI iter = ListSeq_.front();

   Key = iter->first;
   ListSeq_.pop();
   ListMap_.erase(iter);

   return true;
}
//---------------------------------------------------------------------------

TUpdInfoList_Impl(bool) front_pop (TDat& Dat)
{
   ost::MutexLock Locker(Mutex_);

   if (ListSeq_.empty()) {
      return false;
   }

   TListMapI iter = ListSeq_.front();
   Dat = iter->second;

   ListSeq_.pop();
   ListMap_.erase(iter);

   return true;
}
//---------------------------------------------------------------------------

TUpdInfoList_Impl(bool) front_pop (TKey& Key, TDat& Dat)
{
   ost::MutexLock Locker(Mutex_);

   if (ListSeq_.empty())
      return false;

   TListMapI iter = ListSeq_.front();

   Dat = iter->second;

   ListSeq_.pop();
   ListMap_.erase(iter);

   return true;
}
//---------------------------------------------------------------------------

TUpdInfoList_Impl(void) clear ()
{
   ost::MutexLock Locker(Mutex_);

   while (ListSeq_.empty() == false)
      ListSeq_.pop();

   ListMap_.clear();
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_UpdInfoList_h_)

