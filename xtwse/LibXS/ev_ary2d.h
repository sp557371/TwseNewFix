//
// template ev_ary2d<>
//
// ����: $Id: ev_ary2d.h,v 1.3 2006/04/03 14:29:15 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ev_ary2dH
#define ev_ary2dH
//---------------------------------------------------------------------------
#include "ary2d_recs.h"
#include "ev_container.h"
/////////////////////////////////////////////////////////////////////////////
namespace Kstl {
//
// Kstl �R�W�W�h����:�аѦ� "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
//
// ev_ary2d �� ev_ary �� 2���}�C
//  �Ҧp:�Ȥ�b���q 1 �� 999,999
//       ���i�� 1��20,000���s��(�Ψ䤤���p����������)
//       ���ۦ� 970,000�� ���b��...����,�q 20,001��969,999 ���ť�
//       �Y�ϥ� ev_ary �N�y�����j���Ŷ����O
//       �ҥH�� ev_ary2d<> ������
//
//  �Ҧp:�e�U�Ѹ� TTseOrdID:[X9999]���@��3���}�C,
//       �w�q�覡���I���c��, �аѦ� TwStkOrdMap.h
//
/////////////////////////////////////////////////////////////////////////////
template <class Key, class LeftAry, class RightAry,
          class EvContainerBase = ev_container_t1< ary2d_base<Key,LeftAry,RightAry> >
          >
class ev_ary2d : public EvContainerBase, protected ary2d_base<Key,LeftAry,RightAry>
{
   typedef ev_ary2d<Key,LeftAry,RightAry,EvContainerBase>   this_type;
   typedef ary2d_base<Key,LeftAry,RightAry>                 ary_base_type;
   typedef EvContainerBase                                  ev_inherited;

#ifndef __BORLANDC__
protected:
   using ev_inherited::bf_destroy;
   using ev_inherited::bf_assign;
   using ev_inherited::af_assign;
   using ev_inherited::bf_clear;
   using ev_inherited::af_clear;
   using ary_base_type::get_recs;
   using ary_base_type::size_catch_;
#endif

public:
//Types.
   typedef typename ary_base_type::key_type                 key_type;
   typedef typename ary_base_type::mapped_type              mapped_type;
   typedef typename ary_base_type::value_type               value_type;
   typedef typename ary_base_type::size_type                size_type;
   typedef typename ary_base_type::difference_type          difference_type;
   typedef typename ary_base_type::pointer                  pointer;
   typedef typename ary_base_type::const_pointer            const_pointer;
   typedef typename ary_base_type::reference                reference;
   typedef typename ary_base_type::const_reference          const_reference;
   typedef typename ary_base_type::left_iterator            left_iterator;
   typedef typename ary_base_type::right_iterator           right_iterator;
   typedef typename ary_base_type::left_type                left_type;
   typedef typename ary_base_type::right_type               right_type;

   typedef EvContainerBase                                  ev_container_base_type;
   typedef typename ev_container_base_type::ev_iterator     ev_iterator;
   typedef typename ev_container_base_type::iterator        iterator;
   typedef typename std::reverse_iterator<iterator>         reverse_iterator;
   typedef typename ary_base_type::const_iterator           const_iterator;
   typedef typename ary_base_type::const_reverse_iterator   const_reverse_iterator;
   typedef typename ev_inherited::updater                   updater;//���[�W�o��,gcc�|�䤣�� updater

//construct/copy/destroy
   inline K_ctor  ev_ary2d  ()  { }
   inline K_dtor ~ev_ary2d  ()  { bf_destroy(); }

   this_type&  operator=  (const this_type& a)  { bf_assign();
                                                  ary_base_type::operator=(a);
                                                  af_assign();
                                                  return *this;  }
//Iterators.
   inline K_mf(iterator)                begin  ()       { return ary_base_type::begin();  }
   inline K_mf(const_iterator)          begin  () const { return ary_base_type::begin();  }
   inline K_mf(iterator)                end    ()       { return ary_base_type::end();    }
   inline K_mf(const_iterator)          end    () const { return ary_base_type::end();    }
   inline K_mf(reverse_iterator)        rbegin ()       { return ary_base_type::rbegin(); }
   inline K_mf(const_reverse_iterator)  rbegin () const { return ary_base_type::rbegin(); }
   inline K_mf(reverse_iterator)        rend   ()       { return ary_base_type::rend();   }
   inline K_mf(const_reverse_iterator)  rend   () const { return ary_base_type::rend();   }

//Capacity.
   inline K_mf(size_type)  size     () const  { return ary_base_type::size();     }
   inline K_mf(bool)       empty    () const  { return ary_base_type::empty();    }
   inline K_mf(size_type)  max_size () const  { return ary_base_type::max_size(); }

//Element access.
   inline K_mf(const_reference)  operator[]  (const key_type& key)
   {
      return *fetch(key);
   } //operator[]

