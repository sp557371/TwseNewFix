//
// template ary_index<> :使用陣列對應的方式產生索引,如:用[股票代號前4碼]
//    ps.尚未完成!!
//
// 版本: $Id: ary_index.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef ary_indexH
#define ary_indexH
//---------------------------------------------------------------------------
#include "ev_container.h"
//---------------------------------------------------------------------------
#include <bitset>
//---------------------------------------------------------------------------
namespace Kstl {
/////////////////////////////////////////////////////////////////////////////
template <size_t N, class TRecs>//尚未完成,IndexMaker的處理方式
class ary_index : private container_ev_handler
{
public:
   typedef TRecs::iterator    iterator;
   typedef TRecs::key_type    key_type;

   K_ctor  ary_index  ()             : recs_(0)  { }
   K_ctor  ary_index  (TRecs& recs)  : recs_(0)  { re_index(recs); }
   K_dtor ~ary_index  ()             { if(recs_)  recs_->del_ev_handler(*this); }

   K_mf(void)  re_index  (TRecs& recs)
      {
         if(recs_ != &recs) {
            if(recs_)
               recs_->del_ev_handler(*this);
            recs_ = &recs;
            recs.add_ev_handler(*this); 
         }
         is_iter_valid_.reset();
         CreateIndex(recs.begin(), recs.size(), true);
      }

   K_mf(iterator)  find  (const key_type& key)
      {
         size_t   idx = IndexMaker(key);
         if(idx >= N)
            return recs_->find(key);
         if(is_iter_valid_.test(idx))
            return iters_[idx];
         return recs->end();
      }

private:

   //Container Destroy之後、Clear之後,全部的 iter 都變成無效
   K_mf(void)  OnContainerBfDestroy  (const ev_container&) { recs_ = 0;
                                                             is_iter_valid_.reset(); }
 //K_mf(void)  OnContainerBfClear    (const ev_container&);
   K_mf(void)  OnContainerAfClear    (const ev_container&) { is_iter_valid_.reset(); }

 //K_mf(void)  OnContainerBfAssign   (const ev_container&);
   K_mf(void)  OnContainerAfAssign   (const ev_container&) { re_index(*recs_); }

   //↓(ev_iterator& ifrom, size_type count)
 //K_mf(void)  OnContainerBfInsert   (const ev_container&, const ev_iterator&, size_type);
   K_mf(void)  OnContainerAfInsert   (const ev_container&, const ev_iterator& itr, size_type count)
                                       { CreateIndex(static_cast<const iterator&>(itr), count, true); }
   K_mf(void)  OnContainerBfErase    (const ev_container&, const ev_iterator& itr, size_type count)
                                       { CreateIndex(static_cast<const iterator&>(itr), count, false); }
 //K_mf(void)  OnContainerAfErase    (const ev_container&, const ev_iterator&, size_type);

   //↓資料異動: don't care
   //K_mf(void)  OnContainerBeginUpdate  (const ev_updater_impl&);
   //K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl&);
   K_mf(void)  CreateIndex  (iterator ifrom, size_type count, bool is_add)
      {
         while(count > 0) {
            size_t   idx = IndexMaker(*ifrom);
            if(idx < N) {
               is_iter_valid_.set(idx,is_add);
               if(is_add)
                  iters_[idx] = ifrom;
            }
            ++ifrom;
            --count;
         }
      }

   TRecs*            recs_;
   iterator          iters_[N];
   std::bitset<N>    is_iter_valid_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
