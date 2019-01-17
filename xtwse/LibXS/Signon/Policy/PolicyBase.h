//
// ñ�J�޲z - [ñ�J�F��]��¦ classes
//
// ����: $Id: PolicyBase.h,v 1.4 2006/04/04 04:02:11 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PolicyBaseH
#define PolicyBaseH
//---------------------------------------------------------------------------
#include <utility>
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "fixed_num.h"
#include "char_ary.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
/////////////////////////////////////////////////////////////////////////////
//
// �ϥΪ̵n�J�޲z,�����O�w�q
// �b Signon/Policy/* ���ؿ��U�s�񪺬O [�F�����W��] �� �F����[���e]
// �� Signon/Policy/UFGroups.h
//
//---------------------------------------------------------------------------
typedef std::pair<const void*, word>   TPolicyBaseRef;
typedef Kstl::fixed_num<dword>         TOnlineID;
/////////////////////////////////////////////////////////////////////////////
template <class T>
class TTRecsPolicy
{
   const T*    Recs_;
   word        Count_;
public:
   typedef const T*  iterator;

   inline K_ctor  TTRecsPolicy  (const TPolicyBaseRef& ref)
            : Recs_(static_cast<const T*>(ref.first)),
              Count_(static_cast<word>(ref.second/sizeof(T)))
            { }

   inline K_mf(iterator)  begin  () const  { return Recs_; }
   inline K_mf(iterator)  end    () const  { return Recs_ + Count_; }
   inline K_mf(bool)      empty  () const  { return Count_==0; }
   inline K_mf(word)      size   () const  { return Count_; }
};
//---------------------------------------------------------------------------
template <class T>
class TTRecPolicy
{
   const T* Rec_;
public:
   typedef T   TRecType;

   inline K_ctor  TTRecPolicy  (const TPolicyBaseRef& ref)
            : Rec_(ref.second==sizeof(T) ? static_cast<const T*>(ref.first) : 0)
            { }

   inline K_mf(bool)      empty       ()     const  { return Rec_==0; }
   inline K_mf(const T*)  operator->  ()     const  { return Rec_;    }
   inline K_mf(const T&)  operator*   ()     const  { return *Rec_;   }
   inline K_mf(const T*)  get         ()     const  { return Rec_;    }
   inline K_mf(bool)      assign_to   (T& a) const  { if(Rec_==0)  { a = T(); return false; }
                                                      a = *Rec_;   return true; }
};
//---------------------------------------------------------------------------
template <class TRecs, class TRec>
K_mf(TPolicyBaseRef)  MakeRecsPolicy  (const TRecs& recs, std::string& result, const TRec*)
{
   result.clear();
   typename TRecs::const_iterator  iend = recs.end();
   for(typename TRecs::const_iterator i = recs.begin();  i != iend;  ++i) {
      const char* elem = reinterpret_cast<const char*>(&(i->first));
      result.insert(result.end(), elem, elem+sizeof(typename TRecs::key_type));
      elem = reinterpret_cast<const char*>(static_cast<const TRec*>(&(i->second)));
      result.insert(result.end(), elem, elem+sizeof(TRec));
   }
   return TPolicyBaseRef(result.data(), static_cast<TPolicyBaseRef::second_type>(result.length()));
}
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TPmBase;
//---------------------------------------------------------------------------
//�Цb�ϥήɤ~���~�Ӫ��覡,�Ҧp: struct TPolicyKey_UserRights : public TPolicyKey {};
//�H�׶} BCB �� bug!
LIBNS_class TPolicyKey : public Kstl::char_ary<char,12,' '>
{
   typedef Kstl::char_ary<char,12,' '>   inherited;
public:
   inline K_ctor  TPolicyKey  (const std::string& str)  : inherited() { assign(str); }
   inline K_ctor  TPolicyKey  (const char* str)         : inherited() { assign(str); }
   inline K_ctor  TPolicyKey  ()                        : inherited() {}

   inline K_mf(void)  assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void)  assign  (const char* str)         { char buf[sizeof(*this)+1];
                                                          inherited::assign(assign(buf,str)); }

   static K_mf(const char*)  assign  (char* buf, const char* str);
};
//---------------------------------------------------------------------------
LIBNS_class TOnPmUpdate
{
protected:
   /** �Ѻc��b protected, ��ܱz�����ۦ�R��������, TPmBase_GenUpdEv���|��L�R��.
   */
   virtual K_dtor ~TOnPmUpdate  ()   {}
public:
   inline  K_ctor  TOnPmUpdate  ()   {}

   virtual K_mf(void) OnPmUpdate (TPmBase&, const TPolicyKey&, bool erased) = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TPmBase
{  //�򥻬F���]�w��ƪ�
   //��:TPmUserTwStkRights  [�ϥ��v]�F����   "PmUserTwStkRights.h"
   //   TPmUserTwStkScItems [��������]�F���� "PmUserTwStkScItems.h"
        TPmBase  (const TPmBase&);//�T��ƻs.
   void operator=(const TPmBase&);//�T��ƻs.
public:
   inline  K_ctor  TPmBase ()  {}
   virtual K_dtor ~TPmBase ()  {}

   virtual K_mf(TPolicyBaseRef) GetPolicy  (const TPolicyKey&, bool forLocalUse) const = 0;

   /** ���Ѳ��ʳq�����U����:
       �w��Y TPolicyKey ���U���ʳq��.
   */
   virtual K_mf(bool)        AddOnUpdate   (const TPolicyKey&, TOnPmUpdate&)  { return false; }
   virtual K_mf(bool)        DelOnUpdate   (const TPolicyKey&, TOnPmUpdate&)  { return false; }
   virtual K_mf(const char*) GetPolicyName () const                           { return "";    }

protected:
   template <class TRecs,
             class TRec>//�ϥ� SimpleRecorder ����, TRecs::mapped_type �w���O���檺���A, �ҥH�ݭn�[�W���Ѽ�
                        //�аѦҨϥνd�� UserTwStkRes.h
   static K_mf(TPolicyBaseRef)  GetPolicy_TRec  (const TRecs& recs, const typename TRecs::key_type& key, const TRec*)
      {
         typename TRecs::const_iterator  i = recs.find(key);
         if(i==recs.end())
            return TPolicyBaseRef(0,0);
         return TPolicyBaseRef(static_cast<const TRec*>(&(i->second)), sizeof(TRec));
      }
   template <class TRecs>
   static K_mf(TPolicyBaseRef)  GetPolicy_TMap  (const TRecs& recs, const typename TRecs::key_type& key)
      {
         typename TRecs::const_iterator  i = recs.find(key);
         if(i==recs.end())
            return TPolicyBaseRef(0,0);
         return i->second.GetPolicy();
      }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

