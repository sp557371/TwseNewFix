//
// template ary_recs<>
//
// ����: $Id: ary_recs.h,v 1.5 2006/04/03 14:22:20 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ary_recsH
#define ary_recsH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "wise_obj.h"
//---------------------------------------------------------------------------
#include <vector>
#include <limits>
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl �R�W�W�h����:�аѦ� "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
#ifdef __alpha
#define  ary_recs_iterator_base  ArIb
#define  index_maker             ArIm
#define  const_ary_base          CayB
#endif
#ifdef _MSC_VER
#pragma warning(disable:4267 4244)
#endif
/////////////////////////////////////////////////////////////////////////////
//
// ary_recs<> ²�檺�X�R�F vector<> ���@�ǥ\��,
// (�D�n�O�� ary_base<> �Ϊ����U���O)
// �ϱo�b�ϥ� operator[] ��, �i�H�����q 0 �}�l
// ���]�]���b�ϥΤW, �n��u�@�� ary_recs<> ���W�h:
//   �I�s erase() ��,��[�ۦ�]�T�w���O�qbegin�}�l
//                   �Y�q begin() �}�l, �h�ЩI�s erase_begin()
//   �ФũI�s insert(), �Ч�� insert_from();
//
template <class RecsBase, class IndexType>  // RecsBase = std::vector<RecT>
class ary_recs : private RecsBase
{
   typedef ary_recs<RecsBase,IndexType>   this_type;
   typedef RecsBase                       inherited;

   IndexType   index_begin_;
public:
   typedef IndexType                            index_type;
   typedef typename inherited::reference        reference;
   typedef typename inherited::pointer          pointer;
   typedef typename inherited::const_reference  const_reference;
   typedef typename inherited::const_pointer    const_pointer;
   typedef typename inherited::value_type       mapped_type;
   typedef typename inherited::size_type        size_type;
   typedef typename inherited::iterator         iterator;
   typedef typename inherited::const_iterator   const_iterator;
   typedef typename inherited::difference_type  difference_type;

   K_ctor  ary_recs  ()                   : index_begin_(0) { }
   K_ctor  ary_recs  (const this_type& a) : inherited(a), index_begin_(a.index_begin_){ }

   K_mf(this_type&)  operator=  (const this_type& a) { inherited::operator=(a);
                                                       index_begin_ = a.index_begin_;
                                                       return *this;  }
   inline K_mf(void)  clear  ()              { inherited::clear();  index_begin_ = 0; }
   inline K_mf(void)  swap   (this_type& a)  { inherited::swap(a);
                                               std::swap(index_begin_, a.index_begin_); }
   inline K_mf(size_type)  size   () const   { return inherited::size();              }
   inline K_mf(bool)       empty  () const   { return inherited::empty();             }

   inline K_mf(iterator)        begin  ()         { return inherited::begin(); }
   inline K_mf(iterator)        end    ()         { return inherited::end();   }
   inline K_mf(const_iterator)  begin  () const   { return inherited::begin(); }
   inline K_mf(const_iterator)  end    () const   { return inherited::end();   }

   inline K_mf(reference)        operator[] (index_type a)        { return inherited::operator[](a - index_begin_); }
   inline K_mf(const_reference)  operator[] (index_type a) const  { return inherited::operator[](a - index_begin_); }

   inline K_mf(index_type)  get_index_end   () const  { return static_cast<index_type>(index_begin_ + size()); }
   inline K_mf(index_type)  get_index_begin () const  { return index_begin_; }

   inline K_mf(void)  erase_begin  (size_type count)  { iterator ibegin = begin();
                                                        inherited::erase(ibegin, ibegin+count);
                                                        if(empty())  index_begin_ = 0;
                                                        else         index_begin_ = static_cast<index_type>(index_begin_ + count);
                                                      }
   inline K_mf(void)  erase_begin  ()                 { inherited::erase(begin());
                                                        if(empty())  index_begin_ = 0;
                                                        else         ++index_begin_;
                                                      }
   inline K_mf(void)  erase_not_begin  (iterator position)              { inherited::erase(position);   }
   inline K_mf(void)  erase_not_begin  (iterator first, iterator last)  { inherited::erase(first,last); }
   inline K_mf(void)  reserve          (size_type sz)                   { inherited::reserve(sz);       }

