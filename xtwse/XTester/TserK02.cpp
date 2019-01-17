/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TserK02.h"
#include "KTime.h"
#include "ExtDef.h"
#include "ApaSmpMsg.h"
#include "TwseType.h"
#include "TserIniFile.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTserK02 impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTserK02::TTserK02 ()
{
   char filePath[256];
   
   sprintf(filePath, "%sF02", GetProcLogPath().c_str());

   File_.Open(filePath, fmAppend);
}
//---------------------------------------------------------------------------

K_mf(void) TTserK02::AddRec (TTserIni&                     iniTser,
                             const TSmpString&             strTandSesID,
                             ETwseFixRejectCode            code,
                             int                           pwd,
                             const SSmpMsgMiddlewareEvent& smpData,
                             int                           fixVer)
{
   STserK02Data2  data;
   char           buf[128];
   TTwseSesIDTand twseSesIDTand(strTandSesID);
   
   // BkTestNo
   data.BkTestNo_.assign(iniTser.GetTestNo(strTandSesID));
   
   // BkPvcNo
   data.BkPvcNo_.assign(iniTser.GetSourcePort(strTandSesID));
   
   // MarketKind
   data.MarketKind_ = twseSesIDTand.GetMarket();
   
    // BrokerNo
   data.BrokerNo_.assign(twseSesIDTand.GetSesID().as_string());
   
   //PVCID
   data.PVCID_.assign(twseSesIDTand.GetSocketID().as_string());

   //APCode
   data.APCode_.assign(iniTser.GetAPCode(strTandSesID));

   // Password
   data.Password_.assign(pwd);
   
   // Type (©T©w­È 'N')
   data.Type_.assign("N");
   
   // FIXVersion
   if (fixVer == 42)  
      data.FixVersion_.assign("1");
   else if (fixVer == 44)  
      data.FixVersion_.assign("2");
   
   // IP1 & IP2
   data.IP1_.assign(iniTser.GetSourceIP1(strTandSesID));
   data.IP2_.assign(iniTser.GetSourceIP2(strTandSesID));
   
   // FwdGrp
   data.FwdGrp_.assign(iniTser.GetFwdGrp(strTandSesID));

   // AppGrp
   data.AppGrp_.assign(iniTser.GetAppGrp(strTandSesID));

   // DestIP
   data.DestIP_.assign(iniTser.GetDestIP(strTandSesID));

   // ListenPort1 & ListenPort2
   data.ListPort1_.assign(iniTser.GetListenPort1(strTandSesID));
   data.ListPort2_.assign(iniTser.GetListenPort2(strTandSesID));

   // ApplySeqNo
   data.ApplySeqNo_.assign(iniTser.GetApplySeqNo(strTandSesID));
   
   // ApplyDate
   data.ApplyDate_.assign(iniTser.GetApplyDate(strTandSesID));
   


   // CompleteDate, CompleteTime, Result
   if (iniTser.IsTestOK(strTandSesID)) {
      std::string str = TKDate::Now().as_string(TKDate::sf_ELong, 0);
         
      data.CompleteDate_.assign(str);
      data.CompleteTime_.assign(TKTime::Now().as_string(false));
      data.Result_      .clear ();
   }
   else {
      // ´ú¸Õ¥¢±Ñ
      data.CompleteDate_.clear ();
      data.CompleteTime_.clear ();
      if (code == tfrc_TradeSuspended)
         data.Result_   .clear();
      else
         data.Result_   .assign(atoi(TTwseRes::KeyValueError));
   }
   
   File_.Write(&data, sizeof(data));
}
//---------------------------------------------------------------------------
