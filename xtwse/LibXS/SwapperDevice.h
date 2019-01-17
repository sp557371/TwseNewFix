//---------------------------------------------------------------------------
/* TSwapperDevice<>
    [���t/�޲z] �@�s�i�b MainMemory/File �����m���� objs
*/
// $Id: SwapperDevice.h,v 1.3 2005/06/26 11:15:14 fonwin Exp $
//---------------------------------------------------------------------------
#ifndef SwapperDeviceH
#define SwapperDeviceH
//---------------------------------------------------------------------------
#include "inn_file.h"
#include "KwayList.h"
#include "InnPeeker.h"
#include <assert.h>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
/** @brief �w�]�� swap file �x�s���A.
*/
typedef Kstl::rr_inn_file<Kway::dword, Kway::word>  TDefaultSwapperInn;
//---------------------------------------------------------------------------
/** @brief �@�ӳ�±N T �m���� Inn �� template.

   T ���i�]�t����, �z�i�H�ѦҦ� template class, �g�@�ӾA�X�z�� InnSwapper.
*/
template < class Peeker
         , class Inn = TDefaultSwapperInn >
class TSimpleInnSwapper
{ //�ĥ�²�檺�x�s�覡: �����NT�s�J/���X
  typedef TSimpleInnSwapper<Peeker,Inn>   this_type;
  Peeker    Peeker_;
protected:
  /** @brief �j���M�� swap file.

      �w swap out ����ƱN�û���!!
      Inn_.remove() = close(); & erase_file();
  */
  inline K_mf(void) InnRemove ()    { Inn_.remove(); }

public:
  //types.
  typedef Peeker                       peeker_type;
  typedef typename Peeker::value_type  value_type;
  typedef Inn                          inn_type;
  typedef typename Inn::rr_pos         rr_pos;
  typedef typename Inn::rr_key         rr_key;
  typedef typename Inn::rr_room        rr_room;
  typedef typename Inn::rr_live_peek   rr_live_peek;
  typedef typename Inn::rr_live_into   rr_live_into;
  static const int RoomSize  = peeker_type::RoomSize;
  static const int RoomItems = peeker_type::RoomItems;

  /** @brief �غc.

      �غc�ɻݭn�����z�� swap file.
  */
  K_ctor  TSimpleInnSwapper  (const std::string& swapperFileName)
    : Inn_(swapperFileName,
           RoomSize, //�T�w�ж��j�p,�_�h�j�M�i�Ωж��O�ܦY�O��!
           true)     //�ж������i�k��
    , SwapoutNum_(0)
    , SwapoutCount_(0)
    , SwapinCount_(0)
    { Inn_.clear(); }

  /** @brief �Ѻc.

      �N swap file �M��.
  */
  K_dtor  ~TSimpleInnSwapper ()
    { InnRemove();
    }

  /** @brief �N�Y��� T, �g�J swap file.

      data �g�J swap file ����, �ߧY�N data �R��.

      @return  �Ǧ^�ж����X, ����SwapIn()�ɪ��̾�.
  */
  K_mf(rr_pos)  SwapOut  (value_type* data)
    { if(!Inn_.is_opened())
         return rr_pos(-1);
      rr_key        key  = Inn_.check_in(0);
      rr_room       room = Inn_.get_room(key);
      rr_live_into  irecs[RoomItems];
      Peeker_.SetLiveInto(irecs, *data);
      room.into(key, irecs, irecs+numofele(irecs));
      delete data;
      ++SwapoutNum_;
      ++SwapoutCount_;
      return key.get_room_id();
    }

  /** @brief ���X swap file �̪��Y�����.

      @return �Ǧ^�� new ���ͪ����, �Y�Ǧ^ 0, ��ܵL�k���^�z�n�����.
  */
  K_mf(value_type*)  SwapIn  (rr_pos pos)
    { if(!Inn_.is_opened())
         return 0;
      rr_key         key(pos,RoomSize);
      rr_room        room(Inn_.get_room(key));
      rr_live_peek   irecs[RoomItems];
      Peeker_.SetLivePeek(irecs);
      if(!room.peek(irecs, irecs+numofele(irecs)))
         return 0;
      Inn_.check_out(key);
      --SwapoutNum_;
      ++SwapinCount_;
      return Peeker_.NewPeekValue(irecs);
    }

