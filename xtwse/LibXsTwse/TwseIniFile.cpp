/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseIniFile.h"
#include "ExtDef.h"
#include "Nsa_UtilComm.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
//---------------------------------------------------------------------------
TTwseMainIni::SCont  TTwseMainIni::Cont_;

/////////////////////////////////////////////////////////////////////////////
// class TTwseMainIni impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseMainIni::TTwseMainIni ()
   : inherited()
{
   char filePath[256];

   // ./Settings/Main.ini
   sprintf(filePath, "%s/Main.ini", GetConfPath().c_str());
   inherited::FileRead(std::string(filePath));
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseMainIni::GetAppiaSvrPort ()
{
   std::string ident("AppiaMWPort");
   std::string port = inherited::ReadString(GetProcName(), ident, std::string());
   if (!port.empty())
      return port;
      
   return inherited::ReadString("Main", ident, std::string());
}
//---------------------------------------------------------------------------

K_mf(void) TTwseMainIni::InitCont (const std::string& sesId)
{
   SSecMain& main = Cont_.Main_;

   main.TmpFailCount_        = GetTmpFailCount       ();
   main.SocketRetryInterval_ = GetSocketRetryInterval();

   main.AppiaMwSesLogonItvl_    = GetAppiaMwSesLogonItvl   ();
   main.AppiaMwSesBfHbItvl_     = GetAppiaMwSesBfHbItvl    ();
   main.AppiaMwSesAfHbItvl_     = GetAppiaMwSesAfHbItvl    ();
   main.AppiaMwSesAfBrokenItvl_ = GetAppiaMwSesAfBrokenItvl();
   main.AppiaMwSesReopenItvl_   = GetAppiaMwSesReopenItvl  ();

   main.AutoFlushCoids_      = IsAutoFlushCoids     ();
   main.AutoFlushExecIDGene_ = IsAutoFlushExecIDGene();
   main.AutoFlushOrdCenter_  = IsAutoFlushOrdCenter ();
   main.AutoFlushOrdTrack_   = IsAutoFlushOrdTrack  ();
   main.AutoFlushRptCenter_  = IsAutoFlushRptCenter ();
   
   main.AppiaSvrIP_ = GetAppiaSvrIP();
   
   if (sesId.empty() == false)
      main.FixVer_ = GetFixVer(sesId);
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseAppendNoIni impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseAppendNoIni::TTwseAppendNoIni ()
{
   char filePath[256];

   // ./Settings/AppendNo/T5380X1.ini
   sprintf(filePath, "%s/AppendNo/%s.ini", GetConfPath().c_str(), GetProcName().c_str());
   inherited::FileRead(std::string(filePath));
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseAppendNoIni::IsRepeat (const std::string& appendNo)
{
   // [AppendNo]
   // ®æ¦¡: AppendNo,DateTime,FixMsgSeqNum
   // Item=001,2009/12/18 11:10:57.832,84
   // Item=002,2009/12/18 11:15:57.832,91
   // ...
   int itemSize = inherited::ReadStrings(std::string("AppendNo"), std::string("Item"), ItemList_);

   if (itemSize > 0) {
      std::string::size_type pos;
      std::string            str;
      
      for (TStrings::iterator iter=ItemList_.begin(); iter!=ItemList_.end(); ++iter) {
         pos = iter->find(',');
         if (pos == std::string::npos)  str = *iter;
         else                           str = iter->substr(0, pos);

         if (str == appendNo)
            return true;
      }
   }

   return false;
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseAppendNoIni::GetListStr () const
{
   std::string str;

   for (TStrings::const_iterator iter=ItemList_.begin(); iter!=ItemList_.end(); ++iter) {
      str.append(*iter);
      str.append(1, '_');
   }

   if (!str.empty())
      str.erase(str.length());
      
   return str;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseAppendNoIni::SetList (const std::string& appendNo, int fixMsgSeqNum)
{
   char buf[50];

   sprintf(buf, "%s,%s,%d", appendNo.c_str(), TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(), fixMsgSeqNum);

   inherited::AppendValue(std::string("AppendNo"), std::string("Item"), std::string(buf), 5);
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseDrIni impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseDrIni::TTwseDrIni ()
   : inherited()
{
   char filePath[256];
   
   sprintf(filePath, "%s/DR.ini", GetConfPath().c_str());
   inherited::FileRead(std::string(filePath));
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseDrInfoIni impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseDrInfoIni::TTwseDrInfoIni ()
   : inherited()
{
   char filePath[256];
   
   sprintf(filePath, "%s/DrInfo.ini", GetProcLogPath().c_str());
   inherited::FileRead(std::string(filePath));
}
//---------------------------------------------------------------------------
