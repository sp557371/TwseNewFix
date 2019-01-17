//
// template rr_inn<>
//
// ����: $Id: inn.h,v 1.3 2006/04/03 14:34:39 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef innH
#define innH
//---------------------------------------------------------------------------
#include <stdexcept>
//---------------------------------------------------------------------------
#include "KwayBase.h"
/////////////////////////////////////////////////////////////////////////////
namespace Kstl {
//
// Kstl �R�W�W�h����:�аѦ� "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
/*
����i�@���ȴ̪��B�J:
  1. ���@���X�A���ȴ�:
       (1) �ȴ̴��Ѫ� room_size �O�_���j
       (2) �O�_���\ check_out (�O�h��,�N�O���ȴ̤����\�A check out)
       (3) �Y�ȴ̴��ѥi�ܤj�p���ж�,�i�O�S�����\�A check out,
           ���Ъ`�N: �Y�z�w�g��i�h�F,�S�Q�n�@�ӧ�j���ж�,
                     �i��|�� rr_inn::check_out_not_allowed �� exception
  2. ��ȴ� check_in(needed_size) �ô��ѱz�ݭn�h�j���ж�:
       (1) �p�G���ȴ̵L�k���ѱz�ݭn���ж�,�h�|���ͩ��U�� exception:
             rr_inn::storage_error("rr_inn::check_in() <room_size> error!");
       (2) �p�G�ȴ̤��\�z��i��,�h�|���z�@���_��,�Ч����O�s
       (3) �����_��:�Y�z���ȴ̴��Ѥ@�ӯS����k���o�_��:get_map_room_key(room_seqno);
                    �h�i�H���� check_in();
  3. �αz���_��,�n�D�ȴ�,�a�z��z���ж�: get_room()
       �b�z�٨S�n�D��i�h���e,�o�өж����i���٥u�O�ӡu�Ť��ӻաv(�ä��s�b��o�ӥ@�ɤW)
       ���L�z���ݭn���,�@���z�n�D��i�h: get_room().into() �o�өж��N�|���p�]�k�몺�u��!�v�X�ӤF
  4. �ש�,�z�i�H��i�z���ж��F:
       (1) �ȴ�.get_room().into(rr_key& key, const rr_live_into* begin, const rr_live_into* end)
       (2) key:�n�i�J�ж�,��M�ݭn��赹�z���_���o!
               ���Ъ`�N:���}�ж���(into()������),key�����e���i��Q���ܳ�!
       (3) begin,end:�b�o�̩w�q,�z�n��i�ж����F��
       (4) �p�G�z�Q�n����[��i�h���F��],�ЦA�@�����ϥ� 4.(1) ����k
  5. rr_live_into�|�ӨҤl�a:
       std::string    MyName;
       #pragma pack(push,1)
       struct {...}   MyMemory;
       #pragma pack(pop)
       ...
       rr_live_into   MySoul[] = { { MyName.c_str(),  MyName.length()+1 },  //�ܰʪ���,�H'\0'��������
                                   { &MyMemory,       sizeof(MyMemory)  } };
       �o�˩w�q���M²��,�����@�ӯ��I,�N�O�n��F����X�ɸ�����K:�]���Ĥ@����Ƭ��ܰʪ���
       ���U���ѤF�t�@�ظ���K���覡:
       int   len = MyName.length();
       if(len > 255)  len = 255;//����MyName�̪�255bytes
       rr_live_into   MySoul[] = { { &MySoul[1].obj_size_,   1                },  //1bytes����MyName������
                                   { MyName.c_str(),         len              },  //�ܰʪ��׸��
                                   { &MyMemory,              sizeof(MyMemory) } };
       �p�G�z��o�˰�,�N�i�j�T²�ƨ��X�ɪ��@�~
       �t�~,�n�бz�B�~�`�N�@�I:�Y�z�n��@�ӵ��c(�p�W�Ҫ�MyMemory),�����O�J��ж���,
         (1) �Х[�W:
             #pragma pack(push,1)   �p�G���[�W����,�o�ӥ@���٬O�i�H�B�檺�ܦn,�u�O�z�i��ݭn��j���ж�
                                    �ӥB�h�X�Ӫ��Ŷ��O���O����
             struct {...}  MyMemory;
             #pragma pack(pop)
         (2) MyMemory�������঳[����]��������(�p: char*�Bint*�Bstd::string�Bstd::vector...)
             �̦n�u�t�� plain old data(�p: int�Bchar�Bfloat...)

���n�F,�{�b�z��i�@�a�ȴ̸̤F,���O��@�ɭ��ͮ�(AP���s����),�n�p���^[�z��i�ȴ̸̪��F��]�O?
  1. �������z�����a�ȴ�
  2. �Q�Ϋȴ̴��Ѫ� begin()�Bend() �uiterator�v�ӿs���C�@���ж�
      (�b�o�دS�O���ɨ�,�мȮɧO��[���p�v]�����D�a!�_�h����z��^�өO?)
  3. �w�q���^�����c: rr_live_peek
     (1) ����H�W���Ҥl�a! �Y�z�ϥβĤ@�ؤ覡:
       rr_inn:iterator  iend = inn.end();   //�zı�o����n�����o iend �O? �Q�@�U�a!
       for(rr_inn:iterator i = inn.begin();  i != iend;  ++i) {
          char           cName;
          rr_live_peek   MySoul[] = { { 0,       0             }, //�o�����X���,�nskip�h�֪Ŷ�
                                      { &cName,  sizeof(cName) } };
          MyName = "";
          //�����XMyName
          while(i->peek(MySoul,MySoul+2)) {  //�@����sizeof(cName)��bytes,�n��n�h��!
             MyName += cName;
             MySoul[0].obj_size_ += MySoul[1].obj_size_;
             if(cName==0)//�ש��MyName�짹�F
                break;
          }
          //���U�Ө��XMyMemory
          MySoul[1].obj_ptr_  = &MyMemory;
          MySoul[1].obj_size_ = sizeof(MyMemory);
          if(i->peek(MySoul,MySoul+2)) {
             //�ѤU�N�OMyName�BMyMemory�������o
             ...
          }
       } //for(i...
      (2) �p�G�βĤG�ؤ覡,�N²��(�]��)�h�F:
       char           cName[256];
       rr_live_peek   MySoul[] = { { &MySoul[1].obj_size_,   1  },
                                   { cName,                  0  },
                                   { &MyMemory,              sizeof(MyMemory) } };
       rr_inn:iterator  iend = inn.end(); //�zı�o,�b�j��̭��@���I�sinn.end()�����,�٬O���Niend�ǳƦn����֩O?
       for(rr_inn:iterator i = inn.begin();  i != iend;  ++i) {
          if(i->peek(MySoul,MySoul+3)) {
             cName[MySoul[1].obj_size_] = 0;//�[�W�r�굲���Ÿ�
             MyName = cName;
             //�ѤU�N�OMyName�BMyMemory�������o
             ...
          }
       }

���t�~,�p�G�z���Q��F(�αz�Q���},�Q�û����}�o�Ӭ��R���@��...)
  1. �����i�D�ȴ̱z�n check_out() �Y�i
  2. �Y�z���ȴ̤����\ check_out�A���z�|�o��@�� rr_inn::check_out_not_allowed �� exception

*/
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos = Kway::uint64, class RoomSize = Kway::dword>
class rr_inn
{
   typedef rr_inn<RoomPos,RoomSize>    this_type;
   typedef this_type                   this_inn;
public:
   //------------------------------------------------------------------------
   #ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(push,1)
   #else
   #pragma pack(1)
   #endif
   //---------------------------------------------------------------------------
   typedef RoomPos         rr_pos;
   typedef RoomSize        rr_size;
   //------------------------------------------------------------------------
   class rr_key
   {  //�֦��@��Ъ����_�� rr_key, �~�i�i�J�@�� room
      typedef  rr_key      this_type;
      rr_pos   room_id_;
      rr_size  room_size_;
   public:
      K_ctor  rr_key  ()
               : room_id_(rr_pos(-1)), room_size_(0)
               { }
      K_ctor  rr_key  (rr_pos room_id, rr_size room_size)
               : room_id_(room_id), room_size_(room_size)
               { }
      K_mf(rr_pos)   get_room_id   () const  { return room_id_;   }
      K_mf(rr_size)  get_room_size () const  { return room_size_; }