  /** @brief �N�Y����Ʊq swap file �����R��.
  */
  K_mf(void)  CheckOut  (rr_pos pos)
    { if(Inn_.is_opened())
         Inn_.check_out(rr_key(pos,RoomSize));
      --SwapoutNum_;
    }

  inline K_mf(rr_pos)  GetSwapoutNum    () const   { return SwapoutNum_;   }
  inline K_mf(rr_pos)  GetSwapoutCount  () const   { return SwapoutCount_; }
  inline K_mf(rr_pos)  GetSwapinCount   () const   { return SwapinCount_;  }
private:
  Inn     Inn_;
  rr_pos  SwapoutNum_;
  rr_pos  SwapoutCount_;
  rr_pos  SwapinCount_;
};
//---------------------------------------------------------------------------
/** @brief �N�@��� Container �m���� Inn.

*/
template < class Peeker
         , class Inn = TDefaultSwapperInn >
class TRecsInnSwapper
{
  typedef TRecsInnSwapper<Peeker,Inn>  this_type;
  Peeker    Peeker_;
protected:
  /** @brief �j���M�� swap file.

      �w swap out ����ƱN�û���!!
      Inn_.remove() = close(); & erase_file();
  */
  inline K_mf(void) InnRemove ()    { Inn_.remove(); }

public:
  //types.
  typedef Peeker                       peeker_type;
  typedef typename Peeker::recs_type   value_type;
  typedef typename Peeker::recs_type   recs_type;
  typedef typename Peeker::value_type  rec_type;
  typedef Inn                          inn_type;
  typedef typename Inn::rr_pos         rr_pos;
  typedef typename Inn::rr_key         rr_key;
  typedef typename Inn::rr_room        rr_room;
  typedef typename Inn::rr_live_peek   rr_live_peek;
  typedef typename Inn::rr_live_into   rr_live_into;
  static const int RoomSize  = peeker_type::RoomSize  + sizeof(rr_pos);
  static const int RoomItems = peeker_type::RoomItems + 1;

  /** @brief �غc.

      �غc�ɻݭn�����z�� swap file.
  */
  K_ctor  TRecsInnSwapper  (const std::string& swapperFileName)
    : Inn_(swapperFileName,
           RoomSize,//�T�w�ж��j�p,�_�h�j�M�i�Ωж��O�ܦY�O��!
           true)    //�ж������i�k��
    , SwapoutNum_(0)
    , SwapoutCount_(0)
    , SwapinCount_(0)
    , SwapoutRecNum_(0)
    , SwapoutRecCount_(0)
    , SwapinRecCount_(0)
    { Inn_.clear(); }

  /** @brief �Ѻc.

      �N swap file �M��.
  */
  K_dtor  ~TRecsInnSwapper ()
    { InnRemove();
    }

  /** @brief �N�Y Container �����g�J swap file.

      recs �g�J swap file ����, �Y delRecs==true, �h�ߧY�N recs �R��.

      @return  �Ǧ^�ж����X, ����SwapIn()�ɪ��̾�.
  */
  K_mf(rr_pos)  SwapOut  (recs_type* recs, bool delRecs = true)
    { if(!Inn_.is_opened())
         return rr_pos(-1);
      rr_pos         rkey = 0;
      rr_live_into   irecs[RoomItems] = { { &rkey, sizeof(rkey) } };
      typename recs_type::iterator  iend = recs->end();
      for(typename recs_type::iterator i = recs->begin();  i != iend;  ++i) {
         Peeker_.SetLiveInto(irecs+1, *i);
         rr_key   key  = Inn_.check_in(0);
         rr_room  room = Inn_.get_room(key);
         room.into(key, irecs, irecs+numofele(irecs));
         rkey = key.get_room_id();
         ++SwapoutRecNum_;
         ++SwapoutRecCount_;
      }
      ++SwapoutNum_;
      ++SwapoutCount_;
      if(delRecs)
         delete recs;
      return rkey;
    }