   K_mf(void)  insert_from (index_type from, size_type count, const mapped_type& value)
      {  //��}�C��empty()��, from = index_maker<> ����,
         //�_�hfrom��,�}�C�����ޭ�(�q0�}�l),
         if(empty()) {
            index_begin_ = from;
            this->insert(begin(), count, value);
         } else {
            this->insert(begin()+static_cast<size_type>(from), count, value);
            if(from==static_cast<index_type>(0))
               index_begin_ = static_cast<index_type>(index_begin_ - count);
         }
      }
};
/////////////////////////////////////////////////////////////////////////////
//
// ary_recs_iterator_base<>
//
template <class AryRecs, class ValueType>
class ary_recs_iterator_base
{
  typedef ary_recs_iterator_base<AryRecs,ValueType>   this_type;
public:
  typedef std::random_access_iterator_tag       iterator_category;
  typedef AryRecs                               recs_type;
  typedef typename recs_type::index_type        index_type;
  typedef ValueType                             value_type;
  typedef value_type&                           reference;
  typedef value_type*                           pointer;
  typedef typename recs_type::difference_type   difference_type;

  inline K_ctor    ary_recs_iterator_base ()                                       { }
  inline K_ctor    ary_recs_iterator_base (recs_type* recs, const index_type idx)  : recs_(recs), idx_(idx)  { }
  inline K_mf(reference)   operator*    () const          { return (*recs_)[idx_];                         }
  inline K_mf(pointer)     operator->   () const          { return &(operator*());                         }
  inline K_mf(this_type&)  operator++   ()                { ++idx_; return *this;                          }
  inline K_mf(this_type)   operator++   (int)             { this_type tmp = *this;  ++(*this); return tmp; }
  inline K_mf(this_type&)  operator--   ()                { --idx_; return *this;                          }
  inline K_mf(this_type)   operator--   (int)             { this_type tmp = *this;  --(*this); return tmp; }
  inline K_mf(bool)        operator==   (const this_type& a) const    { return idx_ == a.idx_;   }
  inline K_mf(bool)        operator!=   (const this_type& a) const    { return idx_ != a.idx_;   }

  inline K_mf(difference_type) operator-  (const this_type& a) const  { return idx_ - a.idx_;    }
  inline K_mf(this_type&)      operator+= (const difference_type n)   { idx_ = (index_type)(idx_ + n);
                                                                        return *this; }
  inline K_mf(this_type) operator+ (difference_type n)   { return this_type(recs_, idx_+n); }
  inline K_mf(this_type) operator- (difference_type n)   { return this_type(recs_, idx_-n); }

  inline K_mf(recs_type*)  get_recs () const  { return recs_; }
  inline K_mf(index_type)  get_idx  () const  { return idx_;  }

protected:
  recs_type*   recs_;
  index_type   idx_;
};
/////////////////////////////////////////////////////////////////////////////
//
// ary ���@�ت����O��������� map
// �� std::map �ݭn�@�� Compare, �� ary �h�ݭn�@�ӥi�H�N Key �ܦ� index �� IndexMaker
//       IndexMaker �ݭn:
//          (1) ��ƥi�N Key ���ܦ� index ����O
//          (2) �w�q index_type
//          (3) �i�Ѧҩ��U�� template<class Key> struct index_maker {};
// �� std::map �ݭn�@�� Allocator, �� ary �h�ݭn�@�� vector<> or deque<> or pod_vector<>
//
/////////////////////////////////////////////////////////////////////////////
struct index_maker_base  {
   inline K_mf(void)  swap  (index_maker_base&)  { }
};        
//---------------------------------------------------------------------------
template <class Key>
struct index_maker : public index_maker_base {
   typedef Key index_type;
   inline K_mf(index_type)  operator()  (index_type a) const  { return a; }
};
/////////////////////////////////////////////////////////////////////////////
//
// ary_base �Ĩ��ϥ� wise_container<ary_recs<> > ���覡(���ĥ��~�Ӫ��覡), �]�t�@�� ary_recs
//
template <class Key,
          class RecT,
          class IndexMaker        = index_maker<Key>,
          class Container         = std::vector<RecT>,
          class IteratorValueType = typename Container::value_type >
