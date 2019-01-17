//
// Singon Server �ݪ��B�z - �ϥ� SignonDB (DataCenter)
//
// ����: $Id: SignonSvr_DC.h,v 1.7 2006/04/04 03:56:03 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SignonSvr_DCH
#define SignonSvr_DCH
//---------------------------------------------------------------------------
//
// SignOnSvr Data Center
//
//---------------------------------------------------------------------------
#include "SignonSvr.h"
#include "PmBase.h"
#include "PmUFrom.h"       //class TPmUFromGroup;
#include "PmUFromIPv4.h"   //class TPmUFromIPv4;
#include "PmUserDB.h"      //class TPmUserDB;
#include <deque>
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TSignonSvr_DataCenter;
//---------------------------------------------------------------------------
// SignonSvr �X�R�Ҳժ��g�k:
//    (1) �W��: sneXXXXX.dll �� sneXXXXX.bpl
//    (2) ���� 2 �� functions: (�аȥ��b function [�W�٤��e][�Ǧ^�Ȥ���] �[�W __export)
typedef void*  TsneStartObj;
 SIGDB_exportC(TsneStartObj)   sneStart    (TSignonSvr_DataCenter&);
 SIGDB_exportC(void)           sneFinish   (TSignonSvr_DataCenter&, TsneStartObj);
  typedef  TsneStartObj     (*TsneStart)   (TSignonSvr_DataCenter&);
  typedef  void             (*TsneFinish)  (TSignonSvr_DataCenter&, TsneStartObj);
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TSignonSvr_DataCenter : public TSignonSvr
{
public:
   K_ctor  TSignonSvr_DataCenter  (const std::string& defaultPath, const std::string& signonLogFileName);

   inline K_mf(std::string)    GetPathDB     ()    { return PathDB_;     }
   inline K_mf(TViewAreaMgr&)  GetMainVAMgr  ()    { return MainVAMgr_;  }//ñ�J�򥻸�ƺ޲zVAMgr
   inline K_mf(TPmMgr&)        GetUserPmMgr  ()    { return UserPmMgr_;  }//[�ϥΪ̬F��]�޲z
   inline K_mf(TPmMgr&)        GetUFromPmMgr ()    { return UFromPmMgr_; }//[�ϥΪ̨ӷ��F��]�޲z
   inline K_mf(TPmGroup&)      GetPmUserGroup()    { return *(PmUserGroup_.get()); }//�ϥΪ̬F���޲z
   inline K_mf(TPmUserDB&)     GetUserDB     ()    { return *(UserDB_.get()); }
   virtual K_mf(TViewAreaMgr*) GetSignonVAMgr()    { return NULL;  }

   //----- �����ˬd
   typedef Kway::Signon::Policy::TOnlineID         TOnlineID;
   typedef Kway::Signon::Policy::TPolicyBaseRef    TPolicyBaseRef;
   K_mf(TPolicyBaseRef)  GetUserPolicy   (const std::string& user,  const std::string& policyName, bool forLocalUse = true) const;
   K_mf(TPolicyBaseRef)  GetUFromPolicy  (const std::string& ufrom, const std::string& policyName, bool forLocalUse = true) const;
   K_mf(bool)            GetUsersList    (std::set<std::string>&);

   //2008/07/09 ��ΫD�P�B [�n�J���G�q��]
   virtual K_mf(int) ChgPass    ( const std::string& user,  const std::string& oldPass, const std::string& newPass
                                , const std::string& ufrom, TSignonResult&);
   virtual K_mf(int) SignonUser ( const std::string& user,  const std::string& pass
                                , const std::string& ufrom, const std::string& clientAP
                                , TSignonResult&);
   virtual K_mf(int) CheckPass  ( const std::string& user,  const std::string& pass, TSignonResult& );
   virtual K_mf(bool) CancelSignonResult (int pendingID, TSignonResult* sgnResult);

   //----- Policy���ʳq��.
   K_mf(bool) AddOnUpdate (const std::string& user, const std::string& policyName, TOnPmUpdate&);
   K_mf(bool) DelOnUpdate (const std::string& user, const std::string& policyName, TOnPmUpdate&);

   /** 2005/01/13�W�[:�O�_�i���Ƶn�J����,
       �ҥH�I�sSignonUser()����,�Y�ϥΪ��_�u�εn�X,�аȥ��I�sSignoutUser().
       �_�h�i��y���ϥΪ̵L�k�n�J�����p.
   */
   K_mf(bool) SignoutUser (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID&);

   ///���o�u�W�ϥΪ�
   K_mf(const TOnlineUserData*) GetOnlineUser (TOnlineID);

   /** �K�X�䴩���ؽs�X�覡
   */
   virtual K_mf(Kway::byte) GetPassEncMode ();
   virtual K_mf(void) WriteLog (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID onlineID, const char* msg);

   inline K_mf(void) SetPostSignonSvr(TSignonSvr* svr) {
      PostSignonSvr_ = svr;
   }
private:
   std::string    PathDB_;  //Signon��Ʈw�w�]���|

protected:
   K_mf(std::string)  DoChgPass    (const std::string& user,  const std::string& oldPass, const std::string& newPass, const std::string& ufrom);
   K_mf(std::string)  DoSignonUser (const std::string& user,  const std::string& pass, const std::string& ufrom, const std::string& clientAP, TOnlineID&);
   K_mf(bool)         DoCheckPass  (const std::string& user,  const std::string& pass);

   //�ϥ�[�ӷ��޲z],��[�ϥΪ̰򥻸��]��:�Ъ`�N�غc����
   std::auto_ptr<TPmUFromGroup>     PmUFromGroup_;    //�ϥΪ̨ӷ��޲z:�s�դ����޲z,�ӷ��޲z�F��
   std::auto_ptr<TPmUFromIPv4>      UFromIPv4_;       //IPv4���u�@���޲z
   std::auto_ptr<TPmGroup>          PmUserGroup_;     //�ϥΪ̬F���޲z
   std::auto_ptr<TPmUserDB>         UserDB_;          //�ϥΪ̸�Ʈw

   //�إ� [�F���޲z] ������ƪ�, ���ѵ��U��
   TPmMgr         UserPmMgr_;    //�ϥΪ̺޲z�F����
   TPmMgr         UFromPmMgr_;   //�u�@���ӷ��޲z�F����

   // �� PmMgr_ ���U���H, �����ۦ�������U
   // (�Y�z�ϥ� new ���覡�إ߱z�ۤv������ӵ��U,
   //  �h�i Handle TVACreater::OnBfVAMgrDestroy() ���ƥ�,�ӱN�z�إߪ��������)
   // �_�h�p�G�z������� PmMgr_ ����,�ӤS�S���������U,
   // �� PmMgr_ �n�o�X OnBfVAMgrDestroy() ���ƥ�q����, ���N����F!!

   SIGDB_class TOnlineUserMgr : public TVACreater {
      typedef TVACreater   inherited;
   public:
      K_ctor  TOnlineUserMgr (const TPmUserDB&, const std::string& signonLogFileName);

      K_mf(void)      GetUserCount(const std::string& user, const std::string& ufrom, const std::string& clientAP, size_t& ufCount, size_t& dufCount) const;
      K_mf(TOnlineID) SignonUser  (const std::string& user, const std::string& ufrom, const std::string& clientAP);
      K_mf(bool)      SignoutUser (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID);
      K_mf(void)      WriteErrLog (const std::string& user, const std::string& ufrom, const std::string& clientAP, const std::string& msg);
      K_mf(void)      WriteLog    (const TKDateTimeMS& time
                                  , const std::string& user
                                  , const std::string& aufrom
                                  , const std::string& clientAP
                                  , TOnlineID          onlineID
                                  , const char*        appMsg );
      ///���o�u�W�ϥΪ�
      K_mf(const TOnlineUserData*) GetOnlineUser (TOnlineID);
      K_mf(int) GetUserCount() const {
         return (int)OnlineMap_.size();
      }
   private:

      TFile             Log_;
      TKDate            LastMsgDate_;
      const TPmUserDB*  UserDB_;    //�d�ߩm�W��.
      typedef Kstl::ev_map<TOnlineID,TOnlineUserData> TOnlineUsers;
      typedef Kstl::ev_map<TUserID,TOnlineUsers>      TOnlineUsersMap;
      typedef std::map<TOnlineID,TOnlineUserData*>    TOnlineMap;
      TOnlineUsersMap         OnlineUsersMap_;
      TOnlineID               LastOnlineID_;
      std::deque<TOnlineID>   FreeOnlineIDs_;
      TOnlineMap              OnlineMap_;

      struct  TFieldMaker {
         typedef TOnlineUsersMap          TEvContainer;
         typedef TEvContainer::iterator   iterator;
         typedef size_t                   field_n_type;

         inline K_ctor  TFieldMaker  (const TPmUserDB* userDB) : UserDB_(userDB) {}
         static K_mf(field_n_type) GetFieldCount  ();
         K_mf(const TFieldDesp&)   GetFieldDesp   (field_n_type fieldNo) const;
         K_mf(std::string)         Get  (const iterator& i, field_n_type fieldNo) const;
         K_mf(void)                Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
      private:
         const TPmUserDB*  UserDB_;
      }; // class TFieldMaker
      typedef TTViewArea<TOnlineUsersMap,TFieldMaker>  TOnlineVA;

      struct TvacOnlineUser : public TVACreater {
         typedef TOnlineVA       TMasterVA;
         typedef TOnlineUsersMap TMasterRecs;
         typedef TOnlineUsers    TRecs;
         struct  TFieldMaker {
            typedef TRecs                    TEvContainer;
            typedef TEvContainer::iterator   iterator;
            typedef size_t                   field_n_type;

            static K_mf(field_n_type) GetFieldCount  ();
            K_mf(const TFieldDesp&)   GetFieldDesp   (field_n_type fieldNo) const;
            K_mf(std::string)         Get  (const iterator& i, field_n_type fieldNo) const;
            K_mf(void)                Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
         }; // class TFieldMaker
         typedef TTViewArea<TRecs,TFieldMaker>  TVADetail;
         K_mf(TaViewArea)  CreateNewVA  ();

         struct TDetailtMaker {
               inline K_mf(TRecs*) operator() (const TMasterRecs::iterator& mi, bool isCreateNeeded)
                        { return &(mi->second); }
         };
      };

      TvacOnlineUser    VACDetail_;
      K_mf(TVACreater*) GetDetailVACreater (size_t);
      K_mf(size_t)      GetDetails         (const char**& details);
      //�~�Ӧ� TVACreater, �� ViewAreaMgr �޲z�Ϊ� members
      K_mf(TaViewArea)  CreateNewVA        ();
   };
   TOnlineUserMgr OnlineUserMgr_;

   //�H�W��ƪ��޲z��,
   //���F��VAMgr�����ɳq���H�W����ƪ�,�ҥH MainVAMgr_ ������b�W�z��ƪ�غc��������
   TViewAreaMgr   MainVAMgr_;

   TSignonSvr*    PostSignonSvr_;
   std::string    PrevSgnUserID_;

   K_mf(bool) CheckPrevSgnUserID(const std::string& user, std::string& user2) const;
};
/////////////////////////////////////////////////////////////////////////////
// K_fn(TSignonSvr_DataCenter&)  GetDefaultSignonSvr_DC  (); fonwin:921106:����
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif
