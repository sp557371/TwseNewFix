//
// rr_inn_file<> 的 member functions 都寫在這兒
//
// 版本: $Id: inn_file.cc,v 1.5 2004/07/21 00:48:36 fonwin Exp $
//
//---------------------------------------------------------------------------
// 您不應該 include 這個檔
// 這個檔案應該由 "inn_file.h" include
//---------------------------------------------------------------------------
#include <stdlib.h>  //有些環境 free(), calloc() 需要.
LIBNS_gd(const char)  nullbytes[128];
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos, class RoomSize>
K_ctor  rr_inn_file<RoomPos,RoomSize>::rr_inn_file  (const std::string& fname, rr_size room_size, bool can_check_out, bool isReadonly)
   : storage_(fname.c_str(), isReadonly ? Kway::fmRead : Kway::TFileMode(Kway::fmRandomAccess|Kway::fmCreatePath|Kway::fmOpenAlways)),
     can_check_out_(can_check_out)
{
   // 在某些特殊的情況下,可以:
   //   不存 room_info : 永遠不會check out、每間房間的大小固定
   //   不存 inn_info  : 永遠不會check out(因房間大小可由建構時參數決定,不需要儲存)
   if(can_check_out) {
      inn_info_reserve_ = sizeof(nullbytes);
      keep_room_status_ = true;
      //允許 check_out (要儲存room.status)
      //  房間大小可變(要儲存room.size)
      //  房間大小固定(不用儲存room.size);
      keep_room_size_ = (room_size == 0);
      rr_size   rdsz = 0;
      if(storage_.IsOpened()) {
         rdsz = static_cast<rr_size>(storage_.Read(&inn_info_, sizeof(inn_info_)));
         if(rdsz > 0  &&  room_size != inn_info_.room_size_)
            throw storage_error("rr_inn::constructor(" + fname + ") storage's <room_size> error!");
      }
      memset(((Kway::byte*)&inn_info_)+rdsz, 0, sizeof(inn_info_)-rdsz);
      inn_info_.room_size_ = room_size;
      if(storage_.IsOpened()  &&  rdsz < inn_info_reserve_) {
         rr_pos  ss = static_cast<rr_pos>(storage_.GetFileSize());
         if(ss < static_cast<rr_pos>(inn_info_reserve_)) {
            storage_.Seek(0,Kway::fsBegin);
            storage_.Write(&inn_info_, sizeof(inn_info_));
            storage_.Write(nullbytes,  inn_info_reserve_-sizeof(inn_info_));
         }
      }
   } else {
      //不允許 check_out: 一旦住進來,就永遠都是我的人啦! 哈哈!
      inn_info_reserve_ = 0;
      keep_room_status_ = false;
      //不允許 check_out (不用儲存room.status)
      //  房間大小可變(要儲存room.size)
      //  房間大小固定(不用儲存room.size)
      keep_room_size_ = (room_size == 0);
      memset(&inn_info_, 0, sizeof(inn_info_));
      inn_info_.room_size_ = room_size;
   }
   room_info_reserve_ = static_cast<rr_size> ( (keep_room_status_ ? sizeof(rr_status) : 0)
                                             + (keep_room_size_   ? sizeof(rr_size)   : 0) );
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_ctor  rr_inn_file<RoomPos,RoomSize>::rr_inn_file  ( const std::string& fname, rr_size room_size
                                                    , bool openAlways, const char* //newLineSplit
                                                    , rr_size skipHeadSize)
   : storage_( fname.c_str(), openAlways ? Kway::TFileMode(Kway::fmReadSeq|Kway::fmCreatePath|Kway::fmOpenAlways)
                                         : Kway::TFileMode(Kway::fmReadSeq) ),
     can_check_out_(false),
     inn_info_reserve_(skipHeadSize),
     keep_room_status_(false),
     keep_room_size_(false),
     room_info_reserve_(0)
{
   memset(&inn_info_, 0, sizeof(inn_info_));
   inn_info_.room_size_ = room_size;
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_dtor  rr_inn_file<RoomPos,RoomSize>::~rr_inn_file  ()
{
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
#ifdef __BORLANDC__
       rr_inn_file<RoomPos,RoomSize>::rr_key  __fastcall
#else
typename rr_inn_file<RoomPos,RoomSize>::rr_key
#endif
         rr_inn_file<RoomPos,RoomSize>::check_in  (rr_size needed_size)
{
   if(needed_size==0)//use default size
      needed_size = inn_info_.room_size_;
   else
   if(inn_info_.room_size_ != 0
   && inn_info_.room_size_ <  needed_size)
      //這間小客棧的房間容納不下這位客倌
      throw storage_error("rr_inn::check_in(" + storage_name() + ") <room_size> error!");
   //剛剛 check in 的客倌先分配一個代表[新房間]的 key
   //等實際將東西般進來into_room()時,才把房間蓋起來!
   return rr_key(rid_new, needed_size);
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_mf(bool)  rr_inn_file<RoomPos,RoomSize>::check_out  (rr_key room_key)
{
   if(room_key.get_room_id() == static_cast<rr_room_id>(rid_new))//還沒住進去,就不要啦
      return true;
   if(can_check_out_) {
      // 串列結構:歸還房間(B)
      //  變動前 first_free(A1)-(A2)...
      //  step1: (B)-(A1)-(A2)...
      rr_room_info   roomi;
      roomi.status_     = rrs_free;
      roomi.free_.next_ = inn_info_.first_free_;
      roomi.free_.size_ = room_key.get_room_size();
      if(put_room_info(room_key.get_room_id(), roomi)) {
         //變動後 first_free(B)-(A1)-(A2)...
         //將 free-list 的 first, 指向剛歸還的那間房間
         inn_info_.first_free_ = room_key.get_room_id();
         if(seek_room(static_cast<rr_room_id>(rid_first_free)))
            return storage_write(&inn_info_.first_free_, sizeof(inn_info_.first_free_));
      }
   }
   //不能 check_out!
   //若[房間大小可變]在第一次分配房間後,就不能再改變大小了!
   throw check_out_not_allowed("rr_inn_file::check_out(" + storage_name() + ") not allowed!");
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
#ifdef __BORLANDC__
         rr_inn_file<RoomPos,RoomSize>::iterator __fastcall
#else
typename rr_inn_file<RoomPos,RoomSize>::iterator
#endif
         rr_inn_file<RoomPos,RoomSize>::begin  ()
{
   rr_room  result(*this, rr_key(rid_1st, static_cast<rr_size>(inn_info_reserve_ - room_info_reserve_)));
   peek_next(result);
   return iterator(result);
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_mf(bool)  rr_inn_file<RoomPos,RoomSize>::peek_next  (rr_room& room)
{
   rr_room_id  rid_next = room.get_room_id();
   if(rid_next < static_cast<rr_room_id>(rid_1st)) //無效的 room_id
      return false;
   rid_next += room.get_room_size();
   for(;;) {
      rid_next += room_info_reserve_;
      rr_room_info roomi;
      if(get_room_info(rid_next, roomi)) {
         if(roomi.status_ & rrs_free) {//空房間,繼續找下一個
            rid_next += roomi.free_.size_;
            continue;
         }
         //找到了有人住的房間
         this->set_room_key(room, rr_key(rid_next,roomi.size_));
         return true;
      } else {//無法取出房間的描述資料,
         this->set_room_key(room, end()->get_key());
         return false;
      }
   } // for(;;...
}
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos, class RoomSize>
K_mf(bool)  rr_inn_file<RoomPos,RoomSize>::put_room_info  (rr_room_id rid, const rr_room_info& roomi)
{
   if(seek_room(rid)) {
      if(room_info_reserve_ > 0) {
         if(roomi.status_ & rrs_free) {
            //[空出房間]的資訊
            if(keep_room_status_)
               return storage_write(&roomi, static_cast<rr_size>(room_info_reserve_ + sizeof(roomi.free_.next_)));
            //房間空出來了,但 room_info 不儲存[空出房間]的資訊??
            throw rr_error("rr_inn_file::put_room_info(" + storage_name() + ") cannot put check out info!");
         }
         return storage_write( (keep_room_status_ ? static_cast<const void*>(&roomi.status_)
                                                  : static_cast<const void*>(&roomi.size_)),
                              room_info_reserve_ );
      }
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_mf(bool)  rr_inn_file<RoomPos,RoomSize>::get_room_info  (rr_room_id rid, rr_room_info& roomi)
{
   if(seek_room(rid)) {
      if(room_info_reserve_ == 0) {
         roomi.status_ = rrs_normal;
         roomi.size_   = inn_info_.room_size_;
         return true;
      }
      if(keep_room_status_) {
         if(storage_read(&roomi, room_info_reserve_)) {
            if(roomi.status_ & rrs_free) {
               if(storage_read(((Kway::byte*)(&roomi))+room_info_reserve_, sizeof(roomi.free_.next_))) {
                  if(!keep_room_size_)
                     roomi.free_.size_ = inn_info_.room_size_;
                  return true;
               }
            } else {
               if(!keep_room_size_)
                  roomi.size_ = inn_info_.room_size_;
               return true;
            }
         }
      } else {
         roomi.status_ = rrs_normal;
         return storage_read(&roomi.size_, room_info_reserve_);
      }
   } // if(seek_room...
   return false;
}
/////////////////////////////////////////////////////////////////////////////
template <class rr_size, class live_obj>
K_mf(rr_size)  rr_live_obj_calc_size (const live_obj* begin, const live_obj* end)
{
   rr_size  needed_size = 0;
   while(begin != end) {
      needed_size += begin->obj_size_;
      ++begin;
   }
   return needed_size;
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
#ifdef __BORLANDC__
         rr_inn_file<RoomPos,RoomSize>::rr_key  __fastcall
#else
typename rr_inn_file<RoomPos,RoomSize>::rr_key
#endif
         rr_inn_file<RoomPos,RoomSize>::alloc_room  (rr_size& needed_size)
{
   if(inn_info_.first_free_==static_cast<rr_room_id>(rid_new)  ||  !can_check_out_) {
      //沒有空房間了(or不能check_out),
      //馬上蓋一間,一定要馬上住進來,否則下一個新客倌會分配到相同的 room_id
__alloc_new:
      rr_room_id   new_room_id = static_cast<rr_room_id>(storage_.GetFileSize());
      if(new_room_id < static_cast<rr_room_id>(inn_info_reserve_))
         new_room_id = inn_info_reserve_;
      rr_size  minsz = static_cast<rr_size>(can_check_out_ ? sizeof(rr_room_id)//需要保留[下一個空房id]的空間
                                                           : 1);
      if(needed_size < minsz)
         needed_size = minsz;
      return rr_key((rr_pos)(new_room_id + rid_1st), needed_size);
   }
   //還有空房間,找一間住得下的吧
   rr_room_info   roomi;
   rr_room_id     prev_rid = static_cast<rr_room_id>(rid_first_free);//上一個儲存 free_.next_ 的地方
   roomi.free_.next_ = inn_info_.first_free_;
   do {
      rr_room_id  cur_rid = roomi.free_.next_;
      if(get_room_info(cur_rid, roomi)) {
         if(roomi.status_ & rrs_free) {
            //此房間確實沒有人住
            if(roomi.free_.size_ < needed_size) {
               //空間不足,找下一間空房間試試
            } else {
               //找到可用的空房間了
               if(seek_room(prev_rid)) {
                  //因為這間房間要被分配出去了
                  //所以前一間空房[指向這間要被分配出去的房間]的地方,要改成這間房間的下一間空房
                  if(storage_write(&roomi.free_.next_,sizeof(roomi.free_.next_))) {
                     if(prev_rid == static_cast<rr_room_id>(rid_first_free))
                        inn_info_.first_free_ = roomi.free_.next_;
                     return rr_key(cur_rid, needed_size = roomi.free_.size_);
                  }
                  throw storage_error("rr_inn::into_room(" + storage_name() + ") write free list error!");
               } else
                  goto __throw_err;
            }
         } else {
            //此房間明明有住人,但仍在free list裡面: free list有問題: 自動修正 free list.
            roomi.free_.next_ = static_cast<rr_room_id>(rid_new);
            if(seek_room(prev_rid))
               storage_write(&roomi.free_.next_, sizeof(roomi.free_.next_));
            if(prev_rid == static_cast<rr_room_id>(rid_first_free))
               inn_info_.first_free_ = roomi.free_.next_;
            goto __alloc_new;
         }
      } else {
      __throw_err:
         throw storage_error("rr_inn::into_room(" + storage_name() + ") read free list error!");
      }
      if(roomi.free_.next_ == static_cast<rr_room_id>(rid_new))//全部的空房間,沒有一間有足夠大小容納新客倌
         goto __alloc_new;            //那就蓋一間給他吧
      prev_rid = cur_rid;
      if(keep_room_status_)
         prev_rid += (rr_pos)sizeof(roomi.status_);
   } while(1);
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_mf(bool)  rr_inn_file<RoomPos,RoomSize>::into_room  (rr_key& key, rr_room& room, const rr_live_into* begin, const rr_live_into* end)
{  //新客倌要進來住囉!
   if(!storage_)//客棧蓋不起來 :(
      return false;
   rr_size  needed_size = rr_live_obj_calc_size<rr_size>(begin, end);
   if(needed_size==0)//沒有任何東西要住進來
      return true;
   bool  isNewRoom = (room.get_room_id() == static_cast<rr_room_id>(rid_new));
   if(inn_info_.room_size_ > 0) { //此客棧全部的房間都是固定大小
      if(needed_size > inn_info_.room_size_)  //房間大小不足
         return false;
      needed_size = inn_info_.room_size_;
   } else //此客棧的房間大小可變
   if(!isNewRoom) { //此房客已經有分配一間了,檢查此房間空間是否足夠
      if(needed_size > room.get_room_size()) {//現在住的這間,已經住不下了
         //分配新的一間給這位客倌吧
         key = alloc_room(needed_size);
         check_out(room.get_key());//把舊的房間歸還吧
         this->set_room_key(room, key);
      } else
         needed_size = room.get_room_size();
   }
   if(isNewRoom) //新蓋的房間
      this->set_room_key(room, key = alloc_room(needed_size));

   rr_room_info   roomi;
   roomi.status_ = rrs_normal;
   roomi.size_   = needed_size;
   if(put_room_info(room.get_room_id(),roomi)) {
      while(begin != end) {
         if(!storage_write(begin->obj_ptr_, begin->obj_size_)) {
      __write_obj_error:
            throw storage_error("rr_inn::into_room(" + storage_name() + ") write obj error(disk full?)!");
         }
         if((needed_size -= begin->obj_size_) <= 0)
            break;
         ++begin;
      }
      if(needed_size > 0) {
         //此房間還有剩餘空間,填入null byte
         void* nullspaces = ( needed_size > sizeof(nullbytes)
                              ? calloc(needed_size,1)
                              : 0 );
         if(nullspaces==0)
            do {
               rr_size  wrsz = ( needed_size > sizeof(nullbytes)
                                ? static_cast<rr_size>(sizeof(nullbytes))
                                : needed_size );
               if(!storage_write(nullbytes, wrsz))
                  goto __write_obj_error;
               needed_size = static_cast<rr_size>(needed_size - wrsz);
            } while(needed_size > 0);
         else
            try {
               if(!storage_write(nullspaces, needed_size)) {
                  free(nullspaces);
                  nullspaces = 0;
                  goto __write_obj_error;
               }
               if(nullspaces)
                  free(nullspaces);
            } catch(...) {
               if(nullspaces)
                  free(nullspaces);
               throw;
            }
      }
      return true;
   }
   throw storage_error("rr_inn::into_room(" + storage_name() + ") write status error(disk full?)!");
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
const typename rr_inn_file<RoomPos,RoomSize>::rr_live_peek*
#ifdef __BORLANDC__
   __fastcall
#endif
   rr_inn_file<RoomPos,RoomSize>::peek_objs  (const rr_room& room, const rr_live_peek* begin, const rr_live_peek* end, rr_size* lastsz)
{
   if(room.get_room_id()==static_cast<rr_room_id>(rid_new))//房間還沒蓋好呢,不能取出裡面的資料啦!
      return NULL;
   rr_room_info roomi;
   if(get_room_info(room.get_room_id(),roomi)) {
      //取出指定房間的info了
      if(roomi.status_ & rrs_free)//空房間,拿不出什麼東西的啦!
         return NULL;
      if(storage_eof())
         return NULL;
      while(begin != end) {
         rr_size  rdsz = begin->obj_size_;
         if(roomi.size_ < rdsz) {
            rdsz = roomi.size_;
            if(lastsz)
               *lastsz = rdsz;
            end = 0;
         } else
            roomi.size_ -= rdsz;
         if(begin->obj_ptr_) {
            //將東西取出來
            if(!storage_read(begin->obj_ptr_, rdsz)) {
      __read_obj_error:
               throw storage_error("rr_inn::peek_room(" + storage_name() + ") read obj error!");
            }
         } else {
            //skip此空間
            if(!storage_.IsResultOK(storage_.Seek(rdsz, Kway::fsCurrent)))
               goto __read_obj_error;
         }
         if(end == 0)
            break;
         ++begin;
      }
      return begin;
   }
   throw storage_error("rr_inn::peek_room(" + storage_name() + ") read status error!");
}
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos, class RoomSize>
K_mf(void)  rr_inn_file<RoomPos,RoomSize>::clear  ()
{
   inn_info_.first_free_ = static_cast<rr_room_id>(rid_new);
   storage_.SetFileSize(inn_info_reserve_);
   if(inn_info_reserve_ > 0)
      if(seek_room(static_cast<rr_room_id>(rid_first_free)))
         storage_write(&inn_info_.first_free_, sizeof(inn_info_.first_free_));
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_mf(bool)  rr_inn_file<RoomPos,RoomSize>::remove  ()
{
   if(!storage_.IsOpened())
      return false;
   std::string fname(storage_.GetFileName());
   storage_.SetFileSize(0);
   storage_.Close();
   return ::remove(fname.c_str()) == 0;
}
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos, class RoomSize>
#ifdef __BORLANDC__
         rr_inn_file<RoomPos,RoomSize>::rr_key  __fastcall
#else
typename rr_inn_file<RoomPos,RoomSize>::rr_key
#endif
         rr_inn_file<RoomPos,RoomSize>::get_map_room_key  (rr_pos room_seqno)
{
   if(inn_info_.room_size_ == 0)//房間大小可變,無法用序號找到 room_key
      return rr_key();
   //房間大小固定,用序號產生 room_key
   return rr_key((rr_pos)(room_seqno * (inn_info_.room_size_ + room_info_reserve_) + inn_info_reserve_ + rid_1st),
                 inn_info_.room_size_);
}
/////////////////////////////////////////////////////////////////////////////

