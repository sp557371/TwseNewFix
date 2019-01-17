//
// �Ȥ�ݺ��@�n�J���A�� class:
//    class TSignonClient
//
// ����: $Id: SignonClient.h,v 1.3 2006/04/04 03:57:58 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SignonClientH
#define SignonClientH
//---------------------------------------------------------------------------
#include <map>
#include <vector>
//---------------------------------------------------------------------------
#include "pod_vector.h"
#include "Ses.h"
#include "Signon/OnlineUser.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TSignonClient;
//---------------------------------------------------------------------------
enum ESignonClientState {
      sc_NoSes,             //�|���إ�Session
      sc_WaitLinkReady,     //Session�s�u��
      sc_WaitConnected,     //�s�u�n�D�ǰe��,��Server�^��
      sc_WaitSignonInput,   //�s�uOK,���ԨϥΪ�ñ�J
      sc_WaitSignonLink,    //���ԭ��s�s�u��n�J.  
      sc_WaitSignonResult,  //�n�J�n�D�ǰe��,��Server�^��
      sc_WaitChgPass,       //�K�X���T,���Х���K�X��~�i�n�J.
      sc_Ready,             //�w�n�J����
      sc_Signout,           //�t�εn�X
};
//---------------------------------------------------------------------------
LIBNS_class  TSignonAckEv
{
   friend class TSignonClient;
   //if(result.empty()) => success
   //else  result is error message!
   virtual K_mf(void)  OnSignonResult  (TSignonClient&, const std::string& result) = 0;
   virtual K_mf(void)  OnSignonChgPass (TSignonClient&, const std::string& result) {}
   virtual K_mf(void)  OnSignout       (TSignonClient&) {}
   virtual K_mf(void)  OnStateChanged  (TSignonClient&, ESignonClientState oldState) {}
   virtual K_mf(void)  OnPolicyChanged (TSignonClient&, const std::string& policyName) {}
   virtual K_mf(void)  OnChkTableRightResult (TSignonClient&, const std::string& user, const std::string& tableName, const std::string& result) {}
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TSignonClient : public TOnlineUser
{  //�Y�h��Ses�n�@�ΦP�@�ӵn�J�ϥΪ�,�h�u�n�@�Φ�class�Y�i
public:
   typedef ESignonClientState    EState;
   //----- ��n�J���A���ܮɭt�d�q�������� Session
   struct TSesInfo {
      TSesBase*      Ses_;
      std::string    ServerName_;
      bool           IsReady_;   //is ready for Signon
      inline K_ctor      TSesInfo    (TSesBase& ses)  : Ses_(&ses), ServerName_(), IsReady_(false)  {}
      inline K_mf(bool)  operator==  (const TSesInfo& i) const  { return(Ses_==i.Ses_); }
      inline K_mf(bool)  operator!=  (const TSesInfo& i) const  { return(Ses_!=i.Ses_); }
   }; //���B���A�� pod_vector<> �]�� ServerName_ �� std::string
   typedef std::vector<TSesInfo>    TSesClients;
   typedef TSesClients::iterator    TSesClienti;

private:
   //----- �t�d�x���{�b���A
   EState   State_;

protected:
   //----- �t�d�n�J�ƥ�q��
   typedef Kstl::pod_vector<TSignonAckEv*>   TEvs;
   typedef TEvs::iterator                    TEvi;
   TEvs  evs_;

   //----- �t�d���� Policy -----
   typedef std::string  TPolicyName;
   typedef std::string  TPolicyData;
   typedef std::map<TPolicyName,TPolicyData> TPolicys;
   typedef TPolicys::iterator                TPolicyi;
   TPolicys    UFromPolicys_;
   TPolicys    UserPolicys_;

protected:
   TSesClients    Ses_;

   K_mf(void)  AddSes    (TSesBase&);
   K_mf(void)  DelSes    (TSesBase&);
   K_mf(void)  SetState  (EState);
   typedef void (K_FASTCALL TSignonAckEv::*TSignonResultEv) (TSignonClient&, const std::string&);
   K_mf(void)  OnSignonResultEv      (TSignonResultEv fnEv, const std::string& result);
   K_mf(void)  OnSignonResult        (TSesBase*, const std::string& result);
   K_mf(void)  OnPolicyChanged       (const std::string& policyName);
   K_mf(void)  OnChkTableRightResult (const std::string& user, const std::string& tableName, const std::string& result);

   inline K_mf(void) OnChgPassResult (const std::string& result)  { OnSignonResultEv(&TSignonAckEv::OnSignonChgPass, result); }
public:
   K_ctor  TSignonClient  ();

   //���Ѫ��򥻪A��, �Y������,�h���I�s evs_ �������ƥ�
   virtual K_mf(void)  Open     ();//�}�ҥ�����Ses,�ǳƵn�J�@�~
   virtual K_mf(void)  Signon   (const std::string& user, const std::string& pass)=0;
   virtual K_mf(void)  ChgPass  (const std::string& user, const std::string& oldPass, const std::string& newPass)=0;
   virtual K_mf(void)  Signout  ();//�t�εn�X:Close all Ses 

   //�ˬd[�f�֪�]�O�_���v���]�w tableName !
   virtual K_mf(void)  CheckTableRight (const std::string& user, const std::string& pass, const std::string& tableName)=0;

   //Ses���޲z: 1.�]�w�Y��Ses�s�u��[�D���W��],�άO�_��Ready���A(is ready for Signon?)
   K_mf(void)  SetServerName  (TSesBase&, const std::string& serverName, bool IsReady=true);
   K_mf(void)  SetSesReady    (TSesBase&, bool IsReady);
   //           2.��[�D���W��]��������Ses,�Y���ۦP���W��,�h�ϥβĤ@��LinkReady��Ses
   K_mf(TSesBase*)  GetSes  (const std::string& serverName) const;
   //           3.���X������[�D���W��]
   K_mf(const TSesClients&)  GetSes  () const  { return Ses_; }

   //�ƥ�B�z
   inline K_mf(void)  Add  (TSignonAckEv& ev)   { evs_.add(&ev);  }
   inline K_mf(void)  Del  (TSignonAckEv& ev)   { evs_.del(&ev);  }

   //���o�n�J���󪺪��A
   K_mf(EState)  GetState  () const    { return State_; }

   //���o [�ϥΪ�/�u�@��] �޲z�F��
   typedef Kway::Signon::Policy::TPolicyBaseRef    TPolicyBaseRef;
   K_mf(TPolicyBaseRef)  GetUserPolicy   (const TPolicyName&) const;
   K_mf(TPolicyBaseRef)  GetUFromPolicy  (const TPolicyName&) const;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

