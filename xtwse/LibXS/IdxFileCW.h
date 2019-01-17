//---------------------------------------------------------------------------
#ifndef IdxFileCWH
#define IdxFileCWH
//---------------------------------------------------------------------------
#include "IdxFile.h"
#include "cc++/thread.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
/** 索引-資料檔.
    Catch Write.
    使用Thread寫入資料! 全部的 TIdxFileCW 共用n個Thread.

*/
LIBNS_class TIdxFileCW : public TIdxFile
{
   typedef TIdxFile  inherited;
private:
   unsigned             ReserveCount_;
   ost::Mutex mutable   Mutex_;

   class LockerCW {
      ost::Mutex* Mutex_;
   public:
      inline K_ctor LockerCW(const TIdxFileCW& v)
         : Mutex_(&v.Mutex_)
         { Mutex_->enterMutex();
         }
      inline K_dtor ~LockerCW() {
         if(Mutex_)
            Mutex_->leaveMutex();
      }
   };
   friend class LockerCW;

   class ThreadCW;
   friend class ThreadCW;

   struct PendingRec : public TRecSaver {
      EIdxSaveFlag            SaveFlag_;
      Kstl::persistent_auto   mutable per;
      unsigned                MaxSize_;
      inline K_mf(void) Clear (EIdxSaveFlag flag) {
         per.rewind();
         SaveFlag_ = flag;
      }
      inline K_ctor PendingRec ()
         : SaveFlag_(isf_Append)
         , per(1024)
         { }
      inline K_ctor PendingRec (const PendingRec& r)
         : SaveFlag_(isf_Append)
         , per(1024)
         { }
   private:
      inline void   operator=  (const PendingRec&);//禁止複製!

      virtual K_mf(unsigned) GetStoreSize () const {
         return per.get_saved_size();
      }
      //若您有自己的 Persistent, 則可以不理會此處的 per, 但需要再 GetPersistent() 時傳回您自己的 per!
      virtual K_mf(bool) SaveTo (TIdxFile&, Kstl::persistent&) const {
         return true;
      }
      //在 SaveTo() 之後呼叫, 若傳回非NULL, 則使用傳回的 persistent 儲存!
      virtual K_mf(Kstl::persistent*) GetPersistent () const {
         return &per;
      }
   };
   typedef std::map<TIdxKey,PendingRec>   TPendings;
   TPendings            Pendings_;
   TPendings::iterator  ThrIPending_;

   inline K_mf(bool) FindPending (const TIdxKey& key, TPendings::iterator& i) {
      i = Pendings_.find(key);
      return i != Pendings_.end();
   }
   inline K_mf(bool) FlushPending (const TIdxKey& key) {
      TPendings::iterator i;
      if(FindPending(key, i))
         return FlushPending(*i);
      return true;
   }
   inline K_mf(bool) FlushPending (TPendings::value_type& v) {
      return FlushPending(v.first, v.second);
   }
   K_mf(bool) FlushPending (const TIdxKey&, PendingRec&);
   K_mf(void) ThrFlush     (ThreadCW* thr);

public:
   /** 建構. 索引檔 = fname + ".idx"
             資料檔 = fname + ".dat"
       reserveCount = 保留資料筆數. 保留資料大小 = (第一筆資料大小 * reserveCount);
   */
   inline K_ctor TIdxFileCW (const std::string& fname, size_t reserveCount, TFileMode fileMode = DefaultFileMode)
      : inherited(fname, 1, fileMode)
      , ReserveCount_(reserveCount <= 0 ? 1 : (unsigned)reserveCount)
      { }
   /** 解構.
   */
   virtual K_dtor ~TIdxFileCW () {
      Close();
   }
   /** 將等候寫入的資料,全部寫入後才返回!
   */
   virtual K_mf(bool) Flush ();

   /** 關閉資料檔.
   */
   virtual K_mf(void) Close ();

   /** 將資料寫入檔案.
   */
   virtual K_mf(bool) Save (const TIdxKey&, const TRecSaver&, EIdxSaveFlag);

   /** 載入. 若 key==0 全部載入.
                key!=0 載入指定key的資料.
       isLastBlk = 僅載入最後一個Blk.
   */
   virtual K_mf(bool) Load (const TIdxKey*, TRecLoader&, bool isLastBlk);

   /** 卸載, 卸載後可隨時再載入.
       若找不到指定的key,則傳回false.
   */
   virtual K_mf(bool) Unload (const TIdxKey&);

   /** 移動寫入指標: 從現在位置往前移動 n bytes.(僅能在最後一個blk之內).
   */
   virtual K_mf(bool) SeekBack (const TIdxKey&, unsigned, bool cutTail);

   /** 刪除某Key的全部資料.
   */
   virtual K_mf(bool) DelKey (const TIdxKey&);

   /** 改變Key值, 若 oldKey 不存在, 或 newKey 已存在, 則傳回 false
   */
   virtual K_mf(bool) ReKey (const TIdxKey& oldKey, const TIdxKey& newKey);

   /** 判斷某Key是否存在.
   */
   virtual K_mf(bool) IsKeyExists (const TIdxKey& key) const {
      LockerCW locker(*this);
      return inherited::IsKeyExists(key);
   }
   /** 索引檔的錯誤訊息.
   */
   virtual K_mf(std::string) GetLastErr() const {
      LockerCW locker(*this);
      return inherited::GetLastErr();
   }

   static K_mf(void) InitThread();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