  /** @brief ���X swap file �̪��Y�����.

      @return �Ǧ^�� new ���ͪ� T, �Y�Ǧ^ 0, ��ܵL�k���^�z�n�����.
  */
  K_mf(recs_type*)  SwapIn  (rr_pos pos)
    { if(!Inn_.is_opened())
         return 0;
      --SwapoutNum_;
      ++SwapinCount_;
      if(pos <= 0)
         return 0;
      std::auto_ptr<recs_type>   recs(new recs_type);
      rr_live_peek               irecs[RoomItems] = { { &pos, sizeof(pos) } };
      do {
         Peeker_.SetLivePeek(irecs+1);
         rr_key   key(pos,RoomSize);
         rr_room  room(Inn_.get_room(key));
         if(room.peek(irecs, irecs+numofele(irecs))) {
            Inn_.check_out(key);
            Peeker_.Insert(irecs+1,recs.get());
            --SwapoutRecNum_;
            ++SwapinRecCount_;
         } else
            break;
      } while(pos > 0);
      return recs.release();
    }

  /** @brief �N�Y����Ʊq swap file �����R��.
  */
  K_mf(void)  CheckOut  (rr_pos pos)
    { if(!Inn_.is_opened())
         return;
      if(pos <= 0)
         return;
      rr_live_peek   irecs[RoomItems] = { { &pos, sizeof(pos) } };
      do {
         rr_key   key(pos,RoomSize);
         rr_room  room(Inn_.get_room(key));
         if(room.peek(irecs, irecs+numofele(irecs))) {
            Inn_.check_out(key);
            --SwapoutRecNum_;
         } else
            break;
      } while(pos > 0);
      --SwapoutNum_;
    }

  inline K_mf(rr_pos)  GetSwapoutNum       () const   { return SwapoutNum_;      }
  inline K_mf(rr_pos)  GetSwapoutCount     () const   { return SwapoutCount_;    }
  inline K_mf(rr_pos)  GetSwapinCount      () const   { return SwapinCount_;     }
  inline K_mf(rr_pos)  GetSwapoutRecNum    () const   { return SwapoutRecNum_;   }
  inline K_mf(rr_pos)  GetSwapoutRecCount  () const   { return SwapoutRecCount_; }
  inline K_mf(rr_pos)  GetSwapinRecCount   () const   { return SwapinRecCount_;  }
private:
  Inn     Inn_;
  rr_pos  SwapoutNum_;
  rr_pos  SwapoutCount_;
  rr_pos  SwapinCount_;
  rr_pos  SwapoutRecNum_;
  rr_pos  SwapoutRecCount_;
  rr_pos  SwapinRecCount_;
};
//---------------------------------------------------------------------------
/** @brief �@�ӥi�N T �b [�D�O����] �� [swap file] �����m���� class.
*/
template <class Swapper>
class TSwapperDevice : public Swapper
{
  typedef TSwapperDevice<Swapper>            this_type;
  typedef this_type                          this_device;
  typedef Swapper                            swapper_type;
  typedef unsigned long                      hit_count_type;
  typedef typename Swapper::inn_type         inn_type;
  typedef typename Swapper::peeker_type      peeker_type;
  typedef typename peeker_type::value_type   value_type;
  typedef typename inn_type::rr_pos          rr_pos;

  class HitCountHolder;
  class ObjHolder : public TNodeBase
  {
    typedef ObjHolder   this_type;
    enum Flag {
      f_swapped   = 0x01,
      f_swappable = 0x02,
      f_init      = f_swappable,
    };
    union {
      value_type* Ptr_;
      rr_pos      Key_;
    };
    Flag            Flag_;
    HitCountHolder* HitCount_;
    unsigned        RefCount_;
  public:
    inline K_ctor  ObjHolder  (value_type* a = 0)
               : Flag_(f_init), HitCount_(0), RefCount_(1)
               { Ptr_ = a; }

