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
/** [Admin2]使用權限.
*/
struct TAdmin2Rights
{
   //使用者登入前置ID
   TPolicyKey  PrevUserID_;

   //第2層User資料庫儲存位置: "主資料庫路徑/adm2/Admin2Path_/*"
   //(空白表示無權限)
   //為了避免重複,此欄位與 key 相同,不可另外設定!
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
   apr_Deny,        //無權限
   apr_SameAsAdmin2,//與Admin2的權限相同
   apr_SetByAdmin2, //由Admin2設定

   apr_Count
} K_EnumPacked;
//---------------------------------------------------------------------------
/** [第2層User的policy]權限.
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
/** [第2層User的policy]權限表.
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
/** [Admin2]使用權限.政策
*/
class TAdmin2Policy : public TAdmin2Rights
{
public:
   typedef TAdmin2Rights   TBase;//要儲存的type.

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

   //配合 TTExtabUpdater2d<> 的新增:
   inline K_mf(TPolicyRight&) operator[] (const TPolicyName& dkey)  { return PolicyRights_[dkey];       }
   inline K_mf(size_t)        erase      (const TPolicyName& dkey)  { return PolicyRights_.erase(dkey); }
   inline                     operator TPolicyRights& ()            { return PolicyRights_;             }

   static const char  PolicyName[];
};
/////////////////////////////////////////////////////////////////////////////
} } } // namespaces
