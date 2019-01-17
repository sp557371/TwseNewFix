//
// template rr_inn_file<> :��file���覡�갵�X���ȴ�class
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
// rr_inn ���@�ӹ�@, �N�ȭ����F��s����ɮ׸�
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl �R�W�W�h����:�аѦ� "ev_container.h"
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

   typedef typename inherited::storage_error          storage_error;  //�קK gcc 4.0 �����~�T��.
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
      rr_status      status_;//�Y����check_out,�h����줣���x�s
      union {
         //struct { //�� !(status_ & rrs_free) ��, �h���ϥΦ����c
            rr_size  size_;//��ڥi�ΪŶ�,���]�t[�B�~��T]���j�p,�Y�ж��j�p�T�w,�h�����x�s����
         //};
         struct { //�� (status_ & rrs_free)==rrs_free ��, �h���ϥΦ����c
            rr_room_id  next_;
            rr_size     size_;//�Y�ж��j�p�T�w,�h�����x�s����
         }           free_;
      };
   } K_StructPacked;
   //------------------------------------------------------------------------
   struct rr_inn_info
   {
      rr_room_id  first_free_;
      rr_size     room_size_;  //room_size_ == 0,��ܨC����Ƭ��ܰʤj�p
   } K_StructPacked;
   //           ���o�����Өϥ� rr_room_id, �i�O BCB �|�� static const member �����O integer type
   static const int  rid_new = 0;         //�N��s�ж��� room_id
   static const int  rid_1st = 1;         //�Ĥ@���ж����N��
   static const int  rid_first_free =     //�s�� inn_info_.first_free_ �� room_id
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
/* �קK�b AIX 5.3 / gcc 4.2.3 �b�B�z Signon/UserRemoteMgrPolicy.cpp ��:
   �|�y�� &(inn::storage_) �� address ���`!
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

