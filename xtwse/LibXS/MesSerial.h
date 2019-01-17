//
// �q�Tclasses��¦: Session-Messager
//    class TMesSerial;    // RS232
//
// ����: $Id: MesSerial.h,v 1.3 2005/04/28 05:52:45 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesSERIAL_H__
#define __MesSERIAL_H__
//---------------------------------------------------------------------------
#include <cc++/serial.h>
#include "Mes.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class MesSerialImpl;
//---------------------------------------------------------------------------
LIBNS_class TMesSerial : public TMesBase
{
	friend class MesSerialImpl;
public:
   K_ctor  TMesSerial  ();
   K_dtor ~TMesSerial  ();

   // format: deviceName:baud,parity,databits,stopbits,flowctrl
   //    deviceName: LINUX: /dev/ttyd0�B/dev/ttyd1...
   //                   ��: /dev/ttyS0�B/dev/ttyS1...
   //                WIN:   COM1�BCOM2... �� \\.\COM1
   //    baud:       1200�B2400�B9600�B14400�B19200�B38400�B56000�B57600�B
   //                115200�B128000�B256000
   //    parity:     N=None, E=Even, O=Odd
   //    databits:   6�B7�B8
   //    stopbits:   1�B2
   //    flowctrl:   N=None, H=Hardware(RTS/CTS,DTR/DSR), S=Software(XON/XOFF)
   // ex:   COM1:56000,N,8,1,H
   K_mf(bool)        SettingApply (const std::string& str);
   K_mf(std::string) GetSetting   ();
   //�򥻾ާ@
   K_mf(int)         Send         (const void *buffer, size_t size);
   K_mf(int)         Recv         (      void *buffer, size_t size);

protected:
   K_mf(bool)        OpenImpl     ();
   K_mf(bool)        CloseImpl    ();

private:
   virtual K_mf(MesSerialImpl*)  CreateImpl  ();

   typedef ost::Serial::Parity   TParity;
   typedef ost::Serial::Flow     TFlowCtrl;
   MesSerialImpl* Impl_;
	std::string    DeviceName_;
	dword          BaudRate_;
	TParity        Parity_;
	byte           CharBits_;
	byte           StopBits_;
	TFlowCtrl      FlowCtrl_;
   std::string    ErrMsg_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

