//
// �Ҧ� [ev_�e��] ����(ev_container.h)
//
// ����: $Id: ev_container.h,v 1.5 2006/04/04 00:34:12 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ev_containerH
#define ev_containerH
//---------------------------------------------------------------------------
#include "ev_handler.h"
//---------------------------------------------------------------------------
namespace Kstl {
#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif
//---------------------------------------------------------------------------
//
// Kstl �R�W�W�h����:
//   �ĥ� C++ STL ���R�W�W�h,
//   ���F ev_handler ���ƥ�B�z member functions (On...�t�C��mf)
//
/////////////////////////////////////////////////////////////////////////////
//
// �X�R(�ק�) std::containers �� containers �i�H�Q�� ev_container �Ӳ��ͨƥ�
// std::containers ���X�R:
//    ev_map<>  at  "ev_map.h"
//    ...
/////////////////////////////////////////////////////////////////////////////
//
// �i���Ѩƥ���U��� ev_container
//
LIBNS_class  container_ev_handler;
//---------------------------------------------------------------------------
LIBNS_class  ev_container  //������ ev ��O�� container
{
   typedef ev_container    this_type;
public:
   //Types.
   typedef size_t    size_type;

   //ev_iterator
   struct ev_iterator {
      virtual K_mf(bool)  operator==  (const ev_iterator&) const = 0;
   };

   //ev_updater_impl
   LIBNS_class  ev_updater_impl {
      ev_container*        container_;
      //const ev_iterator& ev_iter_;
      size_type            ref_count_;
      //�p�G�z���b bf_end_update()�����I�s cancel_update() �h�жǦ^ false
      virtual K_mf(bool)  bf_end_update  (bool forFlush)   { return true; }
   protected:
              K_mf(void)  cancel_update  ()   { ev_container* c = container_;
                                                if(c) {
                                                   container_ = 0;
                                                   c->end_update(*this);
                                                }
                                              }
   public:
      inline  K_ctor      ev_updater_impl  (ev_container& c)             : container_(&c), ref_count_(1) { }
      virtual K_dtor     ~ev_updater_impl  ()                            { }
      virtual K_mf(bool)  is_iterator_eq   (const ev_iterator& i) const  { return i==get_iterator(); }

      inline K_mf(void)  add_ref       ()   { ++ref_count_; }
             K_mf(void)  flush_update  ()   { if(bf_end_update(true)  &&  container_)
                                                 container_->flush_update(*this); }
             K_mf(bool)  end_update    ()   { if(--ref_count_==0) {
                                                 if(bf_end_update(false)  &&  container_)
                                                    container_->end_update(*this);
                                                 return true;
                                              }
                                              return false;
                                            }
    //920922:�]��iterator�O�i�H���N�ƻs������,���F�קKiterator��updater����! �ҥH����O�d ev_iter_
      virtual K_mf(const ev_iterator&) get_iterator  () const = 0;
    //inline K_mf(const ev_iterator&)  get_iterator  () const  { return ev_iter_;   }
      inline K_mf(const ev_container*) get_container () const  { return container_; }
      inline K_mf(size_type)           distance_begin() const  { return container_ ? container_->distance_begin(get_iterator()) : 0; }
   };

   //constructor/destructor
           K_ctor  ev_container  ();
   virtual K_dtor ~ev_container  ();

   //�ƥ���U����
   K_mf(void)  add_ev_handler  (container_ev_handler* a)  { evs_.add(a); }
   K_mf(void)  del_ev_handler  (container_ev_handler* a)  { evs_.del(a); }

   //pure virtual functions
   // �p��Y iterator �Z�� begin �h��,�Ǧ^�ȥû� >= 0
   virtual K_mf(size_type)  distance_begin  (const ev_iterator&) const = 0;

//��s container �����������
// ���ӥ� ev_container_u<>::begin_update() ���� updater ��, �I�s�o�̪� begin_update()
// �å� ev_updater_impl::end_update() �өI�s�o�̪� end_update();
private:
   friend class ev_updater_impl;
   virtual K_mf(ev_updater_impl*)  updater_factory  (const ev_iterator&) = 0;
           K_mf(void)              end_update       (ev_updater_impl&);
           K_mf(void)              flush_update     (ev_updater_impl&);
protected: K_mf(ev_updater_impl*)  begin_update     (const ev_iterator&);
   //�]������ container �u�O�N�z���ͨƥ�, ���t�d��ڸ�ƪ���s(�Ҧp ev_map2d<>)
   //�ҥH�����}�� member functions
   K_mf(void)  call_begin_update_evs  (ev_updater_impl&);
   K_mf(void)  call_end_update_evs    (ev_updater_impl&);

protected:
   //����ev, ���~�� ev_container �� class �ҩI�s
   K_mf(void) bf_destroy ();
   K_mf(void) bf_clear   ();
   K_mf(void) af_clear   ();
   K_mf(void) bf_assign  ();
   K_mf(void) af_assign  ();

  template <class value_type>
   inline K_mf(void)  bf_insert  (const ev_iterator& from,
                                  const size_type    count,
                                  const value_type&  //�Ȥ��ϥ�
                                 )  { bf_insert(from, count); }
   K_mf(void)  bf_insert  (const ev_iterator& from, const size_type count);
   K_mf(void)  af_insert  (const ev_iterator& from, const size_type count);
   K_mf(void)  bf_erase   (const ev_iterator& from, const size_type count);
   K_mf(void)  af_erase   (const ev_iterator& from, const size_type count);


private:

   typedef pod_vector<container_ev_handler*> container_ev_handlers;
   typedef container_ev_handlers::iterator   container_ev_handleri;
   container_ev_handlers   evs_;

   typedef pod_vector<ev_updater_impl*>   updaters;
   typedef updaters::iterator             updateri;
   updaters    updaters_;
};
//---------------------------------------------------------------------------
typedef ev_container::ev_iterator         ev_iterator;
typedef ev_container::ev_updater_impl     ev_updater_impl;
/////////////////////////////////////////////////////////////////////////////
template <typename T>
struct DeConst {
    typedef T  R;
};
//---------------------------------------------------------------------------
#ifndef __BORLANDC__
template <typename T>
struct DeConst<const T> {
   typedef T   R;
};
#endif
/////////////////////////////////////////////////////////////////////////////
template <class Iterator, class RecT>
struct rec_updater_impl : public ev_updater_impl {
   typedef typename DeConst<RecT>::R&       reference;
   typedef typename DeConst<RecT>::R*       pointer;
   //typedef typename Iterator::value_type  iterator_value;

