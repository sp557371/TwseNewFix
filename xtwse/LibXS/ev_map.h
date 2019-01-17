//
// template ev_map<>
//
// ����: $Id: ev_map.h,v 1.5 2006/04/03 14:28:44 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ev_mapH
#define ev_mapH
//---------------------------------------------------------------------------
#include "ev_container.h"
#include <map>
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl �R�W�W�h����:�аѦ� "ev_container.h"
//
//���F�קK�L�k�x�� Rec ������, ��ĳ�N RecT �]�� const
//�����ʪ��ݨD��, �h�ϥ� EvContainerBase::begin_update()
//�ԲӪ������аѦ� "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
template <class Key, class RecT,
#ifdef __alpha//���F�Y�u�balpha�����W�� symbol name
          class EvContainerBase = ev_container_t2<std::map<Key,RecT> >
#else
          class Compare         = std::less<Key>,
          class Allocator       = std::allocator<std::pair<const Key, RecT> >,
          class EvContainerBase = ev_container_t2<std::map<Key,RecT,Compare,Allocator> >
#endif
          >
class ev_map : public EvContainerBase
{
#ifdef __alpha
   typedef ev_map<Key, RecT, EvContainerBase>                     this_type;
   typedef std::less<Key>                                         Compare;
   typedef std::allocator<std::pair<const Key, RecT> >            Allocator;
#else
   typedef ev_map<Key, RecT, Compare, Allocator, EvContainerBase> this_type;
#endif
   typedef std::map<Key,RecT,Compare,Allocator>                   like_type;
   typedef EvContainerBase                                        inherited;
public:
//Types.
    typedef Key                           key_type;
    typedef RecT                          mapped_type;
    typedef EvContainerBase               ev_container_base_type;
    typedef std::pair<const Key, RecT>    value_type;
    typedef Compare                       key_compare;
    typedef Allocator                     allocator_type;
private:
    /*typedef __RWSTD::__rb_tree<key_type, value_type,
                               __RWSTD::__select1st<value_type, key_type>,
                               key_compare, allocator_type> __rep_type;*/
    typedef std::map<key_type, mapped_type, key_compare, allocator_type>   __rep_type;
    __rep_type       __t;

#ifndef __BORLANDC__
protected:
    using inherited::bf_destroy;
    using inherited::bf_assign;
    using inherited::af_assign;
    using inherited::bf_clear;
    using inherited::af_clear;
#endif

public:
//Types.
    typedef typename __rep_type::reference                reference;
    typedef typename __rep_type::pointer                  pointer;
    typedef typename __rep_type::const_reference          const_reference;
    typedef typename __rep_type::const_pointer            const_pointer;
    typedef typename __rep_type::size_type                size_type;
    typedef typename __rep_type::difference_type          difference_type;
//Iterator Types.
  //typedef typename __rep_type::iterator                 iterator;
    typedef typename __rep_type::const_iterator           const_iterator;
  //typedef typename __rep_type::reverse_iterator         reverse_iterator;
    typedef typename __rep_type::const_reverse_iterator   const_reverse_iterator;

    typedef typename ev_container_base_type::ev_iterator  ev_iterator;
    typedef typename ev_container_base_type::iterator     iterator;
    typedef typename std::reverse_iterator<iterator>      reverse_iterator;

//construct/copy/destroy
    explicit
      K_ctor  ev_map (const Compare& comp = Compare(), const Allocator& alloc = Allocator())
                     //: __t(comp, false, alloc) {}
                       : __t(comp, alloc) {}

    template<class InputIterator>
     K_ctor  ev_map (InputIterator first, InputIterator last, const Compare& comp = Compare(),
                                                            const Allocator& alloc = Allocator())
                     //: __t(first, last, comp, false, alloc) {}
                       : __t(first, last, comp, alloc) {}

   inline K_ctor   ev_map (const this_type& x) : __t(x.__t) {}//: __t(x.__t, false) {}
   inline K_dtor  ~ev_map ()                   { bf_destroy(); }

    K_mf(this_type&)  operator=  (const this_type& x) { bf_assign();
                                                        __t = x.__t;
                                                        af_assign();
                                                        return *this; }

   inline K_mf(allocator_type)  get_allocator  () const  { return __t.get_allocator(); }

   inline K_mf(bool) operator== (const this_type& x) const { return __t == x.__t; }
   inline K_mf(bool) operator!= (const this_type& x) const { return __t != x.__t; }

//iterators
   inline K_mf(iterator)               begin  ()       { return __t.begin();  }
   inline K_mf(const_iterator)         begin  () const { return __t.begin();  }
   inline K_mf(iterator)               end    ()       { return __t.end();    }
   inline K_mf(const_iterator)         end    () const { return __t.end();    }

