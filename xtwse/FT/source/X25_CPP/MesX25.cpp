//
// 通訊classes基礎: Session-Messager
//    class TMesX25;
//
// 版本: $Id: MesX25.cpp,v 1.3 2004/07/25 10:31:25 fonwin Exp $
//
// Setting format:
//    LineNo:PvcNo
//
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
/////////////////////////////////////////////////////////////////////////////
#include "MesX25.h"
#include "MesRegs.h"
#include <stdio.h>
#include "FileLog.h" 
//---------------------------------------------------------------------------
#if defined(SUNLINK)
#  include "MesX25_Stream.cpp"
#elif defined(__alpha)
#  include "MesX25_AlphaDEC.cpp"
#else
namespace Kway {
class MesX25Service
{
   TMesX25* Mes_;
public:
    MesX25Service  (TMesX25* mes, int lineNo, int pvcNo)  : Mes_(mes) {}
   ~MesX25Service  ()                                     {}

   bool  Start    ()                                  { Mes_->OnStateChanged(mslNormal, "Empty-X.25-Mes!");
                                                        return false; }
   void  Free     ()                                  { delete this;  }
   int   Send     (const void* buffer, size_t size)   { return -1;    }
   int   Recv     (      void* buffer, size_t size)   { return -1;    }
};
}
#endif
//---------------------------------------------------------------------------
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
K_mf(TMesReg&)  TMesX25::GetReg  ()
{
   struct Priv {
      static TMesPtr  MesFactory  ()  { return TMesPtr(new TMesX25());  }
   };
   static TMesReg  MesReg  = { "X.25",  Priv::MesFactory, false, 0, 0 };
   return MesReg;
}
/////////////////////////////////////////////////////////////////////////////
K_ctor  TMesX25::TMesX25  ()
   : LineNo_(0)
   , PvcNo_(0)
   , Service_(0)
{
  appLog.log("Create MesX25"); 
}
//---------------------------------------------------------------------------
K_dtor TMesX25::~TMesX25  ()
{
   appLog.log("Destory MesX25"); 
   Service_->Free();
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesX25::SettingApply  (const std::string& str)
{
   const char* spl = strchr(str.c_str(), ':');
   if(!spl)
      return false;
   LineNo_ = static_cast<word>(atoi(str.c_str()));
   PvcNo_  = static_cast<word>(atoi(spl+1));
   return true;
}
//---------------------------------------------------------------------------
K_mf(std::string)  TMesX25::GetSetting  ()
{
   char  buf[128];
   sprintf(buf, "%d:%d", (int)LineNo_, (int)PvcNo_);
   return std::string(buf);
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesX25::OpenImpl  ()
{
   appLog.log("MesX25 OpenImpl()"); 
   CloseImpl();
   Service_ = new MesX25Service(this, LineNo_, PvcNo_);
   if(Service_->Start()) {
      appLog.log("MesX25 OpenImpl()-> return true"); 
      return true;
   }
   CloseImpl();
   appLog.log("MesX25 OpenImpl()-> return false"); 
   return false;
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesX25::CloseImpl  ()
{
   appLog.log("MesX25 CloseImpl()"); 
   Service_->Free();
   Service_ = 0;
   return true;
}
//---------------------------------------------------------------------------
K_mf(int)  TMesX25::Send  (const void* buffer, size_t size)
{
   return Service_ ? Service_->Send(buffer, size) : -1;
}
//---------------------------------------------------------------------------
K_mf(int)  TMesX25::Recv  (void* buffer, size_t size)
{
   return Service_ ? Service_->Recv(buffer, size) : -1;
}
//---------------------------------------------------------------------------