   //insert() member functions
   inline K_mf(iterator)  insert  (const iterator&, //positionHint,
                                   const value_type& v)
   {
      return insert_aux(v.first, &v.second).first;
   }
   template<class InputIterator>
    K_mf(void)  insert  (InputIterator first, InputIterator last)
    {
      //__t.insert(first, last);
      while(first != last)    insert(*first++);
    }
   inline std::pair<iterator,bool>  insert  (const value_type& v)
   {
      return insert_aux(v.first, &v.second);
   }

   //erase() member functions
   K_mf(size_type)  erase  (const key_type& a)  { iterator i = find(a);
                                                  if(i == end())
                                                     return 0;
                                                  return erase(i);
                                                }
   K_mf(bool)  erase  (const iterator& a)       { ary_erase_aux1  erase_aux(this,a);
                                                  return ary_base_type::erase1(a,erase_aux); }
   //erase() a range
   //TODO:�N���q���� ary2d_base<>
   struct ary_erase_aux2 : public RightAry::ary_erase_aux2 {
      inline K_mf(void)  clear_one  (const right_iterator&, const mapped_type&) const
                                       { } // ev_ary2d::erase()���ۦ�@�B�~���B�z,���|�����o��Ӫ�
   };
   K_mf(size_type)  erase  (iterator first, iterator last)
                        {
                           left_iterator   fmapi   = first.get_mapi();
                           right_iterator  frighti = first.get_righti();
                           if(last == end()) {
                              //�M�����ݸ��
                              if(first == begin()) {
                                 size_type  sz = size();
                                 clear();
                                 return sz;
                              }
                              size_type   count = last - first;
                              bf_erase(first,count);
                              fmapi->erase2(frighti, fmapi->end(), ary_erase_aux2());
                              if(!fmapi->empty())
                                 ++fmapi;
                              get_recs()->erase(fmapi, ary_base_type::end());
                              size_catch_ -= count;
                              af_erase(end(),count);
                              return(count);
                           }
                           size_type count = last - first;
                           if(count==0)
                              return 0;
                           left_iterator   emapi   = last.get_mapi();
                           right_iterator  erighti = last.get_righti();
                           if(fmapi == emapi)//�M���P�@�C�����
                              if(frighti==fmapi->begin()  ||  erighti==emapi->end()) {
                                 //�M���Y�C [�Y��or����] �����: �i��ڱN��� erase()
                                 bf_erase(first,count);
                                 bool isFromBegin = (first==begin());
                                 if(!isFromBegin)
                                    --first;//������}�l���e�@��,�ݷ|��af_erase()�ɭn��
                                 fmapi->erase2(frighti, erighti, ary_erase_aux2());
                                 if(fmapi->empty())
                                    get_recs()->erase(fmapi);
                                 size_catch_ -= count;
                                 if(isFromBegin)
                                    af_erase(begin(), count);
                                 else {
                                    ++first;
                                    af_erase(first, count);
                                 }
                                 return(count);
                              } else {
                                 //�M���Y�C [����] �����: �u��N��ƲM��
                                 mapped_type defaultRec = mapped_type();
                                 do {
                                    updater  u = begin_update(first);
                                    *u = defaultRec;
                                 } while(++first != last);
                                 return 0;
                              }
                           //�M���}�l(first)�C������,�쵲��(last)�C���Y��
                           bf_erase(first,count);
                           bool isFromBegin = (first==begin());
                           if(!isFromBegin)
                              --first;//������}�l���e�@��,�ݷ|��af_erase()�ɭn��
                           fmapi->erase2(frighti, fmapi->end(), ary_erase_aux2());//�M���}�l�C������
                           for(left_iterator fL = fmapi;;) {
                              if(++fL == emapi) {
                                 fL->erase2(fL->begin(), erighti, ary_erase_aux2());//�M���̫�C���Y��
                                 break;
                              }
                              fL->clear();//�M��[����]�C,��C�M��
                           } // for(fL
                           if(fmapi->empty()) {
                              if(!emapi->empty())
                                 --emapi;
                              //�N�����S��ƪ�[�C]����(�O�_��ڱqrecs������,��recs.erase()�өw)
                              get_recs()->erase(fmapi,emapi);
                           }
                           size_catch_ -= count;
                           if(isFromBegin)
                              af_erase(begin(), count);
                           else {
                              ++first;
                              af_erase(first, count);
                           }
                           return count;
                        }

