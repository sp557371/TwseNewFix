////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Twse2KwayFiles.h"
//---------------------------------------------------------------------------
using namespace Kstl;
using namespace Kway;//::Tw::Bx::FT;
//---------------------------------------------------------------------------
static const char K01[] = "K01";
static const char K02[] = "K02";
static const char K03[] = "K03";
static const char K04[] = "K04";
static const char K05[] = "K05";
static const char K06[] = "K06";
static const char K07[] = "K07";
static const char K08[] = "K08";
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
   }
   return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
