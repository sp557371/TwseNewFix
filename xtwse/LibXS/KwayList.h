//---------------------------------------------------------------------------
/* 為何要自己寫一個 List<>?
 * 因為經過仔細思考 std::list<> 實在無法符合需求,
 * 我要的是:
      class MyType : public TNode {
        可在此: 直接將 MyType 從 List 中移除, 或加入另一個 List.
        例如:
             在不知道 MyType 隸屬於哪個 list 的情況下,
             呼叫 Delink() 即可將 this 從 list 之中移除,
             或可直接 delete node;
      };
 * std::list<> 顯然無法達到要求: stl 沒有定義 list<>::node_type
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
	  //在VC會有警告,所以改一下 : Prev_(this), Next_(this)  { }//for THeadNode
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

  // member functions (命名盡量與 std::list<> 相同)
  K_mf(bool)   empty      () const   { return Head_.empty();                       }
  K_mf(Node*)  front      ()         { return static_cast<Node*>(Head_.GetNext()); }
  K_mf(Node*)  back       ()         { return static_cast<Node*>(Head_.GetPrev()); }
  K_mf(void)   push_front (Node* n)  { front()->insert_front(*n);                  }
  K_mf(void)   push_back  (Node* n)  { back()->insert_back(*n);                    }
  K_mf(void)   pop_front  ()         { if(!empty()) Head_.GetNext()->Delink();     }
  K_mf(void)   pop_back   ()         { if(!empty()) Head_.GetPrev()->Delink();     }
  K_mf(Node*)  begin      ()         { return front();                             }
  K_mf(TNodeBase*)  end   ()         { return &Head_;                              }

  //自行擴充的 member functions
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

