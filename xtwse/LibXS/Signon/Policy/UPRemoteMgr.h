//
// �ϥΪ�,���ݺ޲z�v���F��.
//    class TUPRemoteMgr;
//
// ����: $Id: UPRemoteMgr.h,v 1.2 2005/04/26 05:57:37 fonwin Exp $
//
// Note:��Policy������Client�ݪ��d��(�]���]�t�F�@��ev_map<>).
//---------------------------------------------------------------------------
#ifndef UPRemoteMgrH
#define UPRemoteMgrH
//---------------------------------------------------------------------------
//
// �F���W�� = TUPRemoteMgr::PolicyName
// �F�����e = TUPRemoteMgr
//
#include "Signon/Policy/PolicyBase.h"
#include "ev_map.h"
#include "char_ary.h"
#include "inn_file.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::rr_inn_file< ::Kway::dword, ::Kway::word>   TURM_Inn;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
LIBNS_struct TUserRemoteMgrPolicyBase
{
   union {
      byte     Flags_;
      struct {
         byte  AllowVAC_  : 1,
               ReadOnly_  : 1,   //��AllTables_==true��,�h�ѦҦ����]�w�O�_�i�ק�.
               AllTables_ : 1;   //���\��������ƪ�,�Y��true,�h���A�Ѧҩ��Ӫ��]�w.
      };
   };
   inline K_ctor  TUserRemoteMgrPolicyBase  ()
      : Flags_(0)
      { }
};
//---------------------------------------------------------------------------
#ifdef _MSC_VER//K_EnumPacked_Bug ���Ө�VC�ɤ~�o�{�S�[,���F�ɮׯ�ۮe,�ҥH��P�_ _MSC_VER
   typedef byte   ETableRight;
   enum ETableRight_t
#else
   enum ETableRight
#endif
{
   etr_ReadOnly        = 0,   //�����.
   etr_WriteSettingAll = 1,   //�i�ק� + �i�]�w�M��(���ݼf��),�i�f�֥L�H.
   etr_Disabled        = 2,   //�L�v��.
   etr_WriteNoSetting  = 3,   //�i�ק� + �]�w�M�ή�,�ݭn���v���̱j��.
   etr_WriteSetting1   = 4,   //�i�ק� + �i�]�w�M��,�ݥL�H�f��,�i�f�֥L�H.
/*
   etr_Disabled        = 0,   //�L�v��.
   etr_ReadOnly        = 1,   //�����.
   etr_WriteNoSetting  = 2,   //�i�ק� + �]�w�M�ή�,�ݭn���v���̱j��.
   etr_WriteSetting1   = 3,   //�i�ק� + �i�]�w�M��,�ݥL�H�f��,�i�f�֥L�H.
   etr_WriteSettingAll = 4,   //�i�ק� + �i�]�w�M��(���ݼf��),�i�f�֥L�H.
*/
};
//---------------------------------------------------------------------------
struct TTableRight
{
   ETableRight    Right_;

   inline K_ctor  TTableRight  (ETableRight r = etr_Disabled)
      : Right_(r)
      , RoomKey_()
      { }

   //20090213:�קKVA�ƻs�y���s�ɿ��~
   inline K_mf(void) operator= (const TTableRight& r) {
      Right_ = r.Right_;
   }

   TURM_Inn::rr_key  mutable RoomKey_;

#ifdef _MSC_VER
   K_mf(bool) operator==(const TTableRight& r) const {
	   return Right_ == r.Right_;
   }
#endif
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TTableRights : public Kstl::ev_map<std::string,TTableRight>
{
   typedef Kstl::ev_map<std::string,TTableRight>   inherited;
public:
   typedef Kstl::char_ary<char,12,' '>    TMasterKey;

#ifdef _MSC_VER
   inline K_ctor  TTableRights ()  {}
#endif

   inline explicit K_ctor  TTableRights  (const TMasterKey& mkey)
      : MasterKey_(mkey)
      { }

   inline K_mf(const TMasterKey&)  GetMasterKey    () const { return MasterKey_; }
   inline K_mf(std::string)        GetMasterKeyStr () const { return MasterKey_.as_string(); }

   inline K_mf(void) operator= (const TTableRights& r) {
      inherited::operator=(r);
   }

private:
   TMasterKey  MasterKey_;
};
//---------------------------------------------------------------------------
LIBNS_struct TUserRemoteMgrPolicy : public TUserRemoteMgrPolicyBase
{
#ifdef _MSC_VER
   inline K_ctor TUserRemoteMgrPolicy() {}
#endif

   inline K_ctor  TUserRemoteMgrPolicy  (const TTableRights::TMasterKey& mkey)
      : RoomKey_()
      , TableRights_(mkey)
      { }

   inline K_mf(TUserRemoteMgrPolicy&) operator= (const TUserRemoteMgrPolicyBase& r)
            { static_cast<TUserRemoteMgrPolicyBase&>(*this) = r;
              return *this; }

   TURM_Inn::rr_key  mutable RoomKey_;
   TTableRights      mutable TableRights_;

   inline void operator=(const TUserRemoteMgrPolicy& r) {
      //�קKViewArea����[�ƻs]�ާ@,�ҳy�������D.
      static_cast<TUserRemoteMgrPolicyBase&>(*this) = static_cast<const TUserRemoteMgrPolicyBase&>(r);
      TableRights_ = r.TableRights_;
   }

   //�t�X TTExtabUpdater2d<> ���s�W:
   inline K_mf(TTableRight&)  operator[] (const std::string& dkey)    { return TableRights_[dkey]; }
   inline K_mf(size_t)        erase      (const std::string& dkey)    { return TableRights_.erase(dkey); }
   inline                     operator TTableRights& ()               { return TableRights_; }

   K_mf(ETableRight) CheckTableRight (std::string tabName) const;
};
//---------------------------------------------------------------------------
LIBNS_class TUPRemoteMgr : public TTRecPolicy<TUserRemoteMgrPolicy>
{
   typedef TTRecPolicy<TUserRemoteMgrPolicy>   inherited;
public:
   inline K_ctor      TUPRemoteMgr  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif

