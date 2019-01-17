//
// �q�Tclasses��¦: Session-Messager
//    class TMesBase;
//    class TMesEvHandler;
//
// ����: $Id: Mes.h,v 1.10 2005/06/26 11:18:50 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesH
#define MesH
//---------------------------------------------------------------------------
#include <string>
#include "ref_counter.h"
#include "wise_ptr.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
// Communication Messager
// ��s����:
// �� 920704:
//   1.      int  Send  (void* buffer, size_t size)
//      �令 int  Send  (const void* buffer, size_t size) = 0;
//   2. �b�ݭn���a��,�W�[ inline �׹��r
//   3. �W�[ TMesEvHandler::GetClientEvHandler() �ƥ�
//           TMesBase::GetClientEvHandler()
//   4. TMesBase�غc��,�N IsLinkReady_ �]�� false
//
// �� 920709
//   1.�W�[ OnMesBfDestroy () �ƥ�
//   2.�N   TMesEvHandler& EvHandler_;
//     �令 TMesEvHandler* EvHandler_;
//     �æb�I�s�ƥ��,�P�_ EvHandler_ �O�_�� 0
//   3.�W�[ public:  TMesEvHandler* GetEvHandler ();
//                   void           SetEvHandler (TMesEvHandler*);
//   4.�N   TMesBase::TMesBase(TMesEvHandler&);
//     �令 TMesBase::TMesBase()���a�Ѽ�
//   5.�W�[ TMesEvHandler::OnMesLeave() �ƥ�
//          TMesEvHandler::OnMesEnter() �ƥ�
//   6.�W�[ K_mf() K_ctor K_dtor
//   7. OnClientConnect() �Ǧ^ bool �N���\�Υ���,�Y�Ǧ^false,�h clientMes ���i�A�ϥ�!
// ��920716
//   �W�[ TSyncObj ����
//   �N const char* �令 const std::string&
// ��920804
//   ���� virtual K_mf(bool)  IsListenServer  ();
//   ��ѵ��U�ɫ��w
// ��921225
//   �令 ref_counter_ns
//---------------------------------------------------------------------------
LIBNS_class TMesBase;
LIBNS_class TMesNoThread;
//---------------------------------------------------------------------------
enum TMesStateLevel
{  //�p��M�wWarning,Error,Fatal,�ХѼ��gMes���H�ۦ��P,���ݴ��Ѥ�󻡩�
   mslNormal,     //�@�몬�p,�Ҧp:��[�s�u��]�ܦ�[�s�uReady]
   mslWarning,    //�i�঳���`,�����Y��,�i��Mes�ۦ�ѨM
   mslError,      //�@����~,�����Y��,�i��Mes�ۦ�ѨM
   mslFatal,      //�Y�������~,�ݤH�u���J�B�z
};
enum EMesLinkState {
   mls_Initing,
   mls_Opening,
   mls_LinkReady,
   mls_ListenReady,
   mls_Closing,
   mls_Closed,
   mls_LinkError,
   mls_Destroying,
};
//---------------------------------------------------------------------------
LIBNS_class TMesEvHandler
{
// PS.���U��������[����]�u�O�@�ӷ���,�Ҧp:
//    1.�Y�}�Ҫ�Mes��Local�q���W��RS232 port,�Y�}�Ҧ��\�N��O�P[����]�s�u����
//    2.�Y�}�Ҫ��O�@��MesModem:
//      �h���������ûP���ݪ�Modem���q����,�B�i�}�l���e���,�~��O�P���ݳs�u����
friend class TMesBase;
friend class TMesNoThread;
//�s�u�}�ҫ�,�P���ݳs�u����,�i�}�l���e���
//  ����MesModem���Ҥl:�Y�P���ݪ�Modem���q����,�w�i�}�l���e��ƮɩI�s��Event
virtual K_mf(void)  OnMesLinkReady   (TMesBase*) = 0;
//�s�u�}�ҫ�,�L�k�P���ݳs�u
//  ����MesModem���Ҥl:�Y�P���ݦ��u�BModem���q����...�h�I�s��Event
virtual K_mf(void)  OnMesLinkFail    (TMesBase*, const std::string& reason) = 0;
//�w�g�s�u,���᪺�_�u
//  ����MesModem���Ҥl:����Modem�w�g�s�uReady,���q���_�u...�h�I�s��Event
virtual K_mf(void)  OnMesLinkBroken  (TMesBase*, const std::string& reason) = 0;

//�ǰe�w�İϸ�Ƥw�e��
virtual K_mf(void)  OnMesSendEmpty  (TMesBase*) { }
//�����w�İϤw�����,���YMes���䴩�w�����Ѥw���쪺��ƶq,�hsize��J0
virtual K_mf(void)  OnMesRecvReady  (TMesBase*, size_t /*size*/) { }

//��Mes�����A���ܤF,�ХѦ��B�q��
virtual K_mf(void)  OnMesStateChanged (TMesBase*, TMesStateLevel, const std::string& /*stateMsg*/) { }
virtual K_mf(void)  OnMesSetRemark    (TMesBase*, const std::string& /*remarkMsg*/) { }

//�Y��Mes�� ListenServer:
// (1) �����o�@�� EvHandler, �Y�Ǧ^ NULL, �h��ܦ� Handler ���B�z Client �s�u���ƥ�
//     �Щڵ� Client ���s�u
virtual K_mf(TMesEvHandler*)  GetClientEvHandler (TMesBase* mesServer)  { return NULL; }
// (2) �z�L���ƥ�q��,��Client�s�u�W�ӤF
virtual K_mf(bool)  OnMesClientConnect  (TMesBase* mesServer, TMesBase* mesClient)  { return false; }

//��Mes�Ѻc��,�ݭn�I�s���ƥ�:
virtual K_mf(void)  OnMesBfDestroy (TMesBase*)  {}

//��Mes�P MesEvHandler [�������Y/�إ����Y]��,�|�I�s���ƥ�:
virtual K_mf(void)  OnMesLeave (TMesBase*)   {}
virtual K_mf(void)  OnMesEnter (TMesBase*)   {}

public:
virtual K_dtor  ~TMesEvHandler  ()  {}
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesBase : public Kstl::ref_counter<>
{
   EMesLinkState  LinkState_;
   TMesEvHandler* EvHandler_;
   class TMesSyncChange;
   friend class TMesSyncChange;
   friend class TMesNoThread;
public:
   //�غc & �Ѻc
           K_ctor  TMesBase ()  : LinkState_(mls_Initing), EvHandler_(0)  { }
protected:
   virtual K_dtor ~TMesBase ()  { OnBfDestroy(); }

public:
   typedef Kstl::auto_ref<TMesBase>   TLocker;
   typedef Kstl::wise_ref<TMesBase>   TMesPtr;

   //�P�_�s�u�O�_Ready
   inline  K_mf(bool)           IsLinkReady  ()          { return LinkState_ == mls_LinkReady; }
   inline  K_mf(EMesLinkState)  GetLinkState () const    { return LinkState_;   }

   //��Mes�O�_�|���� OnMesClientConnect() �ƥ�?
   //virtual K_mf(bool)  IsListenServer  ()  { return false; }

   //�]�w/���XMes���Ѽ�(Mes���]�w,�ХμзǤ�Ʀr�βŸ����,�B�ݴ��Ѥ�󻡩�)
   virtual K_mf(std::string)  GetSetting   ()                     { return std::string(); }
   virtual K_mf(bool)         SettingApply (const std::string&)   { return true; }

   //�}��/�����s�u
   virtual K_mf(bool)  Open         ();
   virtual K_mf(bool)  Close        ();
   virtual K_mf(bool)  CloseReason  (const std::string& reason);//�I�s OnSetRemark(reason), SetLinkError(reason), Close();

   //�ǰe/�������
   virtual K_mf(int)  Send  (const void* buffer, size_t size) = 0;
   virtual K_mf(int)  Recv  (      void* buffer, size_t size) = 0;

   //������[�ǰe/����]�w�İϪ��j�p
   virtual K_mf(bool)  GetBufSize  (size_t& sendBufSz, size_t& recvBufSz)  { return false; }
   virtual K_mf(bool)  SetBufSize  (size_t  sendBufSz, size_t  recvBufSz)  { return false; }

   //�����o�{�b[�ǰe/����]�w�İ�,���h��Bytes����Ƶ��ݳB�z,�Ǧ^-1���Mes�����Ѧ��S��
   virtual K_mf(int)  GetSendSize  ()  { return -1; }
   virtual K_mf(int)  GetRecvSize  ()  { return -1; }

   //�����o��Mes���ѧO�N��,��:�Y��Local RS232= "dev/COM1"
   //                         �Y��TCP  Client= "ip/127.0.0.1"
   //                         �Y��TCP  Server= "ip/listen:5000"
   //                         ...�H�W�Ȭ��|��,��ڶǦ^�ȥu�n�i�H�ݱo���Y�i
   virtual K_mf(std::string)  GetUID  () { return std::string(); }
   //���̫��Ʊq����UID��(�A�Ω�UDP...�������s������Mes)
   virtual K_mf(std::string)  GetLastUID  () { return GetUID(); }

   inline K_mf(TMesEvHandler*)  GetEvHandler  ()  { return EvHandler_; }
          K_mf(void)            SetEvHandler  (TMesEvHandler* newEvHandler);
protected:
   //��Mes�Ѻc�ɩI�s���ƥ�
   K_mf(void)  OnBfDestroy  ();
   //��P���ݳs�u����,�ЩI�s��method
   K_mf(void)  SetLinkReady ();
   //��Server�ݶ}�l���Գs�u.
   K_mf(void)  StartListen  ();
   //��[�s�u�����\] or [�_�u],�ЩI�s�o�̪�method
   K_mf(void)  SetLinkError  (const std::string& reason);
   //��w�İϪ���Ƥw�ǰe����,�ЩI�s�o�̪�method
   K_mf(void)  OnSendEmpty  ();
   //�����w�İϤw�����,�ЩI�s�o�̪�method
   //�YMes���䴩�w�����Ѥw���쪺��ƶq,�hsize��J0
   K_mf(void)  OnRecvReady  (size_t size);
   //��Mes�����A���ܤF,�бq�o��q��
   K_mf(void)  OnStateChanged  (TMesStateLevel msLevel, const std::string& stateMsg);
   K_mf(void)  OnSetRemark     (const std::string& remarkMsg);
   //��Mes�� ListenServer ��,�Y��Client�s�u�h�|���ͩ��U���ƥ�
   K_mf(TMesEvHandler*)  GetClientEvHandler ();
   K_mf(bool)            OnClientConnect    (TMesBase* mesClient);

private:
   virtual K_mf(bool)  OpenImpl   () = 0;
   virtual K_mf(bool)  CloseImpl  () = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TMesNoThread : public TMesBase
{
   typedef TMesBase  inherited;
protected:
   inline K_mf(void)  OnSendEmpty     ()            { if(EvHandler_)  EvHandler_->OnMesSendEmpty(this);    }
   inline K_mf(void)  OnRecvReady     (size_t sz)   { if(EvHandler_)  EvHandler_->OnMesRecvReady(this,sz); }
   inline K_mf(void)  OnStateChanged  (TMesStateLevel msl, const std::string& stateMsg)
                                                    { if(EvHandler_)  EvHandler_->OnMesStateChanged(this, msl, stateMsg); }
   inline K_mf(void)  OnSetRemark     (const std::string& remarkMsg)
                                                    { if(EvHandler_)  EvHandler_->OnMesSetRemark(this, remarkMsg); }
};
//---------------------------------------------------------------------------
typedef TMesBase::TMesPtr  TMesPtr;
/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway
#endif

