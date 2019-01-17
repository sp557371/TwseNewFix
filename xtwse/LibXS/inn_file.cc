//
// rr_inn_file<> �� member functions ���g�b�o��
//
// ����: $Id: inn_file.cc,v 1.5 2004/07/21 00:48:36 fonwin Exp $
//
//---------------------------------------------------------------------------
// �z������ include �o����
// �o���ɮ����ӥ� "inn_file.h" include
//---------------------------------------------------------------------------
#include <stdlib.h>  //�������� free(), calloc() �ݭn.
LIBNS_gd(const char)  nullbytes[128];
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos, class RoomSize>
K_ctor  rr_inn_file<RoomPos,RoomSize>::rr_inn_file  (const std::string& fname, rr_size room_size, bool can_check_out, bool isReadonly)
   : storage_(fname.c_str(), isReadonly ? Kway::fmRead : Kway::TFileMode(Kway::fmRandomAccess|Kway::fmCreatePath|Kway::fmOpenAlways)),
     can_check_out_(can_check_out)
{
   // �b�Y�ǯS�����p�U,�i�H:
   //   ���s room_info : �û����|check out�B�C���ж����j�p�T�w
   //   ���s inn_info  : �û����|check out(�]�ж��j�p�i�ѫغc�ɰѼƨM�w,���ݭn�x�s)
   if(can_check_out) {
      inn_info_reserve_ = sizeof(nullbytes);
      keep_room_status_ = true;
      //���\ check_out (�n�x�sroom.status)
      //  �ж��j�p�i��(�n�x�sroom.size)
      //  �ж��j�p�T�w(�����x�sroom.size);
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
      //�����\ check_out: �@����i��,�N�û����O�ڪ��H��! ����!
      inn_info_reserve_ = 0;
      keep_room_status_ = false;
      //�����\ check_out (�����x�sroom.status)
      //  �ж��j�p�i��(�n�x�sroom.size)
      //  �ж��j�p�T�w(�����x�sroom.size)
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
      //�o���p�ȴ̪��ж��e�Ǥ��U�o��ȭ�
      throw storage_error("rr_inn::check_in(" + storage_name() + ") <room_size> error!");
   //��� check in ���ȭ������t�@�ӥN��[�s�ж�]�� key
   //����ڱN�F���i��into_room()��,�~��ж��\�_��!
   return rr_key(rid_new, needed_size);
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_mf(bool)  rr_inn_file<RoomPos,RoomSize>::check_out  (rr_key room_key)
{
   if(room_key.get_room_id() == static_cast<rr_room_id>(rid_new))//�٨S��i�h,�N���n��
      return true;
   if(can_check_out_) {
      // ��C���c:�k�٩ж�(B)
      //  �ܰʫe first_free(A1)-(A2)...
      //  step1: (B)-(A1)-(A2)...
      rr_room_info   roomi;
      roomi.status_     = rrs_free;
      roomi.free_.next_ = inn_info_.first_free_;
      roomi.free_.size_ = room_key.get_room_size();
      if(put_room_info(room_key.get_room_id(), roomi)) {
         //�ܰʫ� first_free(B)-(A1)-(A2)...
         //�N free-list �� first, ���V���k�٪������ж�
         inn_info_.first_free_ = room_key.get_room_id();
         if(seek_room(static_cast<rr_room_id>(rid_first_free)))
            return storage_write(&inn_info_.first_free_, sizeof(inn_info_.first_free_));
      }
   }
   //���� check_out!
   //�Y[�ж��j�p�i��]�b�Ĥ@�����t�ж���,�N����A���ܤj�p�F!
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
   if(rid_next < static_cast<rr_room_id>(rid_1st)) //�L�Ī� room_id
      return false;
   rid_next += room.get_room_size();
   for(;;) {
      rid_next += room_info_reserve_;
      rr_room_info roomi;
      if(get_room_info(rid_next, roomi)) {
         if(roomi.status_ & rrs_free) {//�ũж�,�~���U�@��
            rid_next += roomi.free_.size_;
            continue;
         }
         //���F���H���ж�
         this->set_room_key(room, rr_key(rid_next,roomi.size_));
         return true;
      } else {//�L�k���X�ж����y�z���,
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
            //[�ťX�ж�]����T
            if(keep_room_status_)
               return storage_write(&roomi, static_cast<rr_size>(room_info_reserve_ + sizeof(roomi.free_.next_)));
            //�ж��ťX�ӤF,�� room_info ���x�s[�ťX�ж�]����T??
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
      //�S���ũж��F(or����check_out),
      //���W�\�@��,�@�w�n���W��i��,�_�h�U�@�ӷs�ȭ��|���t��ۦP�� room_id
__alloc_new:
      rr_room_id   new_room_id = static_cast<rr_room_id>(storage_.GetFileSize());
      if(new_room_id < static_cast<rr_room_id>(inn_info_reserve_))
         new_room_id = inn_info_reserve_;
      rr_size  minsz = static_cast<rr_size>(can_check_out_ ? sizeof(rr_room_id)//�ݭn�O�d[�U�@�Ӫũ�id]���Ŷ�
                                                           : 1);
      if(needed_size < minsz)
         needed_size = minsz;
      return rr_key((rr_pos)(new_room_id + rid_1st), needed_size);
   }
   //�٦��ũж�,��@����o�U���a
   rr_room_info   roomi;
   rr_room_id     prev_rid = static_cast<rr_room_id>(rid_first_free);//�W�@���x�s free_.next_ ���a��
   roomi.free_.next_ = inn_info_.first_free_;
   do {
      rr_room_id  cur_rid = roomi.free_.next_;
      if(get_room_info(cur_rid, roomi)) {
         if(roomi.status_ & rrs_free) {
            //���ж��T��S���H��
            if(roomi.free_.size_ < needed_size) {
               //�Ŷ�����,��U�@���ũж��ո�
            } else {
               //���i�Ϊ��ũж��F
               if(seek_room(prev_rid)) {
                  //�]���o���ж��n�Q���t�X�h�F
                  //�ҥH�e�@���ũ�[���V�o���n�Q���t�X�h���ж�]���a��,�n�令�o���ж����U�@���ũ�
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
            //���ж���������H,�����bfree list�̭�: free list�����D: �۰ʭץ� free list.
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
      if(roomi.free_.next_ == static_cast<rr_room_id>(rid_new))//�������ũж�,�S���@���������j�p�e�Ƿs�ȭ�
         goto __alloc_new;            //���N�\�@�����L�a
      prev_rid = cur_rid;
      if(keep_room_status_)
         prev_rid += (rr_pos)sizeof(roomi.status_);
   } while(1);
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
K_mf(bool)  rr_inn_file<RoomPos,RoomSize>::into_room  (rr_key& key, rr_room& room, const rr_live_into* begin, const rr_live_into* end)
{  //�s�ȭ��n�i�Ӧ��o!
   if(!storage_)//�ȴ̻\���_�� :(
      return false;
   rr_size  needed_size = rr_live_obj_calc_size<rr_size>(begin, end);
   if(needed_size==0)//�S������F��n��i��
      return true;
   bool  isNewRoom = (room.get_room_id() == static_cast<rr_room_id>(rid_new));
   if(inn_info_.room_size_ > 0) { //���ȴ̥������ж����O�T�w�j�p
      if(needed_size > inn_info_.room_size_)  //�ж��j�p����
         return false;
      needed_size = inn_info_.room_size_;
   } else //���ȴ̪��ж��j�p�i��
   if(!isNewRoom) { //���ЫȤw�g�����t�@���F,�ˬd���ж��Ŷ��O�_����
      if(needed_size > room.get_room_size()) {//�{�b���o��,�w�g���U�F
         //���t�s���@�����o��ȭ��a
         key = alloc_room(needed_size);
         check_out(room.get_key());//���ª��ж��k�٧a
         this->set_room_key(room, key);
      } else
         needed_size = room.get_room_size();
   }
   if(isNewRoom) //�s�\���ж�
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
         //���ж��٦��Ѿl�Ŷ�,��Jnull byte
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
   if(room.get_room_id()==static_cast<rr_room_id>(rid_new))//�ж��٨S�\�n�O,������X�̭�����ư�!
      return NULL;
   rr_room_info roomi;
   if(get_room_info(room.get_room_id(),roomi)) {
      //���X���w�ж���info�F
      if(roomi.status_ & rrs_free)//�ũж�,�����X����F�誺��!
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
            //�N�F����X��
            if(!storage_read(begin->obj_ptr_, rdsz)) {
      __read_obj_error:
               throw storage_error("rr_inn::peek_room(" + storage_name() + ") read obj error!");
            }
         } else {
            //skip���Ŷ�
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
   if(inn_info_.room_size_ == 0)//�ж��j�p�i��,�L�k�ΧǸ���� room_key
      return rr_key();
   //�ж��j�p�T�w,�ΧǸ����� room_key
   return rr_key((rr_pos)(room_seqno * (inn_info_.room_size_ + room_info_reserve_) + inn_info_reserve_ + rid_1st),
                 inn_info_.room_size_);
}
/////////////////////////////////////////////////////////////////////////////