class ary_base
{
   typedef ary_base<Key,RecT,IndexMaker,Container,IteratorValueType>    this_type;
   typedef          Container                         recs_org_type;
protected:
   typedef          IndexMaker                        index_maker_type;
   typedef typename recs_org_type::iterator           orig_iterator;
public:
//Types.
   typedef typename index_maker_type::index_type      index_type;
   typedef          Key                               key_type;
   typedef          IteratorValueType                 mapped_type;
   typedef          std::pair<key_type,mapped_type>   value_type;
   typedef typename recs_org_type::allocator_type     allocator_type;
   typedef typename recs_org_type::size_type          size_type;
   typedef typename recs_org_type::difference_type    difference_type;
   typedef typename recs_org_type::pointer            pointer;
   typedef typename recs_org_type::const_pointer      const_pointer;
   typedef typename recs_org_type::reference          reference;
   typedef typename recs_org_type::const_reference    const_reference;

//Iterator Types.
// ary_base::iterator �㦳<���ĩ�>, �]�N�O�����D��Ƥw�Qerase, �_�h iterator �@������, �� end() ���~
//protected: gcc33 �����w�ڧ� recs_type ��b protected
   typedef ary_recs<recs_org_type,index_type>         recs_type;
public:
   typedef ary_recs_iterator_base<recs_type, IteratorValueType>   iterator;
   friend class ary_recs_iterator_base<recs_type, IteratorValueType>;//iterator, gcc 4�����\�����ϥ� class iterator;

   typedef ary_recs_iterator_base<const recs_type, const IteratorValueType>   const_iterator_base;
   friend class ary_recs_iterator_base<const recs_type, const IteratorValueType>;//const_iterator_base;
   class const_iterator : public const_iterator_base {
      typedef typename index_maker_type::index_type   index_type;
      typedef const_iterator_base                     inherited;
   public:
      typedef const typename inherited::recs_type   recs_type;
      inline K_ctor  const_iterator  ()                                          { }
      inline K_ctor  const_iterator  (recs_type* recs, const index_type idx)     : inherited(recs,idx)  { }
      inline K_ctor  const_iterator  (const const_iterator& a)  : inherited(a.get_recs(), a.get_idx())  { }
      inline K_ctor  const_iterator  (const iterator& a)        : inherited(a.get_recs(), a.get_idx())  { }
   };

   typedef std::reverse_iterator<iterator>         reverse_iterator;
   typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
public:
//construct/copy/destroy
   explicit
    inline K_ctor ary_base  (const index_maker_type& a = index_maker_type())
                                                   : index_maker_(a),              recs_()         { }
   inline K_ctor  ary_base  (const this_type& a)   : index_maker_(a.index_maker_), recs_(a.recs_)  { }
   inline K_dtor ~ary_base  ()                     { }

   K_mf(const index_maker_type&)  get_index_maker  () const                    { return index_maker_;      }
   K_mf(index_type)               get_index        (const key_type& key) const { return index_maker_(key); }

