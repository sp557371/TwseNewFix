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
K_mf(std::string)  SK03Data1::ToString()
{
   std::string  date = Date_.as_string();
   std::string  F92  = Filler1_.as_string();
   
   FillChar(date, sizeof(Date_), fillerC);
   FillChar(F92, sizeof(Filler1_), fillerC);
   return std::string(date+F92);
}
//---------------------------------------------------------------------------
K_mf(std::string)  SK03Data2::ToString()
{
   //std::string  wk, type, pvc, brk, mkt, fixpvc, tmppvc, apc, effD, line, pwd, srcP, ip, f32;
   std::string  wk, type, pvc, brk, mkt, fixpvc, tmppvc, apc, effD, line, pwd, srcP, ip, ver, f31;
   wk = WorkCode_.as_string();
   type = Type_.as_string();
   pvc  = PVCName_.as_string();
   brk  = Broker_.as_string();
   mkt  = Market_.as_string();
   fixpvc = FixPVCID_.as_string();
   tmppvc = TmpPVCID_.as_string();
   apc  = APCode_.as_string();
   effD = EffDate_.as_string();
   line = LineNo_.as_string();
   pwd  = Passwd_.as_string();
   srcP = SrcPort_.as_string();
   ip   = IP_.as_string();
   //f32 = Filler2_.as_string();
   ver = FixVersion_.as_string();
   f31 = Filler2_.as_string();
   
   FillChar(wk, sizeof(WorkCode_), filler0);
   FillChar(type, sizeof(Type_), fillerC);
   FillChar(pvc, sizeof(PVCName_), fillerC);
   FillChar(brk, sizeof(Broker_), fillerC);
   FillChar(mkt, sizeof(Market_), fillerC);
   FillChar(fixpvc, sizeof(FixPVCID_), fillerC);
   FillChar(tmppvc, sizeof(TmpPVCID_), fillerC);
   FillChar(apc, sizeof(APCode_), fillerC);
   FillChar(effD, sizeof(EffDate_), filler0);
   FillChar(line, sizeof(LineNo_), fillerC);
   FillChar(pwd, sizeof(Passwd_), filler0);
   FillChar(srcP, sizeof(SrcPort_), filler0);
   FillChar(ip, sizeof(IP_), fillerC);
   FillChar(ver, sizeof(FixVersion_), fillerC);
   FillChar(f31, sizeof(Filler2_), fillerC);
   //FillChar(f32, sizeof(Filler2_), fillerC);
   
   return std::string(wk+type+pvc+brk+mkt+fixpvc+tmppvc+apc+effD+line+pwd+srcP+ip+ver+f31);
   //return std::string(wk+type+pvc+brk+mkt+fixpvc+tmppvc+apc+effD+line+pwd+srcP+ip+f32);
}
//---------------------------------------------------------------------------
K_mf(void)  SK03Data2::Clear()
{
   WorkCode_.clear();
   Type_.clear();
   PVCName_.clear();
   Broker_.clear();
   Market_.clear();
   FixPVCID_.clear();
   TmpPVCID_.clear();
   APCode_.clear();
   EffDate_.clear();
   LineNo_.clear();
   Passwd_.clear();
   SrcPort_.clear();
   IP_.clear();
   FixVersion_.clear();
   Filler2_.clear();
}
//---------------------------------------------------------------------------
static const char  K01[] = "K01";
static const char  K02[] = "K02";
static const char  K03[] = "K03";
static const char  K04[] = "K04";
static const char  K05[] = "K05";
static const char  K06[] = "K06";
static const char  K07[] = "K07";
static const char  K08[] = "K08";
static const char  K09[] = "K09";
static const char  K11[] = "K11";
static const char  K12[] = "K12";
static const char  K13[] = "K13";
//---------------------------------------------------------------------------
K_mf(std::string)  TFileType::as_string() const
{
   switch(Data_) {
   case ft_K01:   return std::string(K01);
   case ft_K02:   return std::string(K02);
   case ft_K03:   return std::string(K03);
   case ft_K04:   return std::string(K04);
   case ft_K05:   return std::string(K05);
   case ft_K06:   return std::string(K06);
   case ft_K07:   return std::string(K07);
   case ft_K08:   return std::string(K08);
   case ft_K09:   return std::string(K09);
   case ft_K11:   return std::string(K11);
   case ft_K12:   return std::string(K12);
   case ft_K13:   return std::string(K13);
   default:       return std::string("NONE");
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TFileType::assign(const std::string& s)
{
   if(s == std::string(K01)) 
      Data_ = ft_K01;
   else if(s == std::string(K02)) 
      Data_ = ft_K02;
   else if(s == std::string(K03)) 
      Data_ = ft_K03;
   else if(s == std::string(K04)) 
      Data_ = ft_K04;
   else if(s == std::string(K05)) 
      Data_ = ft_K05;
   else if(s == std::string(K06)) 
      Data_ = ft_K06;
   else if(s == std::string(K07)) 
      Data_ = ft_K07;
   else if(s == std::string(K08)) 
      Data_ = ft_K08;
   else if(s == std::string(K09))
      Data_ = ft_K09;
   else if(s == std::string(K11))
      Data_ = ft_K11;
   else if(s == std::string(K12))
      Data_ = ft_K12;
   else if(s == std::string(K13))
      Data_ = ft_K13;
   else
      Data_ = ft_None; 
}
//---------------------------------------------------------------------------
K_mf(int)  TFileType::GetDataSize(int i)
{
   switch(Data_) {
   case ft_K01: {   
	   SK01  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_K02: {
	   SK02  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_K03: {
	   SK03  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_K04: {
	   SK04  tmp;
	   return tmp.GetDataSize(i);
	   }
   case ft_K05: {
	   SK05  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_K06: {
	   SK06  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_K07: {
	   SK07  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_K08: {
	   SK08  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_K09: {
	   SK09  tmp;
	   return tmp.GetDataSize(i);
	}
   case ft_K11: {
      SK11  tmp;
      return tmp.GetDataSize(i);
   }
   case ft_K12: {
      SK12  tmp;
      return tmp.GetDataSize(i);
   }
   case ft_K13: {
      SK13  tmp;
      return tmp.GetDataSize(i);
   }
   }
   return 0;
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