   inline K_ctor  rec_updater_impl  (ev_container& c, const ev_iterator& i)
                     : ev_updater_impl(c),
                       Iter_(static_cast<const Iterator&>(i))
                     { }

   virtual K_mf(const Iterator&)  get_iterator  () const    { return Iter_; }
   virtual K_mf(reference)        get_modifyer  ()          { return const_cast<reference>(*Iter_); }
protected:
   Iterator    Iter_;
};
//---------------------------------------------------------------------------
template <class Iterator, class RecT>
struct pair_updater_impl : public ev_updater_impl {
   typedef typename DeConst<RecT>::R&       reference;
   typedef typename DeConst<RecT>::R*       pointer;
   //typedef typename Iterator::value_type  iterator_value;

   inline K_ctor  pair_updater_impl  (ev_container& c, const ev_iterator& i)
                     : ev_updater_impl(c),
                       Iter_(static_cast<const Iterator&>(i))
                     { }

   virtual K_mf(const Iterator&)  get_iterator  () const    { return Iter_; }
   virtual K_mf(reference)        get_modifyer  ()          { return const_cast<reference>(Iter_->second); }
protected:
   Iterator    Iter_;
};
/////////////////////////////////////////////////////////////////////////////
template <class Iterator, class UpdaterImpl>
class ev_updater
{
   typedef ev_updater<Iterator,UpdaterImpl>  this_type;
   UpdaterImpl*   impl_;
protected:
   inline K_mf(UpdaterImpl*)        get_impl  ()         { return impl_; }
   inline K_mf(const UpdaterImpl*)  get_impl  () const   { return impl_; }

public:
   typedef UpdaterImpl                       impl_type;
   typedef typename UpdaterImpl::reference   reference;
   typedef typename UpdaterImpl::pointer     pointer;