   K_mf(this_type&)  operator=  (const this_type& a)  { recs_ = a.recs_;   return *this;  }
   K_mf(void)        clear      ()                    { recs_.clear();                    }

//�o��w�q�� insert() ���O stl ���з�,�u���F���ѵ� ev_ary<> �� ev_ary2d<> �ϥ�
   struct ary_insert_aux {
      inline K_mf(void)  operator()  (recs_type* recs,
                                      index_type from, size_type count, const mapped_type* value) const
      {  //����ƻݭn insert ��, �z�L�o�̨ӹ�ڶi�� insert
         recs->insert_from(from, count, (value ? *value : mapped_type()));
      }
      inline K_mf(bool)  make_bf_iterator  (recs_type* recs, index_type from, iterator& i) const
      {  //�p�G�n�[�J�������b�}�C����,�h�Ǧ^false
         if(recs->empty()  ||  from==static_cast<index_type>(recs->size()))
            return false;
         i = iterator(recs, from);
         return true;
      }
      inline K_mf(iterator)  make_af_iterator  (recs_type* recs, index_type from, size_type count) const {
         if(recs->size() == count)
            return iterator(recs, from);
         return iterator(recs, from + recs->get_index_begin());
      }
   };
   friend struct ary_insert_aux;
   template <class insert_aux_type>
    std::pair<iterator,bool>  insert  (const key_type& key, const mapped_type* value,
                                       const insert_aux_type& insert_aux)
   {
      recs_type*  recs = get_recs();
      index_type  idx  = index_maker_(key);
      if(empty()) {
         insert_aux(recs, idx, 1, value);
         return std::make_pair(iterator(recs,idx), true);
      }
      const index_type  ibeg = recs->get_index_begin();
      if(idx < ibeg) {
         insert_aux(recs, 0, ibeg - idx, value);
         return std::make_pair(iterator(recs,idx), true);
      }
      index_type  pos = static_cast<index_type>(idx - ibeg);
#ifdef _MSC_VER//�קK MSC �� warning
      index_type  sz  = static_cast<index_type>(recs->get_index_end() - ibeg);
#else
      index_type  sz  = static_cast<index_type>(recs->size());
#endif
      if(pos < sz)
         return std::make_pair(iterator(recs,idx), false);
      insert_aux(recs, sz, pos-sz+1, value);
      return std::make_pair(iterator(recs,idx), true);
   }
   inline std::pair<iterator,bool>  insert  (const key_type& key, const mapped_type* value)
   {
      return insert(key,value,ary_insert_aux());
   }
//erase one element
   struct ary_erase_aux1 {
      typedef iterator  right_iterator;
      //�z�L�o�ӻ��U���O,�ϥ� erase1() ���H,�N�i�x�� erase1() �o�ͤF����Ʊ�
      inline K_mf(void)  erase_begin  (recs_type* recs) const     { recs->erase_begin(); }
      inline K_mf(void)  erase_back   (recs_type* recs, const orig_iterator& oiback) const
                                                                  { recs->erase_not_begin(oiback); }
      inline K_mf(void)  clear_one    (const iterator& a) const
                                    #ifdef __GNUC__
                                       {                                      *a  = mapped_type(); }
                                    #else
                                       { const_cast<DeConst<mapped_type>::R&>(*a) = mapped_type(); }
                                    #endif
                                         //*a = mapped_type();//�Ycompiler���o�঳��,���ˬdmapped_type�O�_��const
   };
   template <class erase_aux_type>
    K_mf(bool)  erase1  (iterator position, const erase_aux_type& erase_aux)
                        { recs_type*  recs = get_recs();
                          index_type  idx  = static_cast<index_type>(position.get_idx()
                                                                   - recs->get_index_begin());
                          if(idx==static_cast<index_type>(0)) {//����[�Y��]���
                             erase_aux.erase_begin(recs);
                             return true;
                          }
                          if(static_cast<size_type>(idx)==size()-1) {//����[����]���
                             orig_iterator  oiback = recs->end();
                             --oiback;
                             erase_aux.erase_back(recs,oiback);
                             return true;
                          }
                          //���F�̨Τ�,���i����[����]�����
                          //�L�k�N��Ʋ���,�h�N��ƲM��
                          erase_aux.clear_one(position);
                          return false;
                        }
   inline K_mf(bool)  erase  (iterator position)
                        { return erase1(position, ary_erase_aux1()); }

//erase a range elements
   struct ary_erase_aux2 {
      //�z�L�o�ӻ��U���O,�ϥ� erase2() ���H,�N�i�x�� erase2() �o�ͤF����Ʊ�
      inline K_mf(void)  clear          (recs_type* recs) const                    { recs->clear();            }
      inline K_mf(void)  erase_begin    (recs_type* recs, size_type count) const   { recs->erase_begin(count); }
      inline K_mf(void)  erase_not_begin(recs_type* recs, const orig_iterator& first, const orig_iterator& last,
                                         size_type /*count*/) const        { recs->erase_not_begin(first,last); }
      inline K_mf(void)  clear_one      (const iterator& a, const mapped_type& v) const//{ *a  = v; }
                                                #ifdef __GNUC__
                                                   {                                      *a  = v; }
                                                #else
                                                   { const_cast<DeConst<mapped_type>::R&>(*a) = v; }
                                                #endif
   };
   template <class erase_aux_type>
    K_mf(size_type)  erase2  (iterator first, iterator last, const erase_aux_type& erase_aux)
                        { recs_type*  recs = get_recs();
                          iterator    iend = end();
                          if(first==begin()) {
                              if(last==iend) {
                                 //�M�����������
                                 size_type sz = size();
                                 erase_aux.clear(recs);
                                 return sz;
                              }
                              //����[�Y��]�����
                              size_type count = last - first;
                              erase_aux.erase_begin(recs, count);
                              return count;
                          }
                          if(last==iend) {
                              //����[����]�����
                              size_type      count = last - first;
                              orig_iterator  oiend = recs->end();
                              erase_aux.erase_not_begin(recs, oiend-count, oiend, count);
                              return count;
                          }
                          //���F�̨Τ�,���i����[����]�����
                          if(first != last) {
                              mapped_type defaultRec = mapped_type();
                              do {
                                 erase_aux.clear_one(first, defaultRec);
                              } while(++first != last);
                          } // if(first != last...
                          return 0;
                        }
   K_mf(size_type)  erase  (iterator first, iterator last)
      { return erase2(first, last, ary_erase_aux2()); }

//Iterators.
   inline K_mf(iterator)        begin ()       {       recs_type* recs=get_recs(); return iterator      (recs, recs->get_index_begin()); }
   inline K_mf(const_iterator)  begin () const { const recs_type* recs=get_recs(); return const_iterator(recs, recs->get_index_begin()); }
   inline K_mf(iterator)        end   ()       {       recs_type* recs=get_recs(); return iterator      (recs, recs->get_index_end());   }
   inline K_mf(const_iterator)  end   () const { const recs_type* recs=get_recs(); return const_iterator(recs, recs->get_index_end());   }
   inline K_mf(iterator)        back  ()       {       recs_type* recs=get_recs(); return iterator      (recs, recs->get_index_end()-1); }
   inline K_mf(const_iterator)  back  () const { const recs_type* recs=get_recs(); return const_iterator(recs, recs->get_index_end()-1); }