   /** �p�G�ϥ�BCB4(BCB5,BCB6�w�ץ�),�h�ݭn�ק� iterator.h
         �쥻:
            template <class U>
            reverse_iterator (const reverse_iterator<U>& x) : current(x) {}
         �ק��:
            template <class U>
            reverse_iterator (const reverse_iterator<U>& x) : current(x.base()) {}
   */
   inline K_mf(reverse_iterator)       rbegin ()       { return __t.rbegin(); }
   inline K_mf(const_reverse_iterator) rbegin () const { return __t.rbegin(); }
   inline K_mf(reverse_iterator)       rend   ()       { return __t.rend();   }
   inline K_mf(const_reverse_iterator) rend   () const { return __t.rend();   }

//capacity
   inline K_mf(bool)       empty    () const { return __t.empty();    }
   inline K_mf(size_type)  size     () const { return __t.size();     }
   inline K_mf(size_type)  max_size () const { return __t.max_size(); }

//element access
   K_mf(mapped_type&)  operator[]  (const key_type& k)
   {
      value_type tmp(k,mapped_type());
      return (*((insert(tmp)).first)).second;
   }
   std::pair<iterator,bool>  insert  (const value_type& v)
   {
      iterator position = lower_bound(v.first);
      if(position != end()) {
         key_compare keycomp = __t.key_comp();
         if(!keycomp(position->first, v.first)   //key�ۦP
         && !keycomp(v.first, position->first))  //���ݭn insert()
            return std::pair<iterator,bool>(position,false);
      }
      this->bf_insert(position,1,v);
      std::pair<iterator,bool>   r(__t.insert(position,v),true);
      this->af_insert(r.first,1);
      return r;
   }
   K_mf(iterator)  insert  (iterator position, const value_type& v)
   {
      key_compare    keycomp = __t.key_comp();
      iterator       iend    = end();
      if(position == iend) {                          //position==end
         if(empty())                                  //  is empty
            return insert(v).first;                   //  => normal insert
         // v ���ӭn�b rightmost ����
         --iend;
         if(keycomp(v.first, iend->first)) // v �b rightmost ���e => position is incorrect
            return insert(v).first;        //   => normal insert
                                           // v ���b rightmost ���e(=����or�۵�)
         if(keycomp(iend->first, v.first)) {// => �B rightmost �b v ���e
                                            //       => v �b rightmost ���� => position is correct
         } else                             // => �B rightmost ���b v ���e
            return iend;                    //       => v ����m�N�b rightmost
      } else {
         if(keycomp(v.first, position->first)) { // v �b position ���e => position maybe correct
            if(position == begin()) {            //   position==begin  => position is correct
            } else {                             // �P�_:�Y v �b before(=position-1)����
               iterator before = position;       //      �h position is correct
               --before;
               if(keycomp(v.first, before->first))// v �b before ���e => position is incorrect
                  return insert(v).first;         //   => normal insert
               // v �b before ����or�۵�
               if(keycomp(before->first, v.first)) {// before �b v ���e => v �b before ����
                                                    //    position is correct
               } else                               // => v ����m�N�b before
                  return before;                    //    return before;
            }
         } else// v �b position ����or�۵�
         if(keycomp(position->first, v.first))  // position �b v ���e => v �b position ����
            return insert(v).first;             //    => position is incorrect => normal insert
         else                                   // => v ����m�N�b position
            return position;                    //      return position
      }
      //position ����m�O���T��:
      this->bf_insert(position,1,v);
      position = __t.insert(position, v);
      this->af_insert(position,1);
      return position;
   }

   template<class InputIterator>
    K_mf(void)  insert  (InputIterator first, InputIterator last)
    {
      //__t.insert(first, last);
      while(first != last)    insert(*first++);
    }

   K_mf(bool)  erase  (iterator position)
                        {  this->bf_erase(position,1);
                           iterator afpos = position;
                           bool     isend = (++afpos == end());//�R���̫�@��
                           __t.erase(position);
                           this->af_erase((isend ? end() : afpos), 1);
                           return true;
                        }
   K_mf(size_type)  erase  (const key_type& x)
                        {
                           iterator i = find(x);
                           if(i == end())
                              return 0;
                           return erase(i);
                        }
   K_mf(size_type)  erase  (iterator first, iterator last)
                        {
                          iterator iend = end();
                          if(first==begin()  &&  last==iend) {
                             size_type sz = size();
                             clear();
                             return sz;
                          }
                          size_type  count = std::distance(first,last);
                          if(count > 0) {
                              this->bf_erase(first,count);
                              bool  isend = (last==iend);
                              __t.erase(first,last);
                              this->af_erase((isend ? end() : last), count);
                          } // if(count > 0
                          return count;
                        }