    inline K_mf(void)  AddRef  ()   { ++RefCount_; }
           K_mf(bool)  Release (this_device* swapper)
                          { if(swapper==0
                            || RefCount_==0
                            || --RefCount_ != 0)
                               return false;
                            if(Flag_ & f_swapped)
                              swapper->CheckOut(Key_);
                            else if(Ptr_) {
                              delete Ptr_;
                              swapper->DelOneInMemory();
                            }
                            --(swapper->CurNumObjs_);
                            delete this;
                            return true; }
    K_mf(value_type*) GetPtr (this_device* swapper)
                          { if(Flag_ & f_swapped) {
                              swapper->AddOneInMemory();
                              Flag_ = Flag(Flag_ & ~f_swapped);
                              Ptr_  = swapper->SwapIn(Key_);
                              // TODO: if(Ptr_==NULL)...
                              HitCount_ = swapper->AddHitLevel(this,0);
                            } else if(Ptr_ == 0) {
                              swapper->AddOneInMemory();
                              Ptr_      = new value_type;
                              HitCount_ = swapper->AddHitLevel(this,0);
                            } else
                              HitCount_ = swapper->AddHitLevel(this,HitCount_);
                            return Ptr_; }
    K_mf(value_type*)  Swappable () const
                          { return ( Ptr_
                                &&  (Flag_ & (f_swapped|f_swappable))
                                      ==f_swappable)
                                ? Ptr_ : 0; }
          K_mf(bool)  SwapOut  (this_device* swapper)
                          { if(!Swappable())
                               return false;
                            rr_pos key = swapper->SwapOut(Ptr_);
                            if(key==0  ||  key==rr_pos(-1))///< �L�Ī�key.
                              return false;
                            Key_  = key;
                            Flag_ = Flag(Flag_ | f_swapped);
                            Delink();//�q HitCountList ������
                            HitCount_ = 0;
                            return true; }
    inline K_mf(bool) GetSwappableFlag () const
                          { return (Flag_ & f_swappable) == f_swappable; }
    inline K_mf(bool) SetSwappable (bool can)
                          { if(can) Kstl::set_enum_flag(Flag_, f_swappable);
                            else  Kstl::clear_enum_flag(Flag_, f_swappable);
                            return true; }
  }; // ObjHodler
public:
  // types.
  typedef size_t        size_type;
  typedef ObjHolder*    ObjHandle;

  /** @brief �z���ӳz�L�� class ���o value_type �����e.
  */
  class PointerType
  {
    typedef PointerType   this_type;
    this_device*  Swapper_;
    ObjHandle     Handle_;
    inline  K_mf(void)  Release  ()   { if(Handle_) Handle_->Release(Swapper_); }
    inline  K_mf(void)  AddRef   ()   { if(Handle_) Handle_->AddRef();          }
  public:
    K_ctor  PointerType()                             : Swapper_(0), Handle_(0)  { }
    K_ctor  PointerType(this_device* s, ObjHandle h)  : Swapper_(s), Handle_(h)  { }
    K_ctor  PointerType(const this_type& a)           : Swapper_(a.Swapper_)
                                                      , Handle_(a.Handle_)
                                                      { AddRef(); }
    K_dtor ~PointerType()    { Release(); }

    inline K_mf(value_type*)  operator-> () const   { return Handle_->GetPtr(Swapper_); }
    inline K_mf(value_type&)  operator*  () const   { return *(operator->());           }
    inline K_mf(void)         Swap (this_type& a)   { std::swap(Swapper_, a.Swapper_);
                                                      std::swap(Handle_,  a.Handle_);   }
    K_mf(PointerType&) operator= (const this_type& a)    { this_type tmp(a);
                                                           tmp.Swap(*this);
                                                           return *this; }
    friend inline K_mf(bool)         SwapOut  (this_type& a)  { return a.Handle_->SwapOut(a.Swapper_); }
    friend inline K_mf(value_type&)  SwapIn   (this_type& a)  { return *a; }
    friend        K_mf(bool)         SetSwappable (this_type& a, bool can)
                                                              { if(can==a.Handle_->GetSwappableFlag())
                                                                  return true;
                                                                if(!a.Handle_->SetSwappable(can))
                                                                  return false;
                                                                if(can)
                                                                  a.Swapper_->AdjMaxNumInMemory(-1);
                                                                else {//cannot swappable
                                                                  a.Swapper_->AdjMaxNumInMemory(+1);
                                                                  *a;//�]�i�H�g��:SwapIn(a);
                                                                }
                                                                return true; }
  };

  // ctor / dtor
  K_ctor  TSwapperDevice  (size_type maxNumInMemory, const std::string& swapperFileName)
    : swapper_type(swapperFileName)
    , MaxNumInMemory_(maxNumInMemory)
    , CurNumInMemory_(0)
    , CurNumObjs_(0)
    { }

  // member functions
  K_mf(PointerType)  Create  ()   { ObjHandle   i(new ObjHolder);
                                    ++CurNumObjs_;
                                    return PointerType(this,i); }