   K_mf(void)  swap  (this_type& a)
                        {  bf_assign();
                           a.bf_assign();
                           ary_base_type::swap(a);
                           a.af_assign();
                           af_assign();
                        }
   K_mf(void)  clear  ()
                        {  bf_clear();
                           ary_base_type::clear();
                           af_clear();
                        }

//ary operations
   inline K_mf(iterator)        find        (const key_type& key)       { return ary_base_type::find(key);        }
   inline K_mf(const_iterator)  find        (const key_type& key) const { return ary_base_type::find(key);        }
   inline K_mf(iterator)        lower_bound (const key_type& key)       { return ary_base_type::lower_bound(key); }
   inline K_mf(const_iterator)  lower_bound (const key_type& key) const { return ary_base_type::lower_bound(key); }
   inline K_mf(iterator)        upper_bound (const key_type& key)       { return ary_base_type::upper_bound(key); }
   inline K_mf(const_iterator)  upper_bound (const key_type& key) const { return ary_base_type::upper_bound(key); }

//ev_ary extended: update record
       inline K_mf(iterator)            fetch       (const key_type& key)     { return insert_aux(key, 0).first; }

//�X�R�� ViewArea �ϥΪ� members
static inline K_mf(const mapped_type&)  get_mapped  (const iterator& i)       { return *i; }
static inline K_mf(const key_type&)     get_key     (const iterator& i);
static inline K_mf(const mapped_type&)  get_mapped  (const const_iterator& i) { return *i; }
static inline K_mf(const key_type&)     get_key     (const const_iterator& i) ;

//override ev_container virtual functions
    K_mf(size_type)  distance_begin  (const ev_iterator& from) const
                                     { const iterator  ifrom  = static_cast<const iterator&>(from);
                                       const iterator  ibegin = const_cast<this_type*>(this)->begin();
                                       return(ifrom - ibegin); }
protected:
   typedef typename ary_base_type::recs_type    recs_type;
   typedef typename ary_base_type::index_type   index_type;

   class ary_insert_aux : private ary_base_type::ary_insert_aux
   {
      friend class ev_ary2d;//this_type;
      typedef typename ary_base_type::ary_insert_aux  inherited;
      this_type*     this_ary2d_;
   public:
      K_ctor  ary_insert_aux  (this_type* a) : this_ary2d_(a)  { }

      K_mf(void)  insert_auxev  (recs_type*         recs,
                                 index_type         from,
                                 size_type          count,
                                 const mapped_type& value) const
      {
         iterator ifrom;
         this->make_bf_iterator(recs,from,ifrom);
         this_ary2d_->bf_insert(ifrom, count, value);
         inherited::operator()(recs,from,count,&value);
         this_ary2d_->af_insert(iterator(this->make_af_iterator(recs,from,count)), count);
      }
      K_mf(void)  operator()  (recs_type*         recs,
                               index_type         from,
                               size_type          count,
                               const mapped_type* value) const
      {
         evary_insert_aux(this, recs, from, count, value);
      }
   }; // struct ary_insert_aux
   friend class ary_insert_aux;
   std::pair<iterator,bool>  insert_aux  (const key_type& key, const mapped_type* value)
   {
      ary_insert_aux  insert_aux(this);
      return ary_base_type::insert(key,value,insert_aux);
   }

   struct ary_erase_aux1 : public ary_base_type::ary_erase_aux1 {
      friend class ev_ary2d;//this_type;
      typedef typename ary_base_type::ary_erase_aux1  inherited;
      typedef typename inherited::right_iterator      right_iterator;
      typedef typename this_type::iterator            iterator;
      this_type*  this_ary2d_;
      iterator    ifrom_;
      K_ctor  ary_erase_aux1  (this_type* a, const iterator& i)  : this_ary2d_(a),ifrom_(i)  {}

      K_mf(void)  erase_begin  (recs_type* recs) const
               {
               iterator iaf = ifrom_;
               ++iaf;
               this_ary2d_->bf_erase(ifrom_,1);
               inherited::erase_begin(recs);
               this_ary2d_->af_erase(iaf,1);
               }
      K_mf(void)  erase_back  (recs_type* recs, const typename recs_type::iterator& oiback) const
               {
               iterator iaf = ifrom_;
               ++iaf;
               this_ary2d_->bf_erase(ifrom_,1);
               inherited::erase_back(recs, oiback);
               this_ary2d_->af_erase(iaf,1);
               }
      K_mf(void)  clear_one  (const right_iterator&) const
               {
               updater  u = this_ary2d_->begin_update(ifrom_);
               *u = mapped_type();
               }
   }; // struct ary_erase_aux1
   friend struct ary_erase_aux1;
   //no data members
};// class ev_ary2d
//---------------------------------------------------------------------------
template <class Key, class LeftAry, class RightAry, class EvContainerBase>
inline void swap (ev_ary2d<Key,LeftAry,RightAry,EvContainerBase>& a,
                  ev_ary2d<Key,LeftAry,RightAry,EvContainerBase>& b)  { a.swap(b); }
/////////////////////////////////////////////////////////////////////////////
// typedef ary_base<Key,RightAry,LeftIndexMaker,pod_vector<RightAry> >    LeftAry;
template <class Key, class LeftIndexMaker, class RightAry,
          class EvContainerBase = ev_container_t1<ary2d_base<Key,
                                                             ary_base<Key,RightAry,LeftIndexMaker,pod_vector<RightAry> >,
                                                             RightAry
                                                             >
                                                   >
          >
class ev_ary2dd : public ev_ary2d<Key,
                                  ary_base<Key,RightAry,LeftIndexMaker,pod_vector<RightAry> >,
                                  RightAry,
                                  EvContainerBase
                                  >
{
};
//---------------------------------------------------------------------------
template <class Key, class LeftIndexMaker, class RightAry, class EvContainerBase>
inline void swap (ev_ary2dd<Key,LeftIndexMaker,RightAry,EvContainerBase>& a,
                  ev_ary2dd<Key,LeftIndexMaker,RightAry,EvContainerBase>& b)  { a.swap(b); }
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

