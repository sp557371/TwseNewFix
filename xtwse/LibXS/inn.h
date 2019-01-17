//
// template rr_inn<>
//
// 版本: $Id: inn.h,v 1.3 2006/04/03 14:34:39 fonwin Exp $
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
// Kstl 命名規則說明:請參考 "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
/*
●住進一間客棧的步驟:
  1. 找到一間合適的客棧:
       (1) 客棧提供的 room_size 是否夠大
       (2) 是否允許 check_out (別懷疑,就是有客棧不允許你 check out)
       (3) 若客棧提供可變大小的房間,可是又不允許你 check out,
           ※請注意: 若您已經住進去了,又想要一個更大的房間,
                     可能會有 rr_inn::check_out_not_allowed 的 exception
  2. 到客棧 check_in(needed_size) 並提供您需要多大的房間:
       (1) 如果此客棧無法提供您需要的房間,則會產生底下的 exception:
             rr_inn::storage_error("rr_inn::check_in() <room_size> error!");
       (2) 如果客棧允許您住進來,則會給您一把鑰匙,請妥善保存
       (3) 關於鑰匙:若您住的客棧提供一個特殊的方法取得鑰匙:get_map_room_key(room_seqno);
                    則可以不用 check_in();
  3. 用您的鑰匙,要求客棧,帶您到您的房間: get_room()
       在您還沒要求住進去之前,這個房間有可能還只是個「空中樓閣」(並不存在於這個世界上)
       不過您不需要擔心,一旦您要求住進去: get_room().into() 這個房間就會有如魔法般的「蹦!」出來了
  4. 終於,您可以住進您的房間了:
       (1) 客棧.get_room().into(rr_key& key, const rr_live_into* begin, const rr_live_into* end)
       (2) key:要進入房間,當然需要剛剛給您的鑰匙囉!
               ※請注意:離開房間後(into()結束後),key的內容有可能被改變喔!
       (3) begin,end:在這裡定義,您要住進房間的東西
       (4) 如果您想要改變[住進去的東西],請再一次的使用 4.(1) 的方法
  5. rr_live_into舉個例子吧:
       std::string    MyName;
       #pragma pack(push,1)
       struct {...}   MyMemory;
       #pragma pack(pop)
       ...
       rr_live_into   MySoul[] = { { MyName.c_str(),  MyName.length()+1 },  //變動長度,以'\0'做為結束
                                   { &MyMemory,       sizeof(MyMemory)  } };
       這樣定義雖然簡單,但有一個缺點,就是要把東西取出時較不方便:因為第一筆資料為變動長度
       底下提供了另一種較方便的方式:
       int   len = MyName.length();
       if(len > 255)  len = 255;//限制MyName最長255bytes
       rr_live_into   MySoul[] = { { &MySoul[1].obj_size_,   1                },  //1bytes紀錄MyName的長度
                                   { MyName.c_str(),         len              },  //變動長度資料
                                   { &MyMemory,              sizeof(MyMemory) } };
       如果您能這樣做,將可大幅簡化取出時的作業
       另外,要請您額外注意一點:若您要把一個結構(如上例的MyMemory),直接記入到房間內,
         (1) 請加上:
             #pragma pack(push,1)   如果不加上此行,這個世界還是可以運行的很好,只是您可能需要更大的房間
                                    而且多出來的空間是浪費掉的
             struct {...}  MyMemory;
             #pragma pack(pop)
         (2) MyMemory之內不能有[指標]類的物件(如: char*、int*、std::string、std::vector...)
             最好只含有 plain old data(如: int、char、float...)

●好了,現在您住進一家客棧裡了,但是當世界重生時(AP重新執行),要如何找回[您住進客棧裡的東西]呢?
  1. 首先找到您住的那家客棧
  2. 利用客棧提供的 begin()、end() 「iterator」來窺視每一間房間
      (在這種特別的時刻,請暫時別提[隱私權]的問題吧!否則怎麼把您找回來呢?)
  3. 定義取回的結構: rr_live_peek
     (1) 接續以上的例子吧! 若您使用第一種方式:
       rr_inn:iterator  iend = inn.end();   //您覺得為何要先取得 iend 呢? 想一下吧!
       for(rr_inn:iterator i = inn.begin();  i != iend;  ++i) {
          char           cName;
          rr_live_peek   MySoul[] = { { 0,       0             }, //這次取出資料,要skip多少空間
                                      { &cName,  sizeof(cName) } };
          MyName = "";
          //先取出MyName
          while(i->peek(MySoul,MySoul+2)) {  //一次抓sizeof(cName)個bytes,要抓好多次!
             MyName += cName;
             MySoul[0].obj_size_ += MySoul[1].obj_size_;
             if(cName==0)//終於把MyName抓完了
                break;
          }
          //接下來取出MyMemory
          MySoul[1].obj_ptr_  = &MyMemory;
          MySoul[1].obj_size_ = sizeof(MyMemory);
          if(i->peek(MySoul,MySoul+2)) {
             //剩下就是MyName、MyMemory的應用囉
             ...
          }
       } //for(i...
      (2) 如果用第二種方式,就簡單(也快)多了:
       char           cName[256];
       rr_live_peek   MySoul[] = { { &MySoul[1].obj_size_,   1  },
                                   { cName,                  0  },
                                   { &MyMemory,              sizeof(MyMemory) } };
       rr_inn:iterator  iend = inn.end(); //您覺得,在迴圈裡面一直呼叫inn.end()比較快,還是先將iend準備好比較快呢?
       for(rr_inn:iterator i = inn.begin();  i != iend;  ++i) {
          if(i->peek(MySoul,MySoul+3)) {
             cName[MySoul[1].obj_size_] = 0;//加上字串結束符號
             MyName = cName;
             //剩下就是MyName、MyMemory的應用囉
             ...
          }
       }

●另外,如果您不想住了(或您想不開,想永遠離開這個美麗的世界...)
  1. 直接告訴客棧您要 check_out() 即可
  2. 若您住的客棧不允許 check_out，那您會得到一個 rr_inn::check_out_not_allowed 的 exception

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
   {  //擁有一把房門的鑰匙 rr_key, 才可進入一個 room
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
   struct rr_live_into   //居住在房間裡面的東西,必須用此方式定義
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

      inline K_mf(bool)  into  (rr_key&, const rr_live_into* begin, const rr_live_into* end);//住進去
      inline K_mf(bool)  peek  (const rr_live_peek* begin, const rr_live_peek* end)//取出來
               { return peek_objs(begin,end,NULL)==end;
               }

      /** 取出, 若 room_size 不足, 則傳回尚未取出的那個 peek.
          傳回 NULL, 表示此room有問題, 無法取出任何資料.
          if(lastsz) *lastsz=rtn讀出的bytes.
          也就是您可以:
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
      inline K_mf(bool)  peek_next  (); //偷窺下一個有人住的房間 :)

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

   //提供給客倌的服務項目:
   virtual K_mf(rr_key)   check_in   (rr_size needed_size) = 0;   //新房客來報到了
   virtual K_mf(bool)     check_out  (rr_key) = 0;                //舊房客退租,將房間歸還
   virtual K_mf(rr_room)  get_room   (rr_key room_key)   { return rr_room(*this,room_key); }

   //Iterators.
   typedef rr_room_iterator   iterator;
   virtual K_mf(iterator)  begin  () = 0;
   virtual K_mf(iterator)  end    ()   { return iterator(rr_room(*this,rr_key(0,0))); }

   //將客房全部清空,請小心使用!!
   virtual K_mf(void)  clear  () = 0;

   //用 get_map_room_key(房間序號) 取得一把鑰匙(房間序號請從0開始)
   //  (例如:每個房間都是固定大小,則可用此方法,快速的取得房間鑰匙)
   //  預設:傳回一把無效的鑰匙,表示不支援此功能
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

   //客倌要住進來,請用 into_room()
   virtual K_mf(bool)  into_room  (rr_key&, rr_room&, const rr_live_into* begin, const rr_live_into* end) = 0;
   //想要看某個房間住著誰嗎? 請用 peek_objs()
   virtual K_mf(const rr_live_peek*) peek_objs (const rr_room&, const rr_live_peek* begin, const rr_live_peek* end, rr_size* lastsz) = 0;
//};
//---------------------------------------------------------------------------
}; // rr_inn<>
/////////////////////////////////////////////////////////////////////////////
template <class RoomPos, class RoomSize>
inline K_mf(bool) rr_inn<RoomPos,RoomSize>::rr_room::into (rr_key& key, const rr_live_into* begin, const rr_live_into* end)//住進去
{
   return inn_.into_room(key, *this, begin, end);
}
//---------------------------------------------------------------------------
template <class RoomPos, class RoomSize>
inline K_mf(bool) rr_inn<RoomPos,RoomSize>::rr_room::peek_next ()
{  //偷窺下一個有人住的房間 :)
   return inn_.peek_next(*this);
}
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