      K_mf(bool)  operator==  (const this_type& a) const  { return(room_id_ == a.room_id_); }
      K_mf(bool)  is_invalid  () const                    { return(room_id_ == rr_pos(-1)); }
      K_mf(void)  clear       ()                          { room_id_ = rr_pos(-1);          }
   } K_StructPacked;
   //---------------------------------------------------------------------------
   #ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
   #else
   #pragma pack()
   #endif
   //////////////////////////////////////////////////////////////////////////
   class  rr_room;
   class  rr_room_iterator;
   //////////////////////////////////////////////////////////////////////////
   struct rr_live_into   //�~��b�ж��̭����F��,�����Φ��覡�w�q
   {
      const void* obj_ptr_;
      rr_size     obj_size_;
   };
   //------------------------------------------------------------------------
   struct rr_live_peek
   {
      void*    obj_ptr_;
      rr_size  obj_size_;
   };
   //------------------------------------------------------------------------
   class  rr_room
   {
   public:
              K_ctor  rr_room  (this_inn& inn, rr_key key)  : inn_(inn), key_(key) {}
      virtual K_dtor ~rr_room  ()                           {}

      inline K_mf(bool)  into  (rr_key&, const rr_live_into* begin, const rr_live_into* end);//��i�h
      inline K_mf(bool)  peek  (const rr_live_peek* begin, const rr_live_peek* end)//���X��
               { return peek_objs(begin,end,NULL)==end;
               }

      /** ���X, �Y room_size ����, �h�Ǧ^�|�����X������ peek.
          �Ǧ^ NULL, ��ܦ�room�����D, �L�k���X������.
          if(lastsz) *lastsz=rtnŪ�X��bytes.
          �]�N�O�z�i�H:
            if(rtn != end) {
               memset((byte*)(rtn->obj_ptr_) + lastsz, 0, rtn->obj_size_ - lastsz);
               for(++rtn;  rtn != end;  ++rtn)
                  memset(rtn->obj_ptr_, 0, rtn->obj_size_);
            }
      */
      inline K_mf(const rr_live_peek*) peek_objs (const rr_live_peek* begin, const rr_live_peek* end, rr_size* lastsz)
               { return inn_.peek_objs(*this, begin, end, lastsz);
               }
      inline K_mf(bool) peek_objs_or_fill (const rr_live_peek* begin, const rr_live_peek* end, int minNeedCount) {
         rr_size             lastsz = 0;
         const rr_live_peek* ipeek  = peek_objs(begin, end, &lastsz);
         if(ipeek==0)
            return false;
         if(ipeek == end)
            return true;
         int idx = (int)(ipeek - begin);
         if(idx < minNeedCount)
            return false;
         memset((Kway::byte*)(ipeek->obj_ptr_) + lastsz, 0, ipeek->obj_size_ - lastsz);
         while(++ipeek != end)
            memset(ipeek->obj_ptr_, 0, ipeek->obj_size_);
         return true;
      }

      inline K_mf(rr_key)     get_key  () const     { return key_; }
      inline K_mf(this_inn&)  get_inn  () const     { return inn_; }

      inline K_mf(rr_pos)   get_room_id   () const  { return key_.get_room_id();   }
      inline K_mf(rr_size)  get_room_size () const  { return key_.get_room_size(); }

   private:
      friend class rr_inn;
      inline K_mf(void)  set_key    (rr_key akey)  { key_ = akey; }

      friend class rr_room_iterator;
      inline K_mf(bool)  peek_next  (); //���s�U�@�Ӧ��H���ж� :)

      this_inn&   inn_;
      rr_key      key_;
   };
   //------------------------------------------------------------------------
   class  rr_room_iterator
   {
      typedef rr_room_iterator      this_type;
      rr_room  room_;
   public:
      explicit K_ctor  rr_room_iterator  (const rr_room& a)  : room_(a)  { }

