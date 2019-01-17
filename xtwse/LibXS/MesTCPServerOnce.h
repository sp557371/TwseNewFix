//
// �q�Tclasses��¦: Session-Messager
//    class TMesTCPServerOnce;
//
// ����: $Id: MesTCPServerOnce.h,v 1.4 2004/01/09 08:47:10 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesTCPServerOnce_H__
#define __MesTCPServerOnce_H__
/////////////////////////////////////////////////////////////////////////////
#include "MesTCPServer.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesTCPServerOnce: public  TMesTCPServer
                             , private TMesEvHandler//handle client ev
{
   typedef TMesTCPServer   inherited;
public:
   K_ctor  TMesTCPServerOnce  ();
   K_dtor ~TMesTCPServerOnce  ();

   enum E2ndClient {
      e2_Close2nd,   //������2�ӳs�u,�O�d��1�ӳs�u(�w�])
      e2_Close1st,   //������1�ӳs�u,�O�d��2�ӳs�u
      //e2_Multi,    //���\�h�s�u(�n�p�󦬰e�O?)
      e2_End,
   };
	//Setting <port,a> for server
   // a=����2�ӳs�u�ɪ��B�z�覡 = E2ndClient
   K_mf(bool)        SettingApply  (const std::string& str);
   K_mf(std::string) GetSetting    ();

   K_mf(int)  Send  (const void *buffer, size_t size);
   K_mf(int)  Recv  (void *buffer, size_t size);

	K_mf(std::string)  GetUID  ();   // Get client id, <ipaddr:port>

protected:
   virtual K_mf(bool)  AcceptClient  (MesSvrClientImpl*&, SOCKET);

   K_mf(void)  OnMesLinkReady    (TMesBase*);
   K_mf(void)  OnMesLinkFail     (TMesBase*, const std::string& reason);
   K_mf(void)  OnMesLinkBroken   (TMesBase*, const std::string& reason);
   K_mf(void)  OnMesSendEmpty    (TMesBase*);
   K_mf(void)  OnMesRecvReady    (TMesBase*, size_t /*size*/);
   K_mf(void)  OnMesStateChanged (TMesBase*, TMesStateLevel, const std::string& /*stateMsg*/);
   K_mf(void)  OnMesBfDestroy    (TMesBase*);
   K_mf(void)  OnMesLeave        (TMesBase*);
   K_mf(void)  OnMesEnter        (TMesBase*);

private:
   E2ndClient  When2ndClient_;
   TMesBase*   Mes_;
   TMesBase*   LastMes_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
