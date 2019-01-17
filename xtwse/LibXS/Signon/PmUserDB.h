//
// Policy Manager - �ϥΪ̰򥻸�Ʈw
//
// ����: $Id: PmUserDB.h,v 1.6 2006/04/04 04:03:59 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmUserDBH
#define PmUserDBH
//---------------------------------------------------------------------------
#include "PmBase.h"
#include "KTime.h"
#include "UserPassPolicy.h"
#include <set>
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
//typedef Kstl::char_ary<char,12,0>      TUserID;  ���F�׶}BCB4��bug! �аѦ� PmBase.h ������!
class TUserID : public TPolicyKey0 {
   typedef TPolicyKey0   inherited;
public:
   inline K_ctor  TUserID  (const std::string& str)  : inherited() { assign(str); }
   inline K_ctor  TUserID  (const char*        str)  : inherited() { assign(str); }
   inline K_ctor  TUserID  ()                        : inherited() {}
};
//---------------------------------------------------------------------------
struct TPassRec;
struct TUserBase
{
   typedef Kstl::char_ary<char,12,0>   TUPass;
   typedef Kstl::char_ary<char,12,0>   TUName;
   typedef Kstl::char_ary<char,20,0>   TUFrom;
   typedef Kstl::char_ary<char,20,0>   TUPassH;//sizeof(TUPassH)+sizeof(TUPass) = 32

   TUPass            PassCode1_;
   TUName            Name_;
   TPmGroupID        PmGroupID_;
   mutable byte      PassMagicNum_; //�[�K/�ѱK��.

   TKDate            StartDate_;
   TKDate            EndDate_;

   TKDate            PassEndDate_;
   union {
         byte        Flags_;
      struct {
         byte        MustChgPass_ : 1,
                     UserLock_    : 1,
                     AllowFromSameIP_ : 1,//�p�G��true,�h�ݰѦ�PassPolicy��:MaxCountFromSameIP_
                     AllowFromDiffIP_ : 1;//�Y���]�wPolicy,�h�w�]��0.
      };
   };

   TUFrom            LastFrom_;
   TKDateTime        LastTime_;

   fixed_num<byte>   PassErrCount_;
   TUFrom            PassErrFrom_;
   TKDateTime        PassErrTime_;
   TUPassH           PassCode2_;

   inline K_ctor  TUserBase  ()
      : Flags_(0)
      , PassMagicNum_(2)
      { MustChgPass_ = true; }

   K_mf(void) EncodePass (const std::string& pass, TPassRec&, std::string* npass) const;
   K_mf(bool) IsPass     (const std::string& pass) const;
};
struct TPassRec
{
   TUserBase::TUPass    PassCode1_;
   TUserBase::TUPassH   PassCode2_;

   inline K_mf(bool) operator==(const TUserBase& u) const {
      return PassCode1_==u.PassCode1_  &&  PassCode2_==u.PassCode2_;
   }
   inline K_mf(bool) operator==(const TPassRec& u) const {
      return PassCode1_==u.PassCode1_  &&  PassCode2_==u.PassCode2_;
   }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TPmUserDB : public TVACreater
{
   typedef TVACreater   inherited;
public:
   K_ctor  TPmUserDB  (const std::string& defaultPath);
   K_dtor ~TPmUserDB  ();

   K_mf(const TUserBase*)  GetUser  (const std::string& user) const;
   K_mf(std::string)       ChkUser  (const std::string& user, const std::string& pass, const std::string& ufrom
                                    ,size_t ufCount, size_t dufCount) const;
   /// �K�X�䴩���ؽs�X�覡, �ϥΫ��w�覡�s�X��: pass[0]=XorCode, pass[1]=�s�X�覡, �s�X��K�X��b pass[2..n]
   K_mf(Kway::byte) GetPassEncMode ();

   K_mf(const TUserBase*)  AddUser     ( const std::string& user, const TUserBase&);
   K_mf(std::string)       ChgPass     ( const std::string& user, const std::string& oldPass, const std::string& newPass
                                       , const std::string& ufrom);
   K_mf(bool)              ChgPolicyID ( const std::string& user, const std::string& policyGroupID);
   K_mf(TPolicyBaseRef)    GetPolicy   ( const std::string& user, const std::string& policyName, bool forLocalUse) const;
   K_mf(TPolicyBaseRef)    GetPolicy   ( const TUserBase* userRec,const std::string& policyName, bool forLocalUse) const;
   K_mf(bool)              GetUsersList(std::set<std::string>& users);

   K_mf(bool) AddDelOnUpdate ( const std::string& user, const std::string& policyName, TOnPmUpdate&
                             , bool (K_FASTCALL TPmBase::*addDel) (const TPolicyKey&, TOnPmUpdate&) );

          K_mf(void)       SetPm      (TPmGroup& pmGrp, TPmMgr& pmMgr);
   inline K_mf(TPmGroup*)  GetPmGroup () const       { return PmGroup_; }
   inline K_mf(TPmMgr*)    GetPmMgr   () const       { return PmMgr_;   }

   #ifdef __TransLnDB__ //�p�G�ݭn�q KPTS��LnDB ��J�n�J��Ʈw��,�~�ݭn�}�񩳤U�� members, ���z�����ۤv�����L
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif

   class TImpl;
private:

   TImpl*      Impl_;
   TPmGroup*   PmGroup_;
   TPmMgr*     PmMgr_;

   #ifndef __TransLnDB__ //�p�G�ݭn�q KPTS��LnDB ��J�n�J��Ʈw��,�~�ݭn�}�񩳤U�� members, ���z�����ۤv�����L
   TUserPPMgr  PassPolicyMgr_;
   K_mf(TVACreater*) GetDetailVACreater (size_t);
   K_mf(size_t)      GetDetails         (const char**& details);
   #endif

   //�~�Ӧ� TVACreater, �� ViewAreaMgr �޲z�Ϊ� members
   K_mf(TaViewArea)  CreateNewVA        ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

