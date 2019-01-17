//
// Policy Manager Base
//
// ����: $Id: PmBase.h,v 1.5 2006/04/04 04:05:59 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmBaseH
#define PmBaseH
//---------------------------------------------------------------------------
#include "libnsSignonSvrDB.h"
#include "Signon/Policy/PolicyBase.h"
#include "ViewAreaMgr.h"
#include "char_ary.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
//---------------------------------------------------------------------------
using Kway::Signon::Policy::TPolicyBaseRef;
using Kway::Signon::Policy::TPolicyKey;
using Kway::Signon::Policy::TPmBase;
using Kway::Signon::Policy::TOnPmUpdate;

SIGDB_class TSignonSvr_DataCenter;
/////////////////////////////////////////////////////////////////////////////
//typedef Kstl::char_ary<char,12,0>   TPolicyName;
//typedef Kstl::char_ary<char,12,0>   TPolicyKey;
// 920623:�쥻���ӥΤW�z���w�q�覡�N�i�H�F,
// �i�OBCB4(BCB5�S�չL),�����b���঳bug! �Y�ϥΤW�z���w�q�覡,�|�y�� std::map<> �������Ȧ���!
// �ҥH����~�Ӫ��覡�w�q:
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
class TPolicyName : public Kstl::char_ary<char,20,0> {
   typedef Kstl::char_ary<char,20,0>   inherited;
public:
   inline K_ctor  TPolicyName (const std::string& str)  : inherited(str) {}
   inline K_ctor  TPolicyName (const char* str)         : inherited(str) {}
   inline K_ctor  TPolicyName ()                        : inherited()    {}
};
#ifdef __BORLANDC__  //�u�� BCB �~����O�b [�e�m�B�z] ��, ���D��ƪ��j�p!
   #if sizeof(TPolicyName) != 20
   #  error "��Ʀ��w�q�覡���~..."
   #endif
#endif
//---------------------------------------------------------------------------
class TPolicyKey0 : public Kstl::char_ary<char,12,0> {
   typedef Kstl::char_ary<char,12,0>   inherited;
public:
   inline K_ctor  TPolicyKey0  (const std::string& str)  : inherited() { assign(str); }
   inline K_ctor  TPolicyKey0  (const char* str)         : inherited() { assign(str); }
   inline K_ctor  TPolicyKey0  ()                        : inherited() {}

   inline K_mf(void)  assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void)  assign  (const char* str)         { char buf[sizeof(*this)+1];
                                                          inherited::assign(TPolicyKey::assign(buf,str)); }
};
#ifdef __BORLANDC__  //�u�� BCB �~����O�b [�e�m�B�z] ��, ���D��ƪ��j�p!
   #if sizeof(TPolicyKey) != 12
   #  error "��Ʀ��w�q�覡���~..."
   #endif
