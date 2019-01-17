/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvCtn_SeqHnd_h_)
#define _Nsa_EvCtn_SeqHnd_h_
//---------------------------------------------------------------------------
#include "ev_container.h"
#include "TimerThread.h"
#include "Nsa_EvCtn.h"
#include "Nsa_UtilComm.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// enum EecshSendResu
/////////////////////////////////////////////////////////////////////////////
enum EecshSendResu
{
   esr_WaitNext,  // 不會自動移到下一個 Key, 必須呼叫 SendNext
   esr_MoveNext,  // 自動移到下一個 Key
};


/////////////////////////////////////////////////////////////////////////////
// class TEvCtnSeqHnd
/////////////////////////////////////////////////////////////////////////////
template <class RecsT, class IterCastT>
class TEvCtnSeqHnd : public Kstl::container_ev_handler
{
   enum ERunStatus
   {
      ers_MoveFirst,    // 要求重新開始
      ers_Total,
   };
   typedef std::bitset<ers_Total>  TRunStatus;

public:
   //==============================================
   // 基本型態
   typedef RecsT                           TRecs;
   typedef IterCastT                       TIterCast;
   typedef TEvCtnSeqHnd<TRecs, IterCastT>  this_type;
   //==============================================
   // 延伸型態
   typedef typename TRecs::key_type     TKey;
   typedef typename TRecs::mapped_type  TRec;
   typedef typename TRecs::iterator     TRecsI;
   typedef typename TRecs::updater      TRecsU;
   //==============================================
   // 使用物件
   typedef TTimer<this_type>  TTimerType;

protected:
   TRecs*         Recs_;
   TRecsI         CurrIter_;
   bool           Sending_;
   TTimerType     Timer_;
   size_t         MoveNextCount_;   // 如果 OnSendData 的回傳值為 esr_MoveNext,
                                    // 若此時距離有效資料要是很遠的話
                                    // 這樣速度會太慢
                                    // 所以利用這個參數加速移動的速度
                                    // 預設為 0 代表不啟用
   TRunStatus     RunStatus_;
   
   // 因為 ev_ary 的 end() 是會變動的
   // 所以改用這個旗標來判斷
   bool           IsIterEnd_;
                                    

public:
           K_ctor  TEvCtnSeqHnd (TRecs*);
   virtual K_dtor ~TEvCtnSeqHnd ();

   // 傳送指定 key 下一筆的資料
   K_mf(bool) SendNext (const TKey&);
   
   // 要求從頭開始
   K_mf(void) SendFirst ();
   
   K_mf(bool) GetUpd (const TKey&, TRecsU&);

protected:
   K_mf(bool) SendBase ();

   // 傳送現在 CurrIter_ 的資料
   K_mf(bool) SendCurr ();

   // 傳送指定 Key 的資料
   K_mf(bool) SendCurr (const TKey&);
   
   // 傳送指定 key 下一筆的資料
   K_mf(bool) SendNext ();

protected:
   //==============================================
   // overwrite Kstl::container_ev_handler virtual function
   K_mf(void) OnContainerBfInsert (const Kstl::ev_container&, const Kstl::ev_iterator&, size_type);
   K_mf(void) OnContainerAfInsert (const Kstl::ev_container&, const Kstl::ev_iterator&, size_type);

   //==============================================
   // TEvMapSeqHnd virtual function
   virtual K_mf(EecshSendResu) OnSendData (const TKey&, const TRec&) = 0;

   //==============================================
   // TEvMapSeqHnd function
   K_mf(void) OnTimer (TTimerBase*);
   
   inline K_mf(void) SetMoveNextCount (size_t count)  { MoveNextCount_ = count; }
   
