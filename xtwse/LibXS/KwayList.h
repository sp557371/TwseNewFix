//---------------------------------------------------------------------------
/* ����n�ۤv�g�@�� List<>?
 * �]���g�L�J�ӫ�� std::list<> ��b�L�k�ŦX�ݨD,
 * �ڭn���O:
      class MyType : public TNode {
        �i�b��: �����N MyType �q List ������, �Υ[�J�t�@�� List.
        �Ҧp:
             �b�����D MyType ���ݩ���� list �����p�U,
             �I�s Delink() �Y�i�N this �q list ��������,
             �Υi���� delete node;
      };
 * std::list<> ��M�L�k�F��n�D: stl �S���w�q list<>::node_type
 */
// $Id: KwayList.h,v 1.3 2006/04/04 02:59:52 fonwin Exp $
//---------------------------------------------------------------------------
#ifndef KwayListH
#define KwayListH
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TNodeBase
{
  typedef TNodeBase  this_type;
  K_ctor TNodeBase (const TNodeBase&);
  void   operator= (const TNodeBase&);
public:
  inline  K_ctor  TNodeBase ()   : Prev_(0), Next_(0)  { }
  virtual K_dtor ~TNodeBase ()   { RemoveImpl(); }

  inline K_mf(void)  Delink  ()   { RemoveImpl();
                                    Prev_ = Next_ = 0; }

  K_mf(void)  insert_back  (TNodeBase& a)  { if(this==&a) return;
                                             a.RemoveImpl();
                                             if(Next_) Next_->Prev_ = &a;
                                             a.Next_ = Next_;
                                             Next_   = &a;
                                             a.Prev_ = this; }
  K_mf(void)  insert_front (TNodeBase& a)  { if(this==&a) return;
                                             a.RemoveImpl();
                                             if(Prev_) Prev_->Next_ = &a;
                                             a.Prev_ = Prev_;
                                             Prev_   = &a;
                                             a.Next_ = this; }
  inline K_mf(this_type*) GetPrev () const  { return Prev_; }
  inline K_mf(this_type*) GetNext () const  { return Next_; }

protected:
  inline K_ctor TNodeBase (bool)
	  //�bVC�|��ĵ�i,�ҥH��@�U : Prev_(this), Next_(this)  { }//for THeadNode
      { Prev_ = Next_ = this; }

  inline K_mf(void)  RemoveImpl  ()  { if(Prev_)  Prev_->Next_ = Next_;
                                       if(Next_)  Next_->Prev_ = Prev_; }
private:
  this_type*  Prev_;
  this_type*  Next_;
};
//---------------------------------------------------------------------------
template <class Node>
class TList
{
  typedef TList<Node>   this_type;
public:
  typedef Node          node_type;

  // ctor / dtor
  K_dtor  ~TList()  { while(!empty())  delete Head_.GetNext(); }

  // member functions (�R�W�ɶq�P std::list<> �ۦP)
  K_mf(bool)   empty      () const   { return Head_.empty();                       }
  K_mf(Node*)  front      ()         { return static_cast<Node*>(Head_.GetNext()); }
  K_mf(Node*)  back       ()         { return static_cast<Node*>(Head_.GetPrev()); }
  K_mf(void)   push_front (Node* n)  { front()->insert_front(*n);                  }
  K_mf(void)   push_back  (Node* n)  { back()->insert_back(*n);                    }
  K_mf(void)   pop_front  ()         { if(!empty()) Head_.GetNext()->Delink();     }
  K_mf(void)   pop_back   ()         { if(!empty()) Head_.GetPrev()->Delink();     }
  K_mf(Node*)  begin      ()         { return front();                             }
  K_mf(TNodeBase*)  end   ()         { return &Head_;                              }

  //�ۦ��X�R�� member functions
  K_mf(bool)  empty_or_1  () const   { return Head_.GetNext()==Head_.GetPrev();    }

private:
  struct THeadNode : public TNodeBase {
    inline K_ctor  THeadNode ()  : TNodeBase(true) {}
    inline K_mf(bool)  empty () const  { return GetNext()==static_cast<const TNodeBase*>(this); }
  } Head_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

