////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Twse2KwayFiles.h"
//---------------------------------------------------------------------------
using namespace Kstl;
using namespace Kway;
//---------------------------------------------------------------------------
static const char  fillerC = ' '; 
static const char  filler0 = '0';
//---------------------------------------------------------------------------
K_mf(void)  Kway::FillChar(std::string& str, int size, char c)
{
   std::string::size_type  len = str.length();
   std::string::size_type  dev = size-len;
   if(dev > 0) {
      if(c == filler0) {
         std::string  tmp;
         tmp.append(dev, c);
         str = tmp + str;
      } else {
         str.append(dev, c);
      }
   }
}
//---------------------------------------------------------------------------
K_mf(std::string)  SF03Data1::ToString()
{
   std::string  date = Date_.as_string();
   std::string  cnt  = Cnt_.as_string();
   std::string  F146  = Filler1_.as_string();
   
   FillChar(date, sizeof(Date_), fillerC);
   FillChar(cnt, sizeof(Cnt_), filler0);
   FillChar(F146, sizeof(Filler1_), fillerC);
   return std::string(date+cnt+F146);
}
//---------------------------------------------------------------------------
K_mf(std::string)  SF03Data2::ToString()
{
   std::string wk, line, srcPort, mkt, pvcName, brk, pvcID, apc, pwd, type, ver, ip1, ip2;
   std::string fwgroup, appgroup, destIP, ListP1, ListP2, fu, tf, effD, f44;

   wk       = WorkCode_.as_string();
   line     = LineNo_.as_string();
   srcPort  = SrcPort_.as_string();
   mkt      = Market_.as_string();
   pvcName  = PVCName_.as_string();
   brk      = Broker_.as_string();
   pvcID    = PVCID_.as_string();
   apc      = APCode_.as_string();
   pwd      = Passwd_.as_string();
   type     = Type_.as_string();
   ver      = FixVersion_.as_string();
   ip1      = IP1_.as_string();
   ip2      = IP2_.as_string();
   fwgroup  = FWGroup_.as_string();
   appgroup = AppiaGroup_.as_string();
   destIP   = DestIP_.as_string();
   ListP1   = ListPort1_.as_string();
   ListP2   = ListPort2_.as_string();
   fu       = FlowUnit_.as_string();
   tf       = TotalFlow_.as_string();
   effD     = EffDate_.as_string();
   f44      = Filler2_.as_string();
   
   FillChar(wk      , sizeof(WorkCode_)  , filler0);
   FillChar(line    , sizeof(LineNo_)    , fillerC);
   FillChar(srcPort , sizeof(SrcPort_)   , filler0);
   FillChar(mkt     , sizeof(Market_)    , fillerC);
   FillChar(pvcName , sizeof(PVCName_)   , fillerC);
   FillChar(brk     , sizeof(Broker_)    , fillerC);
   FillChar(pvcID   , sizeof(PVCID_)     , fillerC);
   FillChar(apc     , sizeof(APCode_)    , fillerC);
   FillChar(pwd     , sizeof(Passwd_)    , filler0);
   FillChar(type    , sizeof(Type_)      , fillerC);
   FillChar(ver     , sizeof(FixVersion_), fillerC);
   FillChar(ip1     , sizeof(IP1_)       , fillerC);
   FillChar(ip2     , sizeof(IP2_)       , fillerC);
   FillChar(fwgroup , sizeof(FWGroup_)   , fillerC);
   FillChar(appgroup, sizeof(AppiaGroup_), fillerC);
   FillChar(destIP  , sizeof(DestIP_)    , fillerC);
   FillChar(ListP1  , sizeof(ListPort1_) , filler0);
   FillChar(ListP2  , sizeof(ListPort2_) , filler0);
   FillChar(fu      , sizeof(FlowUnit_)  , filler0);
   FillChar(tf      , sizeof(TotalFlow_) , filler0);
   FillChar(effD    , sizeof(EffDate_)   , filler0);
   FillChar(f44     , sizeof(Filler2_)   , fillerC);

   return std::string(wk+line+srcPort+mkt+pvcName+brk+pvcID+apc+pwd+type+
      ver+ip1+ip2+fwgroup+appgroup+destIP+ListP1+ListP2+fu+tf+effD+f44);
}
//---------------------------------------------------------------------------
K_mf(void)  SF03Data2::Clear()
{
   WorkCode_.clear();
   LineNo_.clear();
   SrcPort_.clear();
   Market_.clear();
   PVCName_.clear();
   Broker_.clear();
   PVCID_.clear();
   APCode_.clear();
   Passwd_.clear();
   Type_.clear();
   FixVersion_.clear();
   IP1_.clear();
   IP2_.clear();
   FWGroup_.clear();
   AppiaGroup_.clear();
   DestIP_.clear();
   ListPort1_.clear();
   ListPort2_.clear();
   FlowUnit_.clear();
   TotalFlow_.clear();
   EffDate_.clear();
   Filler2_.clear();
}
//---------------------------------------------------------------------------
static const char  F01[] = "F01";
//static const char  K02[] = "K02";
static const char  F03[] = "F03";
static const char  F04[] = "F04";
static const char  F05[] = "F05";
static const char  F06[] = "F06";
static const char  F07[] = "F07";
static const char  F08[] = "F08";
//static const char  K09[] = "K09";
static const char  F11[] = "F11";
//static const char  F12[] = "F12";
//static const char  F13[] = "F13";
static const char  F14[] = "F14";
static const char  F15[] = "F15";
static const char  F16[] = "F16";
static const char  F17[] = "F17";
//---------------------------------------------------------------------------
K_mf(std::string)  TFileType::as_string() const
{
   switch(Data_) {
   case ft_F01:   return std::string(F01);
   //case ft_K02:   return std::string(K02);
   case ft_F03:   return std::string(F03);
   case ft_F04:   return std::string(F04);
   case ft_F05:   return std::string(F05);
   case ft_F06:   return std::string(F06);
   case ft_F07:   return std::string(F07);
   case ft_F08:   return std::string(F08);
   //case ft_K09:   return std::string(K09);
   case ft_F11:   return std::string(F11);
   //case ft_F12:   return std::string(F12);
   //case ft_F13:   return std::string(F13);
   case ft_F14:   return std::string(F14);
   case ft_F15:   return std::string(F15);
   case ft_F16:   return std::string(F16);
   case ft_F17:   return std::string(F17);
   default:       return std::string("NONE");
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TFileType::assign(const std::string& s)
{
   if(s == std::string(F01)) 
      Data_ = ft_F01;
/*      
   else if(s == std::string(K02)) 
      Data_ = ft_K02;
*/      
   else if(s == std::string(F03)) 
      Data_ = ft_F03;
   else if(s == std::string(F04)) 
      Data_ = ft_F04;
   else if(s == std::string(F05)) 
      Data_ = ft_F05;
   else if(s == std::string(F06)) 
      Data_ = ft_F06;
   else if(s == std::string(F07)) 
      Data_ = ft_F07;
   else if(s == std::string(F08)) 
      Data_ = ft_F08;
/*      
   else if(s == std::string(K09))
      Data_ = ft_K09;
*/      
   else if(s == std::string(F11))
      Data_ = ft_F11;
/*      
   else if(s == std::string(F12))
      Data_ = ft_F12;
*/      
/*      
   else if(s == std::string(F13))
      Data_ = ft_F13;
*/      
   else if(s == std::string(F14))
      Data_ = ft_F14;
   else if(s == std::string(F15))
      Data_ = ft_F15;
   else if(s == std::string(F16))
      Data_ = ft_F16;
   else if(s == std::string(F17))
      Data_ = ft_F17;
   else
      Data_ = ft_None; 
}
//---------------------------------------------------------------------------
K_mf(int)  TFileType::GetDataSize(int i)
{
   switch(Data_) {
   case ft_F01: {   
	   SF01  tmp;
	   return tmp.GetDataSize(i);
	}
/*	
   case ft_K02: {
	   SK02  tmp;
	   return tmp.GetDataSize(i);
	}
*/	
   case ft_F03: {
	   SF03  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_F04: {
	   SF04  tmp;
	   return tmp.GetDataSize(i);
	   }
   case ft_F05: {
	   SF05  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_F06: {
	   SF06  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_F07: {
	   SF07  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_F08: {
	   SF08  tmp;
	   return tmp.GetDataSize(i);
	}
/*	
   case ft_K09: {
	   SK09  tmp;
	   return tmp.GetDataSize(i);
	}
*/	
   case ft_F11: {
      SF11  tmp;
      return tmp.GetDataSize(i);
   }
/*   
   case ft_F12: {
      SF12  tmp;
      return tmp.GetDataSize(i);
   }
*/   
/*   
   case ft_F13: {
      SF13  tmp;
      return tmp.GetDataSize(i);
   }
*/   
   case ft_F14: {
      SF14  tmp;
      return tmp.GetDataSize(i);
   }
   case ft_F15: {
      SF15  tmp;
      return tmp.GetDataSize(i);
   }
   case ft_F16: {
      SF16  tmp;
      return tmp.GetDataSize(i);
   }
   case ft_F17: {
      SF17  tmp;
      return tmp.GetDataSize(i);
   }
   }
   return 0;
}
//---------------------------------------------------------------------------
/*
K_mf(std::string)  SK09::ToString()
{
   std::string  brk, pvc, apc, time, filler;
   brk = Broker_.as_string();
   pvc = FixPVCID_.as_string();
   apc = APCode_.as_string();
   time   = TransTime_.as_string();
   filler = Filler_.as_string();
   
   FillChar(brk, sizeof(Broker_), fillerC);
   FillChar(pvc, sizeof(FixPVCID_), fillerC);
   FillChar(apc, sizeof(APCode_), fillerC);
   FillChar(time, sizeof(TransTime_), filler0);
   FillChar(filler, sizeof(Filler_), fillerC);
   
   return std::string(brk+pvc+apc+time+filler);
}
*/
//---------------------------------------------------------------------------
// --> 20180806 add by k288 for 測試線路與正式線路發生重覆
K_mf(std::string)  SF05Data2::ToString()
{
   std::string pvcName   = PVCName_.as_string();
   std::string brk       = Broker_.as_string();
   std::string apc       = APCode_.as_string();
   std::string pvcID     = PVCID_.as_string();
   std::string pwd       = Passwd_.as_string();
   std::string fwgroup   = FWGroup_.as_string();
   std::string appgroup  = AppiaGroup_.as_string();
   std::string smgroup   = SMGroup_.as_string();
   std::string type      = Type_.as_string();
   std::string ver       = FixVersion_.as_string();
   std::string ip1       = IP1_.as_string();
   std::string ip2       = IP2_.as_string();
   std::string srcPort   = SrcPort_.as_string();
   std::string destIP    = DestIP_.as_string();
   std::string listPort1 = ListPort1_.as_string();
   std::string listPort2 = ListPort2_.as_string();
   std::string totalFlow = TotalFlow_.as_string();
   std::string f64       = Filler2_.as_string();
   
   FillChar(pvcName  , sizeof(PVCName_)   , fillerC);
   FillChar(brk      , sizeof(Broker_)    , fillerC);
   FillChar(apc      , sizeof(APCode_)    , fillerC);
   FillChar(pvcID    , sizeof(PVCID_)     , fillerC);
   FillChar(pwd      , sizeof(Passwd_)    , filler0);
   FillChar(fwgroup  , sizeof(FWGroup_)   , fillerC);
   FillChar(appgroup , sizeof(AppiaGroup_), fillerC);
   FillChar(smgroup  , sizeof(SMGroup_)   , filler0);
   FillChar(type     , sizeof(Type_)      , fillerC);
   FillChar(ver      , sizeof(FixVersion_), fillerC);
   FillChar(ip1      , sizeof(IP1_)       , fillerC);
   FillChar(ip2      , sizeof(IP2_)       , fillerC);
   FillChar(srcPort  , sizeof(SrcPort_)   , filler0);
   FillChar(destIP   , sizeof(DestIP_)    , fillerC);
   FillChar(listPort1, sizeof(ListPort1_) , filler0);
   FillChar(listPort2, sizeof(ListPort2_) , filler0);
   FillChar(totalFlow, sizeof(TotalFlow_) , filler0);
   FillChar(f64      , sizeof(Filler2_)   , fillerC);

   return std::string(pvcName+brk+apc+pvcID+pwd+fwgroup+appgroup+smgroup+
      type+ver+ip1+ip2+srcPort+destIP+listPort1+listPort2+totalFlow+f64);
}
// <-- 20180806 add by k288 for 測試線路與正式線路發生重覆
//---------------------------------------------------------------------------
K_mf(std::string)  SF10Data1::ToString()
{
   std::string  date = Date_.as_string();
   std::string  F92  = Filler1_.as_string();
   
   FillChar(date, sizeof(Date_), filler0);
   FillChar(F92, sizeof(Filler1_), fillerC);
   return std::string(date+F92);
}
//---------------------------------------------------------------------------
K_mf(std::string)  SF10Data2::ToString()
{
   std::string pvcName   = PVCName_.as_string();
   std::string brk       = Broker_.as_string();
   std::string apc       = APCode_.as_string();
   std::string pvcID     = PVCID_.as_string();
   std::string pwd       = Passwd_.as_string();
   std::string fwgroup   = FWGroup_.as_string();
   std::string appgroup  = AppiaGroup_.as_string();
   std::string smgroup   = SMGroup_.as_string();
   std::string type      = Type_.as_string();
   std::string ver       = FixVersion_.as_string();
   std::string ip1       = IP1_.as_string();
   std::string ip2       = IP2_.as_string();
   std::string srcPort   = SrcPort_.as_string();
   std::string destIP    = DestIP_.as_string();
   std::string listPort1 = ListPort1_.as_string();
   std::string listPort2 = ListPort2_.as_string();
   std::string totalFlow = TotalFlow_.as_string();
   std::string f64       = Filler2_.as_string();
   
   FillChar(pvcName  , sizeof(PVCName_)   , fillerC);
   FillChar(brk      , sizeof(Broker_)    , fillerC);
   FillChar(apc      , sizeof(APCode_)    , fillerC);
   FillChar(pvcID    , sizeof(PVCID_)     , fillerC);
   FillChar(pwd      , sizeof(Passwd_)    , filler0);
   FillChar(fwgroup  , sizeof(FWGroup_)   , fillerC);
   FillChar(appgroup , sizeof(AppiaGroup_), fillerC);
   FillChar(smgroup  , sizeof(SMGroup_)   , filler0);
   FillChar(type     , sizeof(Type_)      , fillerC);
   FillChar(ver      , sizeof(FixVersion_), fillerC);
   FillChar(ip1      , sizeof(IP1_)       , fillerC);
   FillChar(ip2      , sizeof(IP2_)       , fillerC);
   FillChar(srcPort  , sizeof(SrcPort_)   , filler0);
   FillChar(destIP   , sizeof(DestIP_)    , fillerC);
   FillChar(listPort1, sizeof(ListPort1_) , filler0);
   FillChar(listPort2, sizeof(ListPort2_) , filler0);
   FillChar(totalFlow, sizeof(TotalFlow_) , filler0);
   FillChar(f64      , sizeof(Filler2_)   , fillerC);

   return std::string(pvcName+brk+apc+pvcID+pwd+fwgroup+appgroup+smgroup+
      type+ver+ip1+ip2+srcPort+destIP+listPort1+listPort2+totalFlow+f64);
}
//---------------------------------------------------------------------------
K_mf(void)  SF10Data2::Clear()
{
   PVCName_.clear();
   Broker_.clear();
   APCode_.clear();
   PVCID_.clear();
   Passwd_.clear();
   FWGroup_.clear();
   AppiaGroup_.clear();
   SMGroup_.clear();
   Type_.clear();
   FixVersion_.clear();
   IP1_.clear();
   IP2_.clear();
   SrcPort_.clear();
   DestIP_.clear();
   ListPort1_.clear();
   ListPort2_.clear();
   TotalFlow_.clear();
   Filler2_.clear();
}
//---------------------------------------------------------------------------
