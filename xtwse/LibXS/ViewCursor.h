//
// class TViewCursor;
// 提供 TViewArea 顯示時,可以管理現在顯示哪筆資料
//
// 版本: $Id: ViewCursor.h,v 1.4 2005/08/09 01:15:01 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ViewCursorH
#define ViewCursorH
/////////////////////////////////////////////////////////////////////////////
#include <string>
#include "ev_container.h"
//---------------------------------------------------------------------------
namespace Kway {
using namespace Kstl;
LIBNS_class TViewArea;
/////////////////////////////////////////////////////////////////////////////
enum TViewCursorMoveFrom {
      vcmfBegin,
      vcmfEnd,
      vcmfCurrent,
};
//---------------------------------------------------------------------------
LIBNS_class TViewCursor
{
public:
//Types.
   typedef size_t       field_n_type;
   typedef ptrdiff_t    distance_type;
   typedef size_t       size_type;

//static const.
   static const size_type    InvalidDist_ = static_cast<size_type>(-1);

//Constructor/Destructor
           K_ctor  TViewCursor  ()                      : DistBeginCatch_(InvalidDist_)      { }
           K_ctor  TViewCursor  (const TViewCursor& a)  : DistBeginCatch_(a.DistBeginCatch_) { }
   virtual K_dtor ~TViewCursor  ()  { }

   virtual K_mf(TViewCursor*)  Clone  () const = 0;
   virtual K_mf(TViewCursor*)  Clone  (size_type distBegin, const ev_iterator&) const = 0;

//assign operator
   virtual K_mf(TViewCursor&)  operator=  (const TViewCursor& a) { DistBeginCatch_ = a.DistBeginCatch_;
                                                                   return *this; }
//compare operator
   virtual K_mf(bool)  operator==  (const TViewCursor&)   const = 0;
           K_mf(bool)  operator!=  (const TViewCursor& a) const  { return !(*this==a); }

//取得欄位顯示字串
   virtual K_mf(std::string)  GetStr  (field_n_type) const = 0;
   virtual K_mf(void)         PutStr  (field_n_type, const std::string&, TViewArea* logva) = 0;
   virtual K_mf(void)         BeginUpdate () {}
   virtual K_mf(void)         EndUpdate   () {}

//移動位置,傳回:從指定位置(begin,end,current)走了幾步(可能<0,視steps而定)
//          vcmfBegin    steps必須 >= 0, 結果最多移動到:最後一筆[有效]資料
//          vcmfEnd      steps必須 <= -1 結果最多移動到:第一筆[有效]資料
//          vcmfCurrent  steps >0 <0 或 =0 皆可, 結果同 vcmfBegin 及 vcmfEnd
   virtual K_mf(int)   Move  (int steps, TViewCursorMoveFrom) = 0;
   virtual K_mf(bool)  Find  (const char* key, bool isLowerBound) = 0;
//判斷位置, DistanceBegin()
//          Distance(a) 則傳回 this 到 a 的距離, >0:this在a之後, <0:this在a之前, =0:this與a在相同位置
   inline K_mf(size_type)      DistanceBegin  () const                      { return DistBeginCatch_; }
   inline K_mf(distance_type)  Distance       (const TViewCursor* a) const  { return DistanceBegin() - a->DistanceBegin(); }
   inline K_mf(bool)           IsInvalid      () const                      { return DistBeginCatch_ == InvalidDist_; }

protected:
   size_type   DistBeginCatch_;

   friend LIBNS_class TViewArea;
   // 在 AfErase 事件時,請將 isAfErase 設為 true
   // 此時,若 cursor所指的資料被 erase,則 isAfErase 仍為 true
   // 否則,函式結束時 isAfErase 為 false
   virtual K_mf(void)  AdjDistBeginCatch  (bool& isAfErase, const ev_iterator& ifrom,
                                           size_type frombegin, size_type count);
   virtual K_mf(void)  SetEvIterator (size_type frombegin, const ev_iterator& i) = 0;
};
/////////////////////////////////////////////////////////////////////////////
template <class Container, class FieldMaker, class KeyMaker, class VALogger>
class TTViewCursor : public TViewCursor
{
   typedef TViewCursor                                           inherited;
   typedef TTViewCursor<Container,FieldMaker,KeyMaker,VALogger>  this_type;
public:
//Types.
   typedef Container                      container_type;
   typedef typename Container::iterator   iterator;
   typedef FieldMaker                     field_maker_type;
   typedef KeyMaker                       key_maker_type;

//Constructor
   inline  K_ctor  TTViewCursor  (container_type* c, const field_maker_type& fmaker = field_maker_type(),
                                  VALogger* log = 0)
                     : inherited(),
                       Container_(c), Iterator_(c ? c->begin() : iterator()), FieldMaker_(fmaker),
                       Log_(log),
                       UpdCount_(0)
                     { if(c  &&  !c->empty())  DistBeginCatch_ = 0; }
   inline  K_ctor  TTViewCursor  (const this_type& a)
                     : inherited(a),
                       Container_(a.Container_), Iterator_(a.Iterator_), FieldMaker_(a.FieldMaker_),
                       Log_(a.Log_),
                       UpdCount_(0)
                     { }

   K_mf(this_type*)  Clone  () const  { return new this_type(*this); }
   K_mf(this_type*)  Clone  (size_type distBegin, const ev_iterator& i) const
                                      { this_type* r = new this_type(*this);
                                        r->SetEvIterator(distBegin, i);
                                        return r; }