   inline K_mf(reverse_iterator)        rbegin ()       { return reverse_iterator      (end());   }
   inline K_mf(const_reverse_iterator)  rbegin () const { return const_reverse_iterator(end());   }
   inline K_mf(reverse_iterator)        rend   ()       { return reverse_iterator      (begin()); }
   inline K_mf(const_reverse_iterator)  rend   () const { return const_reverse_iterator(begin()); }

//Capacity.
   inline K_mf(size_type)  size     () const  { return recs_.size();  }
   inline K_mf(bool)       empty    () const  { return recs_.empty(); }
#ifdef max
#undef max
#endif
   inline K_mf(size_type)  max_size () const  { return std::numeric_limits<index_type>::max(); }

//ary operations
   inline K_mf(iterator)        find        (const key_type& key)       { return iterator      (get_recs(), find_aux(key));        }
   inline K_mf(const_iterator)  find        (const key_type& key) const { return const_iterator(get_recs(), find_aux(key));        }
   inline K_mf(iterator)        lower_bound (const key_type& key)       { return iterator      (get_recs(), lower_bound_aux(key)); }
   inline K_mf(const_iterator)  lower_bound (const key_type& key) const { return const_iterator(get_recs(), lower_bound_aux(key)); }
 //inline K_mf(iterator)        upper_bound (const key_type& key)       ;
 //inline K_mf(const_iterator)  upper_bound (const key_type& key) const ;

//modifiers
   K_mf(void)  reserve  (const key_type& keyfront, const key_type& keyback)
   { //�� keyback, ����� keyend ����]:
     //  �|�Ҩӻ��a:�Y key_type �� byte, �h key ���d�� 0��255 �Ҧ���, �L�k���@��keyend����!
      index_type        idxbegin = index_maker_(keyfront),
                        idxend   = index_maker_(keyback) + 1;
      const index_type  ibeg     = get_recs()->get_index_begin();
      if(idxbegin > ibeg)
         idxbegin = ibeg;
      recs_->reserve(idxend-idxbegin);
      //�p��N�h�l��[�w�d�Ŷ�]�M��?
   }

