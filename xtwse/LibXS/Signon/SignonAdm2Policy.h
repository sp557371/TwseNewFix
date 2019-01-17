#include "Signon/Policy/PolicyBase.h"
#include "Signon/PmBase.h"
#include "inn_file.h"
#include "ev_map.h"

namespace Kway { namespace Signon { namespace Admin2 {

using namespace Kway::Signon::Policy;

/////////////////////////////////////////////////////////////////////////////
typedef Kstl::rr_inn_file<Kway::dword, Kway::word>   TAdmin2Inn;
//===========================================================================
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
/** [Admin2]�ϥ��v��.
*/
struct TAdmin2Rights
{
   //�ϥΪ̵n�J�e�mID
   TPolicyKey  PrevUserID_;

   //��2�hUser��Ʈw�x�s��m: "�D��Ʈw���|/adm2/Admin2Path_/*"
   //(�ťժ�ܵL�v��)
   //���F�קK����,�����P key �ۦP,���i�t�~�]�w!
   //TPolicyKey  Admin2SubPath_;

   inline K_ctor TAdmin2Rights ()
      : PrevUserID_()
    //, Admin2SubPath_()
      { }
};
//===========================================================================
#ifdef K_EnumPacked_Bug
   typedef byte   EPolicyRight;
   enum EPolicyRight_t
#else
   enum EPolicyRight
#endif
{
   apr_Deny,        //�L�v��
   apr_SameAsAdmin2,//�PAdmin2���v���ۦP
   apr_SetByAdmin2, //��Admin2�]�w

   apr_Count
} K_EnumPacked;
//---------------------------------------------------------------------------
/** [��2�hUser��policy]�v��.
*/
struct TPolicyRight
{
   EPolicyRight   Right_;

   inline K_ctor  TPolicyRight  ()
      : Right_(apr_SetByAdmin2)
      , RoomKey_()
      { }

   TAdmin2Inn::rr_key  mutable RoomKey_;

   K_ctor TPolicyRight (const TPolicyRight& r) {
      operator=(r);
   }
   K_mf(void) operator= (const TPolicyRight& r) {
      Right_ = r.Right_;
   }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
/** [��2�hUser��policy]�v����.
*/
class TPolicyRights : public Kstl::ev_map<TPolicyName, TPolicyRight>
{
public:
   typedef TPolicyKey   TMasterKey;

   inline explicit K_ctor TPolicyRights (const TMasterKey& mkey)
      : MasterKey_(mkey)
      { }

   inline K_mf(const TMasterKey&)  GetMasterKey    () const { return MasterKey_; }
   inline K_mf(std::string)        GetMasterKeyStr () const { return MasterKey_.as_string(); }

private:
   TPolicyKey   MasterKey_;
};
//---------------------------------------------------------------------------
/** [Admin2]�ϥ��v��.�F��
*/
class TAdmin2Policy : public TAdmin2Rights
{
public:
   typedef TAdmin2Rights   TBase;//�n�x�s��type.

   inline K_ctor  TAdmin2Policy  (const TPolicyRights::TMasterKey& mkey = TPolicyRights::TMasterKey())
      : RoomKey_()
      , PolicyRights_(mkey)
      { }

   inline K_mf(TAdmin2Policy&) operator= (const TAdmin2Rights& r) {
      static_cast<TAdmin2Rights&>(*this) = r;
      return *this;
   }

   inline K_mf(TAdmin2Policy&) operator= (const TAdmin2Policy& r) {
      static_cast<TAdmin2Rights&>(*this) = r;
      PolicyRights_ = r.PolicyRights_;
      return *this;
   }

   inline K_mf(void) clear () {
      static_cast<TAdmin2Policy&>(*this) = TAdmin2Policy();
      PolicyRights_.clear();
   }

   TAdmin2Inn::rr_key  mutable   RoomKey_;
   TPolicyRights mutable         PolicyRights_;

   //�t�X TTExtabUpdater2d<> ���s�W:
   inline K_mf(TPolicyRight&) operator[] (const TPolicyName& dkey)  { return PolicyRights_[dkey];       }
   inline K_mf(size_t)        erase      (const TPolicyName& dkey)  { return PolicyRights_.erase(dkey); }
   inline                     operator TPolicyRights& ()            { return PolicyRights_;             }

   static const char  PolicyName[];
};
/////////////////////////////////////////////////////////////////////////////
} } } // namespaces