#endif
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TPmBaseVA : public TPmBase, public TVACreater
{
public:
   virtual K_mf(TPmBaseVA*) CreatePmBaseVA(TSignonSvr_DataCenter& owner, TSignonSvr_DataCenter* dcParentLimited) {
      return NULL;
   }
};
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TPmBase_GenUpdEv : public TPmBaseVA
{
protected:
   K_ctor  TPmBase_GenUpdEv (Kstl::ev_container&);
   K_dtor ~TPmBase_GenUpdEv ();

   template <class TRecs>
   K_mf(bool) GetPolicyKeys (const TRecs& recs, const ev_iterator& istart, size_t count, TPolicyKey& keyBegin, TPolicyKey& keyEnd) const
      {  typename TRecs::const_iterator   itr = static_cast<const typename TRecs::iterator&>(istart);
         keyBegin = itr->first;
         typename TRecs::const_iterator   iend = recs.end();
         while(count-- > 0)
            if(++itr==iend) {
               --itr;
               keyEnd = itr->first;
               keyEnd.set(0, static_cast<char>(keyEnd[0]+1));
               return true;
            }
         keyEnd = itr->first;
         return true;
      }

public:
   virtual K_mf(bool) AddOnUpdate (const TPolicyKey&, TOnPmUpdate&);
   virtual K_mf(bool) DelOnUpdate (const TPolicyKey&, TOnPmUpdate&);

   virtual K_mf(const char*) GetPolicyName () const = 0;
   /// ���o keyBegin = istart.key, keyEnd = (istart+count).key
   /// �@��[���ʳq��]���d��: �Y���e���I�s AddOnUpdate(key,updater) �h: if(keyBegin<=key && key < keyEnd) �N�|���즹�����q��
   virtual K_mf(bool) GetPolicyKeys (const ev_iterator& istart, size_t count, TPolicyKey& keyBegin, TPolicyKey& keyEnd) const = 0;

private:
   class TImpl;
   TImpl* Impl_;
};
//---------------------------------------------------------------------------
SIGDB_class TPmMgr : protected TViewAreaMgr
{  //�@��[�F����]���޲z
   //  �޲z [PolicyName---TPmBase] �����X
   typedef TViewAreaMgr inherited;

   TViewAreaMgr::TVAGetEditProp_GetNames  VAEditProp_GetNames_;
   TViewAreaMgr::TVAGetEditProp_GetKeys   VAEditProp_GetKeys_;
public:
   K_ctor  TPmMgr  ();

   inline K_mf(bool)  Add  (const std::string& policyName, TPmBaseVA& pm)  { return inherited::Add(policyName, pm); }
   inline K_mf(bool)  Del  (const std::string& policyName, TPmBaseVA& pm)  { return inherited::Add(policyName, pm); }
   inline K_mf(bool)  Del  (TPmBaseVA& pm)                                 { return inherited::Del(pm); }

   inline K_mf(bool) GetRegNames (Kway::TStrings& result) const {
      return inherited::GetRegNames(result);
   }

// inline K_mf(TPmBase*)         GetPolicy       (const std::string& policyName) const   { return static_cast<TPmBaseVA*>(GetReg(policyName)); }
// gcc�����w�ڧ�o�˪��ഫ��b�o��� ���O�ഫ�|�����D: GetReg()=> TVACreater* => TPmBaseVA* => TPmBase*
   K_mf(TPmBase*) GetPolicy (const std::string& policyName) const;

   inline K_mf(TViewAreaMgr&)    GetViewAreaMgr  ()                                      { return *this; }
   inline K_mf(TVAGetEditProp&)  GetEditProp_PolicyNames  ()   { return VAEditProp_GetNames_; }
   inline K_mf(TVAGetEditProp&)  GetEditProp_PolicyKeys   ()   { return VAEditProp_GetKeys_;  }
};
//---------------------------------------------------------------------------
SIGDB_class TPmGroup : public TVACreater
{  //�F���s�޲z
   // �ϥ� TPmGroupID ��@����, ���o�@�� [PolicyName--PolicyKey] �����X
   class TImpl;
   TImpl* Impl_;
   //------------------------------------------------------------------------
   //�� ViewAreaMgr �޲z�Ϊ� members
   K_mf(TaViewArea)  CreateNewVA           ();
   K_mf(size_t)      GetDetails            (const char**&);
   K_mf(TVACreater*) GetDetailVACreater    (size_t);
   K_mf(TVACreater*) OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);

public:
   class TPmGroupID : public TPolicyKey0 {
      typedef TPolicyKey0  inherited;
   public:
      inline K_ctor  TPmGroupID  (const std::string& str)  : inherited(str) {}
      inline K_ctor  TPmGroupID  (const char* str)         : inherited(str) {}
      inline K_ctor  TPmGroupID  ()                        : inherited()    {}
   };

   //----- �غc/�Ѻc
   K_ctor  TPmGroup  (const std::string& fileName, TPmMgr*,
                      const std::string& mGroupName, const std::string& mTableName,
                      const std::string& dGroupName, const std::string& dTableName);
   K_dtor ~TPmGroup  ();

   //----- �d�߬Y Policy ���]�w
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPmMgr&, const TPmGroupID&, const std::string& policyName, bool forLocalUse) const;

   //----- Policy���ʳq��.
   K_mf(bool) AddDelOnUpdate ( const TPmMgr&, const TPmGroupID&, const std::string& policyName, TOnPmUpdate&
                             , bool (K_FASTCALL TPmBase::*addDel) (const TPolicyKey&, TOnPmUpdate&) );

   //----- �ק�Y[PolicyGroup]-[PolicyName]-PolicyKey(�Y��Ƥ��s�b�h�s�W)
   K_mf(bool)  ModPolicy  (const TPmGroupID&, const std::string& policyName, const std::string& policyKey);

   //----- �s�W�Y[PolicyGroup]-[PolicyName]-PolicyKey(�Y��Ƥw�s�b�h����)
   K_mf(const TPolicyKey*)  AddPolicy  (const TPmGroupID&, const std::string& policyName, const std::string& policyKey);

   //���X������ Keys ��@��J�ɪ� [�C�|]
   K_mf(TVAGetEditProp&)  GetVAEditProp_ForKeys  ();

   #ifdef __TransLnDB__ //�p�G�ݭn�q KPTS��LnDB ��J�n�J��Ʈw��,�~�ݭn�}�񩳤U�� members, ���z�����ۤv�����L
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif
};
typedef TPmGroup::TPmGroupID  TPmGroupID;
/////////////////////////////////////////////////////////////////////////////
} // Signon
} // Kway
//---------------------------------------------------------------------------
#endif