   inline K_mf(void)  swap  (this_type& a)    { recs_.swap(a.recs_);
                                                index_maker_.swap(a.index_maker_); }
protected:
   K_mf(index_type)  find_aux  (const key_type& key) const
   {
      const index_type  ibeg = get_recs()->get_index_begin();
      size_type         sz   = size();
      if(sz == 0)
         return ibeg;
      index_type        idx  = index_maker_(key);
      if(idx < ibeg)
         return static_cast<index_type>(ibeg + sz);
      idx -= ibeg;
      if(static_cast<size_type>(idx) > sz)
         return static_cast<index_type>(ibeg + sz);
      return static_cast<index_type>(ibeg + idx);
   } //find_aux()

   K_mf(index_type)  lower_bound_aux  (const key_type& key) const
   {
      size_type   sz = size();
      if(sz == 0)
         return 0;
      index_type        idx  = index_maker_(key);
      const index_type  ibeg = get_recs()->get_index_begin();
      if(idx <= ibeg)
         return(ibeg);
      idx -= ibeg;
      if(static_cast<size_type>(idx) > sz)
         return static_cast<index_type>(ibeg + sz);
      return static_cast<index_type>(ibeg + idx);
   } //lower_bound_aux()

//data members
   index_maker_type           index_maker_;
   wise_container<recs_type>  recs_;

   //���B�Ǧ^�� recs_type* �ä[����(�קKthis�]�h���ӵL��), ���D this �w�g�Q delete
   //��p���Y�Ϩϥ� vector<this_type> ��, �i��|�]��vector<>�Ŷ������ӷh�����
   K_mf(recs_type*)        get_recs  ()        { return recs_.get(); }
   K_mf(const recs_type*)  get_recs  () const  { return recs_.get(); }
};// class ary_base
//---------------------------------------------------------------------------
template <class Key,
          class RecT,
          class IndexMaker = index_maker<Key>,
          class Container  = std::vector<RecT> >
class const_ary_base : public ary_base<Key,RecT,IndexMaker,Container, const RecT>
//���F�T�O�ϥ� updater ��s���, �ҥH�����ϥ� const_pointer, const_reference
{
   typedef const_ary_base<Key,RecT,IndexMaker,Container>       this_type;
   typedef ary_base<Key,RecT,IndexMaker,Container,const RecT>  inherited;
public:
   explicit
    inline K_ctor const_ary_base  (const IndexMaker& a = IndexMaker())  : inherited(a) { }
   inline K_ctor  const_ary_base  (const this_type& a)                  : inherited(a) { }
};
/////////////////////////////////////////////////////////////////////////////
template <class evt, class recs_type, class mapped_type>
void  evary_insert_aux  (evt*                            evary,
                         recs_type*                      recs,
                         typename recs_type::index_type  from,
                         typename recs_type::size_type   count,
                         const mapped_type*              value)
{
   if(count<=1  ||  value==0)
      evary->insert_auxev(recs, from, count, (value ? *value : mapped_type()));
   else
   if(from==static_cast<typename recs_type::index_type>(0)) {
      evary->insert_auxev(recs, 0, count-1, mapped_type());  //��J[�D�w����]���
      evary->insert_auxev(recs, 0, 1,       *value);         //��J[�n�w����]���
   } else {
      evary->insert_auxev(recs, from,   count-1, mapped_type());//��J[�D�w����]���
      evary->insert_auxev(recs, static_cast<typename recs_type::index_type>(from+count-1), 1, *value);//��J[�n�w����]���
   }
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#pragma warning(default:4267 4244)
#endif
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