  K_mf(size_type)  GetCurNumInMemory  () const  { return CurNumInMemory_; }
  K_mf(size_type)  GetMaxNumInMemory  () const  { return MaxNumInMemory_; }
  K_mf(size_type)  GetCurNumObjs      () const  { return CurNumObjs_;     }
  K_mf(void)       AdjMaxNumInMemory  (int adjN, bool swapNow = false)
                                          { SetMaxNumInMemory(MaxNumInMemory_+=adjN, swapNow); }
  K_mf(void)       SetMaxNumInMemory  (size_type n, bool swapNow = false)
                                          { if(MaxNumInMemory_ ==n) return;
                                            if(MaxNumInMemory_ < n) {
                                               MaxNumInMemory_ = n;
                                               return;
                                            }
                                            MaxNumInMemory_ = (n < 1 ? 1 : n);
                                            if(swapNow)
                                              while(SwapSomeOut()!=0);
                                          }
private:
  // member functions for ObjHolder
  friend class ObjHolder;
  K_mf(void)  DelOneInMemory  ()  { assert(CurNumInMemory_);
                                    --CurNumInMemory_; }
  K_mf(void)  AddOneInMemory  ()  { if(++CurNumInMemory_ > MaxNumInMemory_)
                                       SwapSomeOut(); }
  K_mf(size_type) SwapSomeOut ()  { 
                              if(HitCountList_.empty())  return 0;
                              size_type L = CurNumInMemory_ - MaxNumInMemory_;
                              if(L <= 0)  return 0;
                              if(L > 10)  L = 10;
                              HitCountHolder* list = HitCountList_.front();
                              size_type count = 0;
                              for(;;) {
                                while(list->empty()) {
                                __NEXT_LIST:
                                  if(list == HitCountList_.back())
                                    return count;
                                  list = list->GetNext();
                                } // while(list is empty...
                                ObjHolder*  objh = list->front();
                                do {
                                  ObjHolder*  next = static_cast<ObjHolder*>(objh->GetNext());
                                  if(objh->SwapOut(this)) {
                                    --CurNumInMemory_;
                                    ++count;
                                    if(list->empty()) {
                                      HitCountHolder* cur = list;
                                      list = list->GetNext();
                                      delete cur;
                                      if(--L <= 0)
                                         return count;
                                      goto __NEXT_LIST;
                                    } else
                                    if(--L <= 0)
                                      return count;
                                  } //if(SwapOut success
                                  objh = next;
                                } while(objh != list->end());
                              } //for(;;
                            } // SwapSomeOut()
private:
  size_type   MaxNumInMemory_;
  size_type   CurNumInMemory_;
  size_type   CurNumObjs_;

  //--- HitCountList ---
  struct HitCountHolder : public TList<ObjHolder>
                        , public TNodeBase
  {
    typedef HitCountHolder  this_type;
    hit_count_type  HitCount_;
    K_ctor  HitCountHolder(hit_count_type n) : HitCount_(n),TNodeBase() {}
    inline K_mf(this_type*) GetPrev () const  { return static_cast<this_type*>(TNodeBase::GetPrev()); }
    inline K_mf(this_type*) GetNext () const  { return static_cast<this_type*>(TNodeBase::GetNext()); }
  };
  TList<HitCountHolder>   HitCountList_;
  // member functions for HitCountList
  K_mf(HitCountHolder*)  AddHitLevel  (ObjHolder* obj, HitCountHolder* counter)
    { if(counter==0) {
        if(HitCountList_.empty())
          HitCountList_.push_back(counter = new HitCountHolder(1));
        else//�[�b�̫�(HitLevel�̰�,�̤��e���Q���X)
          counter = HitCountList_.back();
        counter->push_back(obj);
        return counter;
      }
      HitCountHolder* next;
      if(counter == HitCountList_.back()) // counter is last
        goto __CHECK_GAP;
      next = counter->GetNext();
      if(counter->HitCount_+1 != next->HitCount_) {
      __CHECK_GAP:
        //counter �P next ������ gap
        if(counter->empty_or_1()) {  //�u���@�Ӥ���
          ++(counter->HitCount_);
          return counter;
        }
        counter->insert_back(*(next = new HitCountHolder(counter->HitCount_+1)));
        next->push_back(obj);
      } else {
        next->push_back(obj);
        if(counter->empty())
          delete counter;
      }
      return next;
    } // AddHitLevel()
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