   K_ctor  ev_updater  (UpdaterImpl* impl = 0)  : impl_(impl)     { }
   K_ctor  ev_updater  (const this_type& a)     : impl_(a.impl_)  { if(impl_)  impl_->add_ref(); }
   K_dtor ~ev_updater  ()                       { end_update(); }

   K_mf(this_type&)  operator=  (const this_type& a)  { if(impl_ != a.impl_) {
                                                            end_update();
                                                            impl_ = a.impl_;
                                                            if(impl_)
                                                               impl_->add_ref();
                                                         }
                                                         return *this;
                                                       }
   inline K_mf(reference)  operator*   ()   { return impl_->get_modifyer();    }
   inline K_mf(pointer)    operator->  ()   { return &(impl_->get_modifyer()); }

   inline K_mf(const Iterator&)     get_iterator () const  { return static_cast<const Iterator&>(impl_->get_iterator()); }
   inline K_mf(const ev_container*) get_container() const  { return impl_->get_container(); }

   inline K_mf(bool)  is_invalid  () const { return impl_==0; }
   inline K_mf(void)  end_update  ()   { if(impl_) {
                                            impl_->end_update();
                                            impl_ = 0;
                                         }
                                       }
   inline K_mf(void)  flush_update  () { if(impl_)  impl_->flush_update(); }
}; //class ev_updater
/////////////////////////////////////////////////////////////////////////////
template <class Iterator>
class ev_container_i : public ev_container
{
  typedef ev_container  inherited;
public:
   //Iterators.
  typedef Iterator  orig_iterator;
  struct iterator : public ev_iterator, public orig_iterator
  {
   inline K_ctor          iterator   ()                        { }
   inline K_ctor          iterator   (const iterator& a)       : orig_iterator(a) { }
   inline K_ctor          iterator   (const orig_iterator& a)  : orig_iterator(a) { }
   inline K_mf(iterator&) operator=  (const iterator& a)       { *static_cast<orig_iterator*>(this) = a; return *this; }
   inline K_mf(iterator&) operator=  (const orig_iterator& a)  { *static_cast<orig_iterator*>(this) = a; return *this; }

   inline K_mf(bool)  operator==  (const iterator& a)    const  { return static_cast<const orig_iterator&>(a)==*this; }
   //override ev_iterator pure virtual function:
   inline K_mf(bool)  operator==  (const ev_iterator& a) const  { return operator==(static_cast<const iterator&>(a)); }
  };
};
//---------------------------------------------------------------------------
template <class EvContainerI, class UpdaterImpl>
class ev_container_u : public EvContainerI
{
  typedef EvContainerI  inherited;
public:
  typedef typename inherited::iterator   iterator;
  //Updater
  typedef ev_updater<iterator, UpdaterImpl>   updater;
  K_mf(updater)       begin_update     (const iterator& i)    { return updater(static_cast<UpdaterImpl*>(inherited::begin_update(i))); }
protected:
  K_mf(UpdaterImpl*)  updater_factory  (const ev_iterator& i) { return new UpdaterImpl(*this, i); }
}; // class ev_container_u
//---------------------------------------------------------------------------
template <class TOrgMap>
class ev_container_t1 : public ev_container_u<ev_container_i<typename TOrgMap::iterator>,
                                              rec_updater_impl<typename ev_container_i<typename TOrgMap::iterator>::iterator,
                                                               typename TOrgMap::mapped_type
                                                               > //rec_updater_impl<>
                                              >//ev_container_u
{  // ev_container_t1<> �Ȱ������U(²��) ev_container_t ���w�q
};
//---------------------------------------------------------------------------
template <class TOrgMap>
class ev_container_t2 : public ev_container_u<ev_container_i<typename TOrgMap::iterator>,
                                              pair_updater_impl<typename ev_container_i<typename TOrgMap::iterator>::iterator,
                                                                typename TOrgMap::mapped_type
                                                                > //pair_updater_impl<>
                                              >//ev_container_u
{  // ev_container_t2<> �Ȱ������U(²��) ev_container_t ���w�q
};
/////////////////////////////////////////////////////////////////////////////
enum container_ev_mask {
         cem_OnContainerBfDestroy,
         cem_OnContainerBfClear,
         cem_OnContainerAfClear,
         cem_OnContainerBfAssign,
         cem_OnContainerAfAssign,

