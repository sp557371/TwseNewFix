//---------------------------------------------------------------------------
#ifndef IdxFileH
#define IdxFileH
//---------------------------------------------------------------------------
#include "kpersistent.h"
#include "KFile.h"
#include <map>
#include <vector>
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
enum EIdxSaveFlag
{
   isf_Append,    ///< 附加在最後.
   isf_ClearOther,///< 儲存前先清除,再寫入新資料.
};
/////////////////////////////////////////////////////////////////////////////
/** 索引-資料檔.
*/
LIBNS_class TIdxFile
{
public:
   typedef std::string     TIdxKey;
   struct TRecSaver {
      virtual K_mf(unsigned) GetStoreSize () const = 0;
      //若您有自己的 Persistent, 則可以不理會此處的 per, 但需要再 GetPersistent() 時傳回您自己的 per!
      virtual K_mf(bool)     SaveTo       (TIdxFile&, Kstl::persistent&) const = 0;
      //在 SaveTo() 之後呼叫, 若傳回非NULL, 則使用傳回的 persistent 儲存!
      virtual K_mf(Kstl::persistent*) GetPersistent () const {
         return NULL;
      }
   };
   struct TRecLoader {
      virtual K_mf(bool) LoadFrom (TIdxFile&, const TIdxKey&, const Kstl::persistent&) = 0;
   };

private:
   unsigned                ReserveCount_;
   TFile                   IdxFile_;
   TFile                   DatFile_;
   Kstl::persistent_auto   per;
   bool                    SaveKeysBfClose_;

   typedef Kway::word      TSize;
   struct TBlkHead {
      TFile::size_type  RoomPos_;
      TSize             RoomSize_;     ///< 房間大小(不含索引用的Header).
      TSize             DatSize_;      ///< 資料大小.
      TFile::size_type  NextRoomPos_;  ///< 若Dest的訊息這個房間放不下,則需要下一個房間來存放.
   };
   struct TIdxRec {
      TFile::size_type  IdxPos_;
      TBlkHead          BlkFirst_;
      TBlkHead          BlkLast_;
      TFile::size_type  ReadEndPos_;///< 尚未讀出的範圍: [BlkFirst_..ReadEndPos_]

      inline K_ctor TIdxRec ()   : IdxPos_(0), ReadEndPos_(0), WaitErase_(false) {}

      K_mf(void) UpdateReadEndPos ();

      K_mf(bool) IsWaitErase() const {
         return(WaitErase_ && BlkFirst_.RoomPos_==0 && BlkLast_.RoomPos_==0);
      }
      inline K_mf(void) SetWaitErase(bool value) {
         WaitErase_ = value;
      }
   private:
      bool  WaitErase_;
   };
   typedef std::map<TIdxKey,TIdxRec>   TIdxRecs;
   TIdxRecs IdxRecs_;

   TFile::size_type  FreeList_[1024];

   enum EFreeRoomFlag {
      frf_All         = 0,
      frf_ExcludeFrom = 0x01,
      frf_ExcludeTo   = 0x02,
   };
   K_mf(bool) FreeRoom  (TBlkHead&);
   K_mf(bool) FreeRooms (TBlkHead& from, TBlkHead& to, EFreeRoomFlag = frf_All);
   K_mf(bool) AllocRoom (TBlkHead&, TSize datSize);

   typedef std::vector<TBlkHead> TBlkList;
   K_mf(bool) ReadRoomList (TBlkHead& from, TBlkHead& to, EFreeRoomFlag, TBlkList&);

   static K_mf(unsigned) CalcFreeListIdx (TSize roomSize);

   K_mf(bool) WriteIdx_All       (TIdxRec&, const TIdxKey*);
   K_mf(bool) WriteIdx_FirstRoom (const TIdxRec&);
   K_mf(bool) WriteIdx_LastRoom  (const TIdxRec&);

   K_mf(bool) WriteRoom_All          (const TBlkHead&, const TRecSaver&);
   K_mf(bool) WriteRoom_HeadNextRoom (TBlkHead&, TFile::size_type nextRoomPos);
   K_mf(bool) WriteRoom_AppendMsg    (TBlkHead&, const TRecSaver&);
   K_mf(bool) ReadRoom_Head          (TFile::size_type roomPos, TBlkHead&);

   K_mf(bool) LoadKeys ();
   K_mf(bool) SaveKeys ();
   K_mf(bool) LoadBlk  (TIdxRec&);
   K_mf(bool) Load     (TIdxRecs::iterator, TRecLoader&, bool isLastBlk);
   K_mf(bool) ClearRoom(const TIdxKey&, TIdxRecs::iterator* ifindKey);
   K_mf(bool) ClearIdxKey(TIdxRec&);
public:
   static const TFileMode DefaultFileMode = TFileMode(fmCreatePath|fmRead|fmWrite|fmDenyWrite|fmCheckFileNameCase);
   /** 建構. 索引檔 = fname + ".idx"
             資料檔 = fname + ".dat"
       reserveCount = 保留資料筆數. 保留資料大小 = (第一筆資料大小 * reserveCount);
   */
   K_ctor TIdxFile (const std::string& fname, size_t reserveCount, TFileMode fileMode = DefaultFileMode);

   /** 解構.
   */
   virtual K_dtor ~TIdxFile ();

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

   /** 移動寫入指標: 從現在位置往前移動 n bytes.
       cutTail==false: 若 SeekBack() 之後,沒有任何寫入動作,則下次載入時,仍維持SeekBack()之前的資料!
   */
   virtual K_mf(bool) SeekBack (const TIdxKey&, unsigned, bool cutTail);

   /** 刪除某Key全部的資料.
   */
   virtual K_mf(bool) DelKey (const TIdxKey&);

   /** 改變Key值, 若 oldKey 不存在, 或 newKey 已存在, 則傳回 false
   */
   virtual K_mf(bool) ReKey (const TIdxKey& oldKey, const TIdxKey& newKey);

   /** 判斷某Key是否存在.
   */
   virtual K_mf(bool) IsKeyExists (const TIdxKey&) const;

   /** 檔案開啟是否成功?
   */
   virtual K_mf(bool) IsOpened()  const   { return IdxFile_.IsOpened()  &&  DatFile_.IsOpened(); }
   inline  K_mf(bool) operator!() const   { return !IsOpened(); }
   /** 索引檔的錯誤訊息.
   */
   virtual K_mf(std::string) GetLastErr() const {
      std::string errMsg = IdxFile_.GetLastErr();
      if(!errMsg.empty())
         return errMsg;
      return DatFile_.GetLastErr();
   }
   inline K_mf(std::string) GetFileName() const {
      return DatFile_.GetFileName();
   }
   inline K_mf(TFileMode) GetFileMode() const {
      return IdxFile_.GetFileMode();
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

