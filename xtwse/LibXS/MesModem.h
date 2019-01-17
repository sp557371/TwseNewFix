//
// 通訊classes基礎: Session-Messager
//    class MesModem;
//
// 版本: $Id: MesModem.h,v 1.1 2004/10/13 15:33:00 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesModem_H__
#define __MesModem_H__
//---------------------------------------------------------------------------
#include "MesSerial.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class MesModemImpl;
//---------------------------------------------------------------------------
LIBNS_class TMesModem : public TMesSerial
{
   typedef TMesSerial   inherited;

   std::string    ModemIniFileName_;
   std::string    PhoneNum_;
   K_mf(MesSerialImpl*)  CreateImpl  ();
public:
   K_ctor  TMesModem  ();
   K_dtor ~TMesModem  ();

   // format: deviceName:baud,parity,databits,stopbits,flowctrl;modem-ini-file;dial-number
   //    deviceName: LINUX: /dev/ttyd0、/dev/ttyd1...
   //                WIN:   COM1、COM2... 或 \\.\COM1
   //    baud:       1200、2400、9600、14400、19200、38400、56000、57600、
   //                115200、128000、256000
   //    parity:     N=None, E=Even, O=Odd
   //    databits:   6、7、8
   //    stopbits:   1、2
   //    flowctrl:   N=None, H=Hardware(RTS/CTS,DTR/DSR), S=Software(XON/XOFF)
   // dial-number:   atdt12345678\r          for dialup mode
   //          or:   ats0=1\r                for answer mode
   // ex:   COM1:56000,N,8,1,H
   K_mf(bool)        SettingApply (const std::string& str);
   K_mf(std::string) GetSetting   ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