   inline K_mf(bool) IsSending () const  { return Sending_; }
};
/////////////////////////////////////////////////////////////////////////////
// class TEvArySeqHnd
/////////////////////////////////////////////////////////////////////////////
template <class RecsT>
class TEvArySeqHnd : public TEvCtnSeqHnd<RecsT, TIterCastEvAry<RecsT> >
{
   typedef          TEvCtnSeqHnd<RecsT, TIterCastEvAry<RecsT> >  inherited;
   typedef typename inherited::TRecs                             TRecs;

public:
   inline K_ctor TEvArySeqHnd (TRecs* rs) : inherited(rs)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMapSeqHnd
/////////////////////////////////////////////////////////////////////////////
template <class RecsT>
class TEvMapSeqHnd : public TEvCtnSeqHnd<RecsT, TIterCastEvMap<RecsT> >
{
   typedef          TEvCtnSeqHnd<RecsT, TIterCastEvMap<RecsT> >  inherited;
   typedef typename inherited::TRecs                             TRecs;

public:
   inline K_ctor TEvMapSeqHnd (TRecs* rs) : inherited(rs)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TEvCtnSeqHnd impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvCtnSeqHnd_Impl(return_type)               \
   template <class RecsT, class IterCastT>               \
   K_mf(return_type) TEvCtnSeqHnd<RecsT, IterCastT>::    \
//---------------------------------------------------------------------------

template <class RecsT, class IterCastT>
K_ctor TEvCtnSeqHnd<RecsT, IterCastT>::TEvCtnSeqHnd (TRecs* recs)
   : Recs_         (recs)
   , CurrIter_     (recs->end())
   , Sending_      (false)
   , Timer_        (this)
   , MoveNextCount_(0)
   , IsIterEnd_    (true)
{
   Recs_->add_ev_handler((Kstl::container_ev_handler*)this);
}
//---------------------------------------------------------------------------

template <class RecsT, class IterCastT>
K_dtor TEvCtnSeqHnd<RecsT, IterCastT>::~TEvCtnSeqHnd ()
{
   Recs_->del_ev_handler((Kstl::container_ev_handler*)this);

   Timer_.Stop();
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(void) OnContainerBfInsert (const Kstl::ev_container&, const Kstl::ev_iterator& evi, size_type)
{
   if (CurrIter_ == Recs_->end())
      IsIterEnd_ = true;
   else
      IsIterEnd_ = false;
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(void) OnContainerAfInsert (const Kstl::ev_container&, const Kstl::ev_iterator& evi, size_type)
{
   TRecsI iter = static_cast<const TRecsI&>(evi);
   
   if (Sending_ == false) {
      if (SendBase())
         return;
      
      if (IsIterEnd_) {
         // 代表一開始是空的 or 資料已送到最後一筆了
         CurrIter_ = iter;
      }
      else {
         ++CurrIter_;
      }
      
      SendCurr();
   }
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(bool) SendBase ()
{
   if (RunStatus_.test(ers_MoveFirst)) {
      RunStatus_.reset(ers_MoveFirst);
      
      if (Recs_->empty() == false) {
         CurrIter_ = Recs_->begin();
         SendCurr();
      }
      else {
         CurrIter_ = Recs_->end();
      }

      return true;
   }

   return false;
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(bool) SendCurr ()
{
   size_t count = 0;

   Sending_ = true;

   while (CurrIter_ != Recs_->end()) {
      EecshSendResu resu = OnSendData(TIterCast::GetKey(CurrIter_), TIterCast::GetMapped(CurrIter_));
      
      switch (resu) {
      case esr_WaitNext:  return true;
      case esr_MoveNext:
         {
            if (MoveNextCount_ == 0 || count >= MoveNextCount_) {
               Sending_ = false;
               Timer_.Start(0, &this_type::OnTimer);
               return true;
            }
         
            ++count;
            ++CurrIter_;
         }
         break;
      }
   }

   Sending_ = false;
   return false;
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(bool) SendCurr (const TKey& key)
{
   if (SendBase())
      return true;

   TRecsI iter;

   Sending_ = false;
   // 找到這個 key 的 iterator
   if (CurrIter_ != Recs_->end() && key == TIterCast::GetKey(CurrIter_)) {
      iter = CurrIter_;
   }
   else {
      iter = Recs_->find(key);
      if (iter == Recs_->end()) {
         CurrIter_ = iter;
         return false;
      }
   }
      
   CurrIter_ = iter;
   return SendCurr();
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(bool) SendNext ()
{
   if (SendBase())
      return true;

   Sending_ = false;
   if (CurrIter_ != Recs_->end()) {
      ++CurrIter_;

      return SendCurr();
   }
   return false;
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(bool) SendNext (const TKey& key)
{
   if (SendBase())
      return true;

   TRecsI iter;

   Sending_ = false;
   // 找到這個 key 的 iterator
   if (CurrIter_ != Recs_->end() && key == TIterCast::GetKey(CurrIter_)) {
      iter = CurrIter_;
   }
   else {
      iter = Recs_->find(key);
      if (iter == Recs_->end()) {
         return false;
      }
   }
      
   // 移到下一筆 key
   ++iter;
   if (iter == Recs_->end()) {
      return false;
   }

   CurrIter_ = iter;
   return SendCurr();
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(void) SendFirst ()
{
   // 如果現在是[傳送中], 無法直接修改 CurrIter_
   // 必須要在每個介面中判斷囉
   RunStatus_.set(ers_MoveFirst);

   // 如果現在不是[傳送中], 就直接移到 begin 開始送囉
   if (Sending_ == false) {
      SendBase();
   }
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(bool) GetUpd (const TKey& key, TRecsU& upd)
{
   TRecsI iter;

   // 找到這個 key 的 iterator
   if (CurrIter_ != Recs_->end() && key == TIterCast::GetKey(CurrIter_)) {
      iter = CurrIter_;
   }
   else {
      iter = Recs_->find(key);
      if (iter == Recs_->end()) {
         return false;
      }
   }
      
   upd = Recs_->begin_update(iter);
   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvCtnSeqHnd_Impl(void) OnTimer (TTimerBase*)
{
   Timer_.Stop();

   SendNext();
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvCtn_SeqHnd_h_)
