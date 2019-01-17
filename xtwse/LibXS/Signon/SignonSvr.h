//
// Singon Server �ݪ��B�z �� class
//
// ����: $Id: SignonSvr.h,v 1.5 2006/04/04 03:57:24 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SignonSvrH
#define SignonSvrH
//---------------------------------------------------------------------------
#include "Signon/Policy/PolicyBase.h"
#include "pod_vector.h"
#include "KTime.h"
#include <set>
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
//---------------------------------------------------------------------------
using Kway::Signon::Policy::TOnPmUpdate;
using Kway::Signon::Policy::TPolicyKey;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSignonSvr;
/////////////////////////////////////////////////////////////////////////////
struct TSignonSvrEvHandler
{
   virtual K_mf(void) OnSignonSvrDestroy (TSignonSvr*) = 0;
};
//---------------------------------------------------------------------------
struct TOnlineUserData
{
   std::string    UserID_;
   std::string    From_;      //�n�J�ӷ�
   std::string    ClientAP_;
   TKDateTimeMS   Time_;      //�n�J�ɶ�
};
//---------------------------------------------------------------------------
/** 2008/07/09 ��ΫD�P�B [�n�J���G�q��]
      SignonSvr �i��P�ɤ䴩 [�P�B] �� [�D�P�B]
      �Ҧp: �i�ھ� userid ���K�X�ˬd�F��, �ӨM�w�ϥ� LDAP �ˬd�K�X, �Ϊ����ϥ� SignonDB �ˬd.
*/
LIBNS_class TSignonResult
{
protected:
   virtual K_dtor ~TSignonResult() {
   }
public:
   typedef Kway::Signon::Policy::TOnlineID   TOnlineID;
   virtual K_mf(void) OnSignonResult    (TSignonSvr*, int pendingID, const std::string& user, TOnlineID oid, const std::string& result) = 0;
   virtual K_mf(void) OnChgPassResult   (TSignonSvr*, int pendingID, const std::string& user, const std::string& result) = 0;
   virtual K_mf(void) OnCheckPassResult (TSignonSvr*, int pendingID, const std::string& user, bool pass) = 0;
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSignonSvr
{
public:
   //----- types.
   typedef Kway::Signon::Policy::TOnlineID         TOnlineID;
   typedef Kway::Signon::Policy::TPolicyBaseRef    TPolicyBaseRef;

   //----- constructor/destructor.
           K_ctor  TSignonSvr  ();
   virtual K_dtor ~TSignonSvr  ();

   //----- �u�@���s�u�ӷ��{��
   virtual K_mf(TPolicyBaseRef)  GetUFromPolicy  (const std::string& ufrom, const std::string& policyName, bool forLocalUse = true) const = 0;

   /* 2008/07/09 ��ΫD�P�B [�n�J���G�q��]
   //----- �ϥΪ̨����{�� (�ثe�|���Ҽ{�D�P�B�����{�Ҿ���! ���U���������A�X�D�P�B�{��!)
   virtual K_mf(std::string)     ChgPass        ( const std::string& user, const std::string& oldPass, const std::string& newPass
                                                , const std::string& ufrom) = 0;
   virtual K_mf(std::string)     SignonUser     ( const std::string& user,  const std::string& pass
                                                , const std::string& ufrom, const std::string& clientAP
                                                , TOnlineID&) = 0;
   ///> ���ˬd�b���K�X�O�_���T.
   virtual K_mf(bool)            CheckPass      ( const std::string& user,  const std::string& pass ) = 0;
   */
   /// �ϥΪ̨����{��, ��K�X...
   /// �Ǧ^ pendingID( > 0) ���� TSignonResult::OnSignonResult() �q�����̾�,
   /// �Y�b��^�e�I�s TSignonResult::OnSignonResult(), �h���� pendingID=0, ��^0
   virtual K_mf(int) ChgPass    ( const std::string& user,  const std::string& oldPass, const std::string& newPass
                                , const std::string& ufrom, TSignonResult&) = 0;
   virtual K_mf(int) SignonUser ( const std::string& user,  const std::string& pass
                                , const std::string& ufrom, const std::string& clientAP
                                , TSignonResult&) = 0;
   /// ���ˬd�b���K�X�O�_���T.
   virtual K_mf(int) CheckPass  ( const std::string& user,  const std::string& pass, TSignonResult& ) = 0;
   /// �����|�����o���G���n�J�q��, �Y pendingID<=0, �h���������� sgnResult
   /// �Y pendingID > 0, �h�������w�� pendingID, �B�� ID ������ TSignonResult ���� = sgnResult
   virtual K_mf(bool) CancelSignonResult (int pendingID, TSignonResult* sgnResult) = 0;

   ///> ���o�ϥΪ��v���F��
   virtual K_mf(TPolicyBaseRef)  GetUserPolicy  ( const std::string& user, const std::string& policyName, bool forLocalUse = true) const = 0;

   /** 2005/01/13�W�[:�O�_�i���Ƶn�J����,
       �ҥH�I�sSignonUser()����,�Y�ϥΪ��_�u�εn�X,�аȥ��I�sSignoutUser().
       �_�h�i��y���ϥΪ̵L�k�n�J�����p.
   */
   virtual K_mf(bool) SignoutUser (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID&) = 0;

   /** ���U��Policy�����ʮɳq��.
   */
   virtual K_mf(bool) AddOnUpdate (const std::string& user, const std::string& policyName, TOnPmUpdate&) = 0;
   virtual K_mf(bool) DelOnUpdate (const std::string& user, const std::string& policyName, TOnPmUpdate&) = 0;

   /** ��SignonSvr���`�ɳq��.
   */
   inline K_mf(bool) AddSignonSvrEvHandler (TSignonSvrEvHandler* v)  { return SignonSvrEvHandlers_.add(v); }
   inline K_mf(bool) DelSignonSvrEvHandler (TSignonSvrEvHandler* v)  { return SignonSvrEvHandlers_.del(v); }

   /** ���o������ user-id.
   */
   virtual K_mf(bool) GetUsersList (std::set<std::string>&) = 0;

   /** �ھ� OnlineID ���o�u�W�ϥΪ�.
   */
   virtual K_mf(const TOnlineUserData*) GetOnlineUser (TOnlineID) = 0;

   /** �K�X�䴩���ؽs�X�覡
   */
   virtual K_mf(Kway::byte) GetPassEncMode () = 0;

   /** �@���s�u(LinkReady)�N�i�H�O��log, �Φ���L�ݭn�]�i�H�O��!
   */
   virtual K_mf(void) WriteLog (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID oid, const char* msg) = 0;
private:
   typedef Kstl::pod_vector<TSignonSvrEvHandler*>  TSignonSvrEvHandlers;
   TSignonSvrEvHandlers SignonSvrEvHandlers_;
};
/////////////////////////////////////////////////////////////////////////////
/** �Ǧ^�]�w�e�� SignonSvr.
*/
LIBNS_fn(TSignonSvr*)  SetGlobalSignonSvr (TSignonSvr*);

/** ���o�{�b�� SignonSvr.
*/
LIBNS_fn(TSignonSvr*)  GetGlobalSignonSvr ();
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif
