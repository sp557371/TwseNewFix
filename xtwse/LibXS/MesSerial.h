//
// 通訊classes基礎: Session-Messager
//    class TMesSerial;    // RS232
//
// 版本: $Id: MesSerial.h,v 1.3 2005/04/28 05:52:45 fonwin Exp $
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
   //    deviceName: LINUX: /dev/ttyd0、/dev/ttyd1...
   //                   或: /dev/ttyS0、/dev/ttyS1...
   //                WIN:   COM1、COM2... 或 \\.\COM1
   //    baud:       1200、2400、9600、14400、19200、38400、56000、57600、
   //                115200、128000、256000
   //    parity:     N=None, E=Even, O=Odd
   //    databits:   6、7、8
   //    stopbits:   1、2
   //    flowctrl:   N=None, H=Hardware(RTS/CTS,DTR/DSR), S=Software(XON/XOFF)
   // ex:   COM1:56000,N,8,1,H
   K_mf(bool)        SettingApply (const std::string& str);
   K_mf(std::string) GetSetting   ();
   //基本操作
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