      inline K_mf(this_type&)  operator++  ()  { room_.peek_next();  return *this; }

      inline K_mf(bool)  operator==  (const this_type& a) const { return room_.get_key()==a.room_.get_key(); }
      inline K_mf(bool)  operator!=  (const this_type& a) const { return !(operator==(a)); }

      inline K_mf(rr_room*)  operator->  ()  { return &room_; }
      inline K_mf(rr_room&)  operator*   ()  { return room_;  }
   };
   //////////////////////////////////////////////////////////////////////////
//class  rr_inn
//{
//public:
           K_ctor  rr_inn  ()  {}
   virtual K_dtor ~rr_inn  ()  {}

   //���ѵ��ȭ����A�ȶ���:
   virtual K_mf(rr_key)   check_in   (rr_size needed_size) = 0;   //�s�ЫȨӳ���F
   virtual K_mf(bool)     check_out  (rr_key) = 0;                //�©ЫȰh��,�N�ж��k��
   virtual K_mf(rr_room)  get_room   (rr_key room_key)   { return rr_room(*this,room_key); }

   //Iterators.
   typedef rr_room_iterator   iterator;
   virtual K_mf(iterator)  begin  () = 0;
   virtual K_mf(iterator)  end    ()   { return iterator(rr_room(*this,rr_key(0,0))); }

   //�N�ȩХ����M��,�Фp�ߨϥ�!!
   virtual K_mf(void)  clear  () = 0;

   //�� get_map_room_key(�ж��Ǹ�) ���o�@���_��(�ж��Ǹ��бq0�}�l)
   //  (�Ҧp:�C�өж����O�T�w�j�p,�h�i�Φ���k,�ֳt�����o�ж��_��)
   //  �w�]:�Ǧ^�@��L�Ī��_��,��ܤ��䴩���\��
   virtual K_mf(rr_key)  get_map_room_key  (rr_pos /*room_seqno*/)  { return rr_key(); }
   //------------------------------------------------------------------------
   // exceptions:
   class rr_error : public std::runtime_error
   {
      typedef std::runtime_error    inherited;
   public:
      explicit rr_error  (const std::string& what_happed)
                  : inherited(what_happed)
                  { }
   };
   //-----------------------------------
   class storage_error : public rr_error
   {
      typedef rr_error     inherited;
   public:
      explicit storage_error  (const std::string& what_happed)
                  : inherited(what_happed)
                  { }
   };
   //-------------------------------------
   class room_size_error : public rr_error
   {
      typedef rr_error     inherited;
   public:
      explicit room_size_error  (const std::string& what_happed)
                  : inherited(what_happed)
                  { }
   };
   //-------------------------------------
   class check_out_not_allowed : public rr_error
   {
      typedef rr_error     inherited;
   public:
      explicit check_out_not_allowed  (const std::string& what_happed)
                  : inherited(what_happed)
                  { }
   };

protected:
   inline K_mf(void)  set_room_key  (rr_room& room, rr_key key)  { room.set_key(key); }

private:
   friend class rr_room;
   virtual K_mf(bool)  peek_next  (rr_room&) = 0;

   //�ȭ��n��i��,�Х� into_room()
   virtual K_mf(bool)  into_room  (rr_key&, rr_room&, const rr_live_into* begin, const rr_live_into* end) = 0;
   //�Q�n�ݬY�өж���۽ֶ�? �Х� peek_objs()
   virtual K_mf(const rr_live_peek*) peek_objs (const rr_room&, const rr_live_peek* begin, const rr_live_peek* end, rr_size* lastsz) = 0;
//};
//---------------------------------------------------------------------------
}; // rr_inn<>
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos, class RoomSize>
inline K_mf(bool) rr_inn<RoomPos,RoomSize>::rr_room::into (rr_key& key, const rr_live_into* begin, const rr_live_into* end)//��i�h
{
   return inn_.into_room(key, *this, begin, end);
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
inline K_mf(bool) rr_inn<RoomPos,RoomSize>::rr_room::peek_next ()
{  //���s�U�@�Ӧ��H���ж� :)
   return inn_.peek_next(*this);
}
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