   K_mf(key_maker_type&)        GetKeyMaker  ()        { return KeyMaker_; }
   K_mf(const key_maker_type&)  GetKeyMaker  () const  { return KeyMaker_; }

//取得顯示字串:由 FieldMaker 代勞
   K_mf(std::string)  GetStr  (field_n_type n) const  { return FieldMaker_.Get(Iterator_,n); }
   K_mf(void)         PutStr  (field_n_type n, const std::string& s, TViewArea* logva)
                                                      { if(Log_)
                                                           Log_->WriteMod(*Container_,Iterator_,n,s,logva);
                                                        return FieldMaker_.Put(*Container_,Iterator_,n,s); }
   K_mf(void) BeginUpdate () {
      if(UpdCount_++ == 0)
         if(!IsInvalid())
            Updater_ = Container_->begin_update(Iterator_);
   }
   K_mf(void) EndUpdate () {
      if(UpdCount_ > 0)
         if(--UpdCount_ == 0) {
            typename container_type::updater u;
            Updater_ = u;
         }
   }
//assign operator
   K_mf(this_type&)  operator=  (const TViewCursor& cur)
            {
               const this_type*  a = dynamic_cast<const this_type*>(&cur);
               if(a) {
                  Container_ = a->Container_;
                  Iterator_  = a->Iterator_;
               }
               inherited::operator=(cur);
               return *this;
            } // operator=()
//compare operator
   K_mf(bool)  operator==  (const TViewCursor& cur) const {
               const this_type*  a = dynamic_cast<const this_type*>(&cur);
               return(a ? (DistBeginCatch_==a->DistBeginCatch_  &&  Iterator_==a->Iterator_) : false);
            } // operator==()

//移動位置,傳回:從指定位置(begin,end,current)走了幾步(可能<0,視steps而定)
//         steps請參考 TViewCursor::Move()的說明
   K_mf(int)  Move  (int steps, TViewCursorMoveFrom from)
            {
               if(Container_->empty()) {
                  DistBeginCatch_ = InvalidDist_;
                  return 0;
               }
               switch(from) {
               case vcmfBegin:   Iterator_ = Container_->begin();
                                 DistBeginCatch_ = 0;
                                 break;
               case vcmfEnd:     Iterator_ = Container_->end();
                                 DistBeginCatch_ = Container_->size();
                                 break;
               case vcmfCurrent: break;
               default:          return 0;
               }
               if(steps) {
                  steps = MoveIterator(steps, static_cast<typename Container::iterator::iterator_category*>(0));
                  DistBeginCatch_ += steps;
               }
               return steps;
            } // Move()
   K_mf(bool)  Find  (const char* key, bool isLowerBound)
            {
               if(Container_->empty()) {
                  DistBeginCatch_ = InvalidDist_;
                  return false;
               }
               typename container_type::key_type   vkey;
               KeyMaker_(vkey,key);
               iterator newitr = (isLowerBound ? Container_->lower_bound(vkey)
                                               : Container_->find(vkey) );
               if(newitr == Container_->end()) {
                  if(isLowerBound) {
                     Iterator_ = --newitr;
                     DistBeginCatch_ = Container_->size()-1;
                  } else
                     return false;
               } else {
                  Iterator_ = newitr;
                  DistBeginCatch_ = Container_->distance_begin(newitr);
               }
               return true;
            } // Find()

   inline K_mf(iterator)         GetIterator  () const  { return Iterator_;  }
   inline K_mf(container_type*)  GetContainer () const  { return Container_; }
protected:
   container_type*   Container_;
   iterator          Iterator_;
   field_maker_type  FieldMaker_;
   key_maker_type    KeyMaker_;
   VALogger*         Log_;

   typename container_type::updater Updater_;
   unsigned                         UpdCount_;

   K_mf(int)  MoveIterator  (int steps, std::random_access_iterator_tag*)
   {
      int maxsteps;
      if(steps > 0) {
         //iterator end = Container_->end();
         //maxsteps = end - Iterator_;
         maxsteps = (int)(Container_->size() - DistBeginCatch_);
         if(steps >= maxsteps) {
            Iterator_ = Container_->end();
            --Iterator_;
            return maxsteps;
         }
      } else {//steps < 0
         //iterator beg = Container_->begin();
         //maxsteps = beg - Iterator_;
         maxsteps = (int)(0 - DistBeginCatch_);
         if(steps <= maxsteps) {
            Iterator_ = Container_->begin();
            return maxsteps;
         }
      }
      Iterator_ += steps;
      return steps;
   }

   K_mf(int)  MoveIterator  (int steps, std::bidirectional_iterator_tag*)
   {
      int      moveCount = 0;
      int      walksteps = (steps > 0 ? 1 : -1);
      iterator itrStop   = (steps > 0 ? Container_->end()
                                      : Container_->begin());
      if(steps > 0)
         --itrStop;
      for(;  steps != 0;  steps -= walksteps) {
         if(Iterator_ == itrStop)
            break;
         if(walksteps > 0)
            ++Iterator_;
         else
            --Iterator_;
         moveCount += walksteps;
      }
      return moveCount;
   }
private:
   //調整 Iterator
   K_mf(void)  AdjDistBeginCatch  (bool& isAfErase, const ev_iterator& ifrom,
                                   size_type frombegin, size_type count)
   {
      inherited::AdjDistBeginCatch(isAfErase, ifrom, frombegin, count);
      if(isAfErase) {
         Iterator_ = static_cast<const iterator&>(ifrom);
         if(Container_->empty())
            DistBeginCatch_ = InvalidDist_;
         else
         if(Iterator_==Container_->end()) {
            --Iterator_;
            --DistBeginCatch_;
            isAfErase = false;
         }
      } //if(isAfErase...
   } // AdjDistBeginCatch()
   virtual K_mf(void)  SetEvIterator (size_type frombegin, const ev_iterator& i) {
      this->DistBeginCatch_ = frombegin;
      this->Iterator_       = static_cast<const iterator&>(i);
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

