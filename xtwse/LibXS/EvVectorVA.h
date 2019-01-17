//
// 因為 ev_vector<> 的特殊性質(請參考 ev_vector.h),
// 當 ev_vector<> 需要使用 ViewArea 時,
// 必須改用 TEvVectorVA<>
//
// 版本: $Id: EvVectorVA.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef EvVectorVAH
#define EvVectorVAH
//---------------------------------------------------------------------------
#include "ViewArea.h"
#include "ev_vector.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
template <class EvVector, class FieldMaker>
class TEvVectorVACursor : public TViewCursor
{
   typedef TViewCursor                             inherited;
   typedef TEvVectorVACursor<EvVector,FieldMaker>  this_type;
public:
//Types.
   typedef EvVector              container_type;
   typedef FieldMaker            field_maker_type;

//Constructor
   inline  K_ctor  TEvVectorVACursor  (container_type* c, const field_maker_type& fmaker = field_maker_type())
                     : inherited(),
                       EvVector_(c),
                       FieldMaker_(fmaker)
                     { if(c  &&  !c->empty())  DistBeginCatch_ = 0; }
   inline  K_ctor  TEvVectorVACursor  (const this_type& a)
                     : inherited(a),
                       EvVector_(a.EvVector_),
                       FieldMaker_(a.FieldMaker_)
                     { }

   K_mf(this_type*)  Clone  () const  { return new this_type(*this); }
   K_mf(this_type*)  Clone  (size_type distBegin, const ev_iterator&) const
                                      { this_type* r = new this_type(*this);
                                        r->DistBeginCatch_ = distBegin;
                                        return r; }

//取得顯示字串:由 FieldMaker 代勞
   K_mf(std::string)  GetStr  (field_n_type n) const  { return FieldMaker_.Get(GetIterator(),n); }
   K_mf(void)         PutStr  (field_n_type n, const std::string& s, TViewArea* logva)
                                                      { return FieldMaker_.Put(*EvVector_,GetIterator(),n,s); }
//assign operator
   K_mf(this_type&)  operator=  (const TViewCursor& cur)
            {  const this_type*  a = dynamic_cast<const this_type*>(&cur);
               if(a)
                  EvVector_ = a->EvVector_;
               inherited::operator=(cur);
               return *this;
            } // operator=()
//compare operator
   K_mf(bool)  operator==  (const TViewCursor& cur) const
            {  const this_type*  a = dynamic_cast<const this_type*>(&cur);
               return(a ? (DistBeginCatch_==a->DistBeginCatch_  &&  EvVector_==a->EvVector_) : false);
            } // operator==()

//移動位置,傳回:從指定位置(begin,end,current)走了幾步(可能<0,視steps而定)
//         steps請參考 TViewCursor::Move()的說明
   K_mf(int)  Move  (int steps, TViewCursorMoveFrom from)
            {  if(EvVector_->empty()) {
                  DistBeginCatch_ = -1;
                  return 0;
               }
               switch(from) {
               case vcmfBegin:   DistBeginCatch_ = 0;                   break;
               case vcmfEnd:     DistBeginCatch_ = EvVector_->size();   break;
               case vcmfCurrent:                                        break;
               default:                                                 return 0;
               }
               if(steps==0)
                  return 0;
               int maxsteps;
               if(steps > 0) {
                  maxsteps = EvVector_->size() - DistBeginCatch_;
                  if(steps >= maxsteps) {
                     DistBeginCatch_ = EvVector_->size() - 1;
                     return maxsteps;
                  }
               } else {//steps < 0
                  maxsteps = 0-DistBeginCatch_;
                  if(steps <= maxsteps) {
                     DistBeginCatch_ = 0;
                     return maxsteps;
                  }
               }
               DistBeginCatch_ += steps;
               return steps;
            } // Move()
   K_mf(bool)  Find  (const char* key, bool isLowerBound)
            {  if(EvVector_->empty()) {
                  DistBeginCatch_ = -1;
                  return false;
               }
               container_type::key_type   vkey = atoi(key);
               container_type::iterator   newitr = (isLowerBound ? EvVector_->lower_bound(vkey)
                                                                 : EvVector_->find(vkey));
               if(newitr == EvVector_->end()) {
                  if(isLowerBound) {
                     DistBeginCatch_ = EvVector_->size()-1;
                  } else
                     return false;
               } else {
                  DistBeginCatch_ = newitr.get_index();
               }
               return true;
            } // Find()

