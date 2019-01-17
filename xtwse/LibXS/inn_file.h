//
// template rr_inn_file<> :用file的方式實做出的客棧class
//
//---------------------------------------------------------------------------
#ifndef inn_fileH
#define inn_fileH
//---------------------------------------------------------------------------
#include "KFile.h"
#include "inn.h"
#include <stdio.h>// ::remove()
#include <malloc.h>
//---------------------------------------------------------------------------
// rr_inn 的一個實作, 將客倌的東西存放到檔案裡
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos, class RoomSize>
class rr_inn_file : public rr_inn<RoomPos,RoomSize>
{
   typedef rr_inn<RoomPos,RoomSize>       inherited;
   typedef rr_inn_file<RoomPos,RoomSize>  this_type;
public:
   typedef typename inherited::rr_room       rr_room;
   typedef typename inherited::rr_key        rr_key;
   typedef typename inherited::rr_pos        rr_pos;
   typedef typename inherited::rr_size       rr_size;
   typedef typename inherited::rr_live_into  rr_live_into;
   typedef typename inherited::rr_live_peek  rr_live_peek;

   typedef typename inherited::storage_error          storage_error;  //避免 gcc 4.0 的錯誤訊息.
   typedef typename inherited::check_out_not_allowed  check_out_not_allowed;
   typedef typename inherited::rr_error               rr_error;
   #ifndef __BORLANDC__
   using inherited::end;
   #endif
//---------------------------------------------------------------------------
//class rr_inn_file : public rr_inn
//{
//   typedef rr_inn                inherited;
//public:
   typedef typename inherited::iterator   iterator;

   K_ctor  rr_inn_file  (const std::string& fname, rr_size room_size, bool can_check_out = true, bool isReadonly = false);
   K_ctor  rr_inn_file  (const std::string& fname, rr_size room_size, bool openAlways,
                         const char* newLineSplit, rr_size skipHeadSize=0);//read only
   K_dtor ~rr_inn_file  ();

   K_mf(rr_key)    check_in          (rr_size needed_size);
   K_mf(bool)      check_out         (rr_key);
   K_mf(iterator)  begin             ();
   K_mf(void)      clear             ();
   K_mf(bool)      remove            ();
   K_mf(rr_key)    get_map_room_key  (rr_pos room_seqno);

   inline K_mf(bool) is_opened () const   { return storage_.IsOpened(); }
   inline K_mf(bool) is_dynamic_room_size() const {
      return keep_room_size_;
   }

private:
   K_mf(bool)  peek_next  (rr_room&);
   K_mf(bool)  into_room  (rr_key&, rr_room&, const rr_live_into* begin, const rr_live_into* end);
   K_mf(const rr_live_peek*)  peek_objs  (const rr_room&, const rr_live_peek* begin, const rr_live_peek* end, rr_size* lastsz);

   //------------------------------------------------------------------------
   #ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(push,1)
   #else
   #pragma pack(1)
   #endif
   //---------------------------------------------------------------------------
   typedef rr_pos    rr_room_id;
   //------------------------------------------------------------------------
   #ifdef K_EnumPacked_Bug
      typedef char   rr_status;
      enum rr_status_t
   #else
      enum rr_status
   #endif
   {
      rrs_normal = 0,
      rrs_free   = 1,
   } K_EnumPacked;
   //------------------------------------------------------------------------
   struct rr_room_info {
      rr_status      status_;//若不能check_out,則此欄位不用儲存
      union {
         //struct { //當 !(status_ & rrs_free) 時, 則此使用此結構
            rr_size  size_;//實際可用空間,不包含[額外資訊]的大小,若房間大小固定,則不用儲存此欄
         //};
         struct { //當 (status_ & rrs_free)==rrs_free 時, 則此使用此結構
            rr_room_id  next_;
            rr_size     size_;//若房間大小固定,則不用儲存此欄
         }           free_;
      };
   } K_StructPacked;
   //------------------------------------------------------------------------
   struct rr_inn_info
   {
      rr_room_id  first_free_;
      rr_size     room_size_;  //room_size_ == 0,表示每筆資料為變動大小
   } K_StructPacked;
   //           ↓這兒應該使用 rr_room_id, 可是 BCB 會說 static const member 必須是 integer type
   static const int  rid_new = 0;         //代表新房間的 room_id
   static const int  rid_1st = 1;         //第一間房間的代號
   static const int  rid_first_free =     //存放 inn_info_.first_free_ 的 room_id
                                            #if defined(__GNUC__)
                                               0
                                            #else
                                               fldoffset(rr_inn_file::rr_inn_info, first_free_)
                                            #endif
                                               + rid_1st;
   //------------------------------------------------------------------------
   #ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
   #else
   #pragma pack()
   #endif
   //---------------------------------------------------------------------------
   rr_inn_info inn_info_;
   Kway::TFile storage_;
   bool        can_check_out_;
   rr_size     inn_info_reserve_;
   bool        keep_room_status_;
   bool        keep_room_size_;
   rr_size     room_info_reserve_;

inline K_mf(bool)  seek_room     (rr_room_id rid)   { rid -= (rr_pos)rid_1st; return(storage_.Seek(rid, Kway::fsBegin)==rid); }
inline K_mf(bool)  storage_write (const void* data, rr_size size)  { return(storage_.Write(data, size)==size); }
inline K_mf(bool)  storage_read  (      void* data, rr_size size)  { return(storage_.Read (data, size)==size); }
inline K_mf(bool)  storage_eof   ()                                { return(storage_.IsEOF()); }
inline K_mf(std::string)  storage_name () const                          { return(storage_.GetFileName()); }

   K_mf(bool)    get_room_info  (rr_room_id, rr_room_info&);
   K_mf(bool)    put_room_info  (rr_room_id, const rr_room_info&);
   K_mf(rr_key)  alloc_room     (rr_size&);
//};
//---------------------------------------------------------------------------
}; // rr_inn_file<>
/////////////////////////////////////////////////////////////////////////////
#include "inn_file.cc"
/////////////////////////////////////////////////////////////////////////////
/* 避免在 AIX 5.3 / gcc 4.2.3 在處理 Signon/UserRemoteMgrPolicy.cpp 時:
   會造成 &(inn::storage_) 的 address 異常!
*/
template<class TInn> class TTgccBug { TInn Inn_; };
typedef Kstl::rr_inn_file<Kway::dword,Kway::word>  TCommonInn1;
class gccBugX1 : public TTgccBug<TCommonInn1> {};
/////////////////////////////////////////////////////////////////////////////
LIBNS_EXTERN_TEMPLATE_CLASS   Kstl::rr_inn_file<Kway::dword, Kway::word>;
LIBNS_EXTERN_TEMPLATE_CLASS   Kstl::rr_inn_file<Kway::dword, Kway::byte>;
LIBNS_EXTERN_TEMPLATE_CLASS   Kstl::rr_inn_file<Kway::word,  Kway::word>;
LIBNS_EXTERN_TEMPLATE_CLASS   Kstl::rr_inn_file<Kway::word,  Kway::byte>;
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
#endif