         cem_OnContainerBfInsert,
         cem_OnContainerAfInsert,
         cem_OnContainerBfErase,
         cem_OnContainerAfErase,

         cem_OnContainerBeginUpdate,
         cem_OnContainerAfEndUpdate,

         cem_End,
};
//---------------------------------------------------------------------------
LIBNS_class  container_ev_handler : public ev_handler_base<cem_End>
{
   typedef ev_handler_base<cem_End>    inherited;
public:
   typedef size_t    size_type;

private:
   friend class ev_container;
   //�p�G���U�� virtual functions �������,
   //����~�Ӫ��H(��� handle �ƥ󪺤H)���ݭn handle �Өƥ�,
   //�ҥH�h�N ev_mask �M��, �H�ᤣ�ΦA���͸Өƥ�
   virtual K_mf(void)  OnContainerBfDestroy  (const ev_container&) { ev_mask_.reset(cem_OnContainerBfDestroy); }
   virtual K_mf(void)  OnContainerBfClear    (const ev_container&) { ev_mask_.reset(cem_OnContainerBfClear);   }
   virtual K_mf(void)  OnContainerAfClear    (const ev_container&) { ev_mask_.reset(cem_OnContainerAfClear);   }
   virtual K_mf(void)  OnContainerBfAssign   (const ev_container&) { ev_mask_.reset(cem_OnContainerBfAssign);  }
   virtual K_mf(void)  OnContainerAfAssign   (const ev_container&) { ev_mask_.reset(cem_OnContainerAfAssign);  }

   //��(ev_iterator& ifrom, size_type count)
   virtual K_mf(void)  OnContainerBfInsert   (const ev_container&, const ev_iterator&, size_type) { ev_mask_.reset(cem_OnContainerBfInsert); }
   virtual K_mf(void)  OnContainerAfInsert   (const ev_container&, const ev_iterator&, size_type) { ev_mask_.reset(cem_OnContainerAfInsert); }
   virtual K_mf(void)  OnContainerBfErase    (const ev_container&, const ev_iterator&, size_type) { ev_mask_.reset(cem_OnContainerBfErase); }
   virtual K_mf(void)  OnContainerAfErase    (const ev_container&, const ev_iterator&, size_type) { ev_mask_.reset(cem_OnContainerAfErase); }

   //����Ʋ���
   virtual K_mf(void)  OnContainerBeginUpdate  (const ev_updater_impl&) { ev_mask_.reset(cem_OnContainerBeginUpdate); }
   virtual K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl&) { ev_mask_.reset(cem_OnContainerAfEndUpdate); }
}; // class container_ev_handler
/////////////////////////////////////////////////////////////////////////////
/*
template <class Iterator>
class null_ev_container
{
   typedef null_ev_container     this_type;
public:
   typedef Iterator        ev_iterator;
   typedef Iterator        iterator;
   typedef size_t          size_type;
   class updater {
      const iterator  iter_;
   public:
      typedef iterator::pointer     pointer;
      typedef iterator::reference   reference;

             K_ctor           updater     (const iterator& a)   : iter_(a)  { }
      inline K_mf(void)       end_update  ()     {                            }
      inline K_mf(reference)  operator*   ()     { return *iter_;             }
      inline K_mf(pointer)    operator->  ()     { return iter_.operator->(); }
   };

   inline K_mf(void)  bf_destroy () const  { }
   inline K_mf(void)  bf_assign  () const  { }
   inline K_mf(void)  af_assign  () const  { }
   inline K_mf(void)  bf_clear   () const  { }
   inline K_mf(void)  af_clear   () const  { }
  template <class value_type>
   inline K_mf(void)  bf_insert  (ev_iterator&,   //from
                                  const size_type,//count
                                  const value_type&
                                 ) const   { }
   inline K_mf(void)  af_insert  (ev_iterator&,   //from
                                  const size_type //count
                                 ) const   { }
   inline K_mf(void)  bf_erase  (ev_iterator&,   //from
                                 const size_type //count
                                ) const    { }
   inline K_mf(void)  af_erase  (ev_iterator&,   //from
                                 const size_type //count
                                ) const     { }

   inline K_mf(updater)  begin_update  (const iterator& itr) const
    {
      return updater(itr);
    }
}; // class null_ev_container
*/
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

