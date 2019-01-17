//
// �q�Tclasses��¦: Session-Messager
//    class TMesHTTP;
//
// ����: $Id$
//
//---------------------------------------------------------------------------
#ifndef MesHTTP_H
#define MesHTTP_H
//---------------------------------------------------------------------------
#include "MesTCPClient.h"
//---------------------------------------------------------------------------
namespace Kway {
struct TMesReg;// MesRegs.h
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesHTTP : public TMesTCPClient
{
   typedef TMesTCPClient   inherited;
public:
   K_ctor  TMesHTTP  ();
   K_dtor ~TMesHTTP  ();

   K_mf(int)  Send  (const void* buffer, size_t size);
   K_mf(int)  Recv  (      void* buffer, size_t size);

   static K_mf(TMesReg&)  GetReg  ();

   // TCPClient-Setting;HttpSetting
   // HttpSetting = RecvHead(Y/N),LinkReadySendFile,SendHttpHeadFile
   //    RecvHead = Y:��I�sRecv()�ɷ|�s�P Http head �@�_�Ǧ^.
   //    LinkReadySendFile = ��s�u���\��,�ߧY�e�X���ɤ��e.
   //    SendHttpHeadFile  = ��I�sSend()��,�n�e�X�� Http head ���.
   //                        ����<����>�]�t "Content-Length:" Send()��size�|��J���B
   K_mf(bool) SettingApply (const std::string&);
   K_mf(std::string) GetSetting ();

protected:
   K_mf(bool) OpenImpl ();

private:
   inline K_mf(int)  PhySend  (const void* buffer, size_t size)  { return inherited::Send(buffer,size); }
   inline K_mf(int)  PhyRecv  (      void* buffer, size_t size)  { return inherited::Recv(buffer,size); }

   K_mf(void)  OnSocketDataIn    (size_t);
   K_mf(void)  OnSocketLinkReady ();
   K_mf(void)  OnSocketLinkError (const std::string&);

   class  TImpl;
   TImpl* Impl_;
   friend class TImpl;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