   inline K_mf(container_type::iterator)  GetIterator  () const  { return EvVector_->find(DistBeginCatch_); }
   inline K_mf(container_type*)           GetEvVector  () const  { return EvVector_; }
protected:
   container_type*   EvVector_;
   field_maker_type  FieldMaker_;

private:
   //調整 Iterator
   K_mf(void)  AdjDistBeginCatch  (bool& isAfErase, const ev_iterator& ifrom,
                                   size_type frombegin, size_type count)
   {  inherited::AdjDistBeginCatch(isAfErase, ifrom, frombegin, count);
      if(isAfErase) {
         if(EvVector_->empty())
            DistBeginCatch_ = -1;
         else
         if(DistBeginCatch_ >= EvVector_->size()) {
            --DistBeginCatch_;
            isAfErase = false;
         }
      }
   }
   K_mf(void) SetEvIterator (size_type frombegin, const ev_iterator&) {
      DistBeginCatch_ = frombegin;
   }
};
//---------------------------------------------------------------------------
template <class EvVector, class FieldMaker, class CursorType = TEvVectorVACursor<EvVector,FieldMaker> >
class TEvVectorVA : public TViewArea
{
   typedef TViewArea                         inherited;
   typedef TEvVectorVA<EvVector,FieldMaker>  this_type;
public:
   typedef EvVector     container_type;
   typedef FieldMaker   field_maker_type;
   typedef CursorType   cursor_type;

   K_ctor  TEvVectorVA  (container_type* c = 0, const FieldMaker& fmaker = FieldMaker(), const TViewControls& vcs = TViewControls())
                           : inherited(c ? CursorFactory(c,fmaker) : 0,
                                       c ? CursorFactory(c,fmaker) : 0,
                                       vcs),
                             EvVector_(c),
                             FieldMaker_(fmaker)
                           { if(c)   RegTo(*c); }
   K_dtor ~TEvVectorVA  ()  { call_OnBfDestroy();
                             if(EvVector_)  UnregFrom(*EvVector_); }

   inline  K_mf(const cursor_type*)  GetTopRow  () const  { return static_cast<const cursor_type*>(TopRow_); }
   inline  K_mf(const cursor_type*)  GetCurRow  () const  { return static_cast<const cursor_type*>(CurRow_); }

   static  K_mf(cursor_type*)  CursorFactory  (container_type* c, const FieldMaker& fmaker)
                                                   { return new cursor_type(c,fmaker); }

   K_mf(size_type)          GetRowCount    () const                { return EvVector_ ? EvVector_->size()  : 0;    }
   K_mf(bool)               IsEmpty        () const                { return EvVector_ ? EvVector_->empty() : true; }
   K_mf(field_n_type)       GetFieldCount  () const                { return FieldMaker_.GetFieldCount();  }
   K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const  { return FieldMaker_.GetFieldDesp(n);  }
   K_mf(std::string)        GetFieldStr    (const ev_iterator& i, field_n_type n) const
                                                                   { return FieldMaker_.Get(static_cast<const container_type::iterator&>(i),n); }

   virtual K_mf(bool)  Fetch  (const char* key, bool copyCurrent)
                                        { if(EvVector_ == 0) {
                                             if(copyCurrent)
                                                return false;
                                             container_type* c = OnCreateEvVector();
                                             if(c == 0)
                                                return false;
                                             SetEvVector(c);
                                          }
                                          if(copyCurrent  &&  GetCurRow()->IsInvalid())
                                             return false;
                                          container_type::key_type   ckey   = atoi(key);
                                          container_type::iterator   ifetch = EvVector_->fetch(ckey);
                                          if(copyCurrent) {
                                             EvVector::updater u = EvVector_->begin_update(ifetch);
                                             *u = *(GetCurRow()->GetIterator());
                                          }
                                          return Find(key,false); }
   virtual K_mf(bool)  Erase  (const char* key)
                                          { if(EvVector_) {
                                               container_type::key_type ckey = atoi(key);
                                               return EvVector_->erase(ckey);
                                            }
                                            return false;  }

   K_mf(void)  SetEvVector  (container_type* c)
                              { if(c == EvVector_)
                                   return;
                                container_type* bf = EvVector_;
                                EvVector_ = c;
                                AfChangeContainer(bf, c,
                                                  c ? CursorFactory(c,FieldMaker_) : 0,
                                                  c ? CursorFactory(c,FieldMaker_) : 0);
                              }
protected:
   container_type*   EvVector_;
   FieldMaker        FieldMaker_;

//override virtual ev handler
   K_mf(void) OnContainerBfDestroy (ev_container& c)
   {
      EvVector_ = 0;
      inherited::OnContainerBfDestroy(c);
   }

private:
   virtual K_mf(container_type*)  OnCreateEvVector  ()   { return 0; }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
