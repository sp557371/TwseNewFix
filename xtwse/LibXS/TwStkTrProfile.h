//
// TwStk �� [����v��]�]�w, �� ����[�e�U�y�{]�^�Щe�U���A����class:
//    class TTrProfile;
//
// ����: $Id: TwStkTrProfile.h,v 1.3 2005/12/28 09:15:47 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkTrProfileH
#define TwStkTrProfileH
/////////////////////////////////////////////////////////////////////////////
#include "TwStkOrdFlow.h"
//---------------------------------------------------------------------------
#include "Signon/Policy/UPTwStkIvacNos.h"
#include "Signon/Policy/UPTwStkSaleNos.h"
#include "Signon/Policy/UPTwStkRights.h"
#include "Signon/Policy/UPTwStkScItems.h"
#include "Signon/Policy/UFTwStkRes.h"
//---------------------------------------------------------------------------
#include <set>
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
   LIBNS_class TSignonSvr;
}
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
typedef std::set<Kway::Signon::Policy::TUserTwStkIvacRec>   TTrIvacNos;
typedef std::set<Kway::Signon::Policy::TUserTwStkSaleRec>   TTrSaleNos;
typedef Kway::Signon::Policy::TUserTwStkRights              TTrRights;
typedef Kway::Signon::Policy::TUserTwStkScItems             TTrScItems;
typedef Kway::Signon::Policy::TUserTwStkRes                 TTrRes;
typedef Kstl::char_ary<char,12,0>                           TTrUserID;
typedef Kstl::char_ary1<char,' '>                           TTrTerm;
//---------------------------------------------------------------------------
TWSTK_class  TTrProfile {
protected:
   TTrRights      TrRights_;     //�ϥ��v��
   TTrScItems     TrScItems_;    //�������س]�w
   TTrIvacNos     TrIvacNos_;    //�N�ޱb��
   TTrSaleNos     TrSaleNos_;    //�N����~��
   TTrRes         TrRes_;        //����귽
   TTrUserID      TrUserID_;     //�ϥΪ̥N��
   TTrTerm        TrTerm_;       //�e�U�d��(�w�d)

public:
           K_ctor  TTrProfile  ();
   virtual K_dtor ~TTrProfile  ();

   //���Y req.IsFinish() �h�N Reqs_ ����Ʋ���, �öǦ^ false, ��ܩe�U�y�{�w����
   //  �_�h�Ǧ^ true, ��ܩe�U�y�{�|������
   virtual K_mf(bool) OnOrdReqUpdate  (TOrdFlow::TReqPtr);

   inline K_mf(const TTrRights&)   GetTrRights  () const { return TrRights_;  }  //�ϥ��v��
   inline K_mf(const TTrScItems&)  GetTrScItems () const { return TrScItems_; }  //�������س]�w
   inline K_mf(const TTrIvacNos&)  GetTrIvacNos () const { return TrIvacNos_; }  //�N�ޱb��
   inline K_mf(const TTrSaleNos&)  GetTrSaleNos () const { return TrSaleNos_; }  //�N����~��
   inline K_mf(const TTrRes&)      GetTrRes     () const { return TrRes_;     }  //����귽
   inline K_mf(const TTrUserID&)   GetTrUserID  () const { return TrUserID_;  }  //�ϥΪ̥N��
   inline K_mf(const TTrTerm)      GetTrTerm    () const { return TrTerm_;    }  //�e�U�d��(�w�d)

   /** ���J������ user policys.
   */
   K_mf(void)  GetUserPolicys  (const std::string& user, const Kway::Signon::TSignonSvr*);
private:
   //Reqs_ �� TOrdFlow::DoReq() �P TTrProfile::~TTrProfile() �@�P���@
   // ��req�i�J�e�U�y�{��, TOrdFlow::DoReq() ���d���Nreq����[�J TrProfile->Reqs_
   // �� TTrProfile�Ѻc��, �|�N�|��������Reqs_ �� TrProfile_ �]���w�]��: ������ TrRights_... �Ҭ� NULL
   //                      �p���@��,�|��������Reqs_�N�L�k�i������������ˬd  
   friend class TOrdFlow;
   typedef TOrdFlow::TReqPtrs    TReqs;
   typedef TReqs::iterator       TReqi;
   TReqs    Reqs_;         // �z�L��profile���b�i��e�U�n�D������
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