   K_mf(void)  swap  (this_type& v)  { bf_assign();
                                       v.bf_assign();
                                       __t.swap(v.__t);
                                       v.af_assign();
                                       af_assign(); }
   K_mf(void)  clear  ()   { bf_clear();
                             __t.erase(begin(), end());
                             af_clear(); }

//observers
   inline K_mf(key_compare)   key_comp   () const { return __t.key_comp(); }

//map operations
   inline K_mf(iterator)       find (const key_type& x)        { return __t.find(x);  }
   inline K_mf(const_iterator) find (const key_type& x)  const { return __t.find(x);  }
   inline K_mf(size_type)      count (const key_type& x) const { return __t.count(x); }
   inline K_mf(iterator)       lower_bound (const key_type& x) { return __t.lower_bound(x); }
   inline K_mf(iterator)       upper_bound (const key_type& x) { return __t.upper_bound(x); }
   inline K_mf(const_iterator) lower_bound (const key_type& x) const { return __t.lower_bound(x); }
   inline K_mf(const_iterator) upper_bound (const key_type& x) const { return __t.upper_bound(x); }

   inline std::pair<iterator,iterator>             equal_range (const key_type& x)        { return __t.equal_range(x); }
   inline std::pair<const_iterator,const_iterator> equal_range (const key_type& x) const  { return __t.equal_range(x); }

//ev_map extended: update record
   K_mf(iterator)  fetch  (const key_type& k)
   {
      iterator position = lower_bound(k);
      if(position == end()
      || __t.key_comp()(position->first, k)
      || __t.key_comp()(k, position->first))
         return insert(position, value_type(k,mapped_type()));
      return position;
   }
//�X�R�� ViewArea �ϥΪ� members
   static inline K_mf(const key_type&)     get_key     (const iterator& i)       { return i->first;  }
   static inline K_mf(const mapped_type&)  get_mapped  (const iterator& i)       { return i->second; }
   static inline K_mf(const key_type&)     get_key     (const const_iterator& i) { return i->first;  }
   static inline K_mf(const mapped_type&)  get_mapped  (const const_iterator& i) { return i->second; }

//override ev_container virtual functions
    K_mf(size_type)  distance_begin  (const ev_iterator& from) const
                                     { const_iterator ibegin = begin();
                                       const_iterator ifrom  = static_cast<const iterator&>(from);
                                       return std::distance(ibegin,ifrom); }
}; //class ev_map
//---------------------------------------------------------------------------
#ifdef __alpha
template <class Key, class RecT, class EvContainerBase>
inline void swap(ev_map<Key,RecT,EvContainerBase>& a,
                 ev_map<Key,RecT,EvContainerBase>& b)
  {
    a.swap(b);
  }
#else
template <class Key, class RecT, class Compare, class Allocator, class EvContainerBase>
inline void swap(ev_map<Key,RecT,Compare,Allocator,EvContainerBase>& a,
                 ev_map<Key,RecT,Compare,Allocator,EvContainerBase>& b)
  {
    a.swap(b);
  }
/////////////////////////////////////////////////////////////////////////////
//�p�G�z�� EvContainerBase �P�w�]�����P, �åB���ϥ� ev_map<> ���ѼƳ·�,
//�h�z�i�H���w�q�n typedef std::map<...>   stdmap;
//�A�ϥ� ev_map2<> �өw�q ev_map<>
template <class stdmap,
          class EvContainerBase>
class ev_map2 : public ev_map<typename stdmap::key_type,
                              const typename stdmap::mapped_type,//����n�b���B�[�W const:
                                                        //�]���Y�ϱz�b�w�q stdmap �ɦ��[�W const,
                                                        //�b���B�Y�S���[�W const,
                                                        //BCB�|�N mapped_type �����D const
                                                        //�o�O BCB �����D, �٬O�зǪ��w�q�N�O�p���O?
                              typename stdmap::key_compare,
                              typename stdmap::allocator_type,
                                       EvContainerBase>
{
};
//---------------------------------------------------------------------------
template <class stdmap, class EvContainerBase>
inline void swap(ev_map2<stdmap,EvContainerBase>& a,
                 ev_map2<stdmap,EvContainerBase>& b)
  {
    a.swap(b);
  }
#endif   // __alpha..else
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
