//---------------------------------------------------------------------------
// class TMesLoopback; �⦬�쪺��ƶǦ^�h,
//    setting format:
//      n[,c][,i=init-string]
//    �̦h�� n bytes ��N��ƶǦ^, �Φ���(cr)�ɱN��ƶǦ^
//    StatStr = �ǰe����/�C��
//
// $Id: MesLoopback.h,v 1.3 2005/04/28 05:52:26 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesLoopbackH
#define MesLoopbackH
//---------------------------------------------------------------------------
#include "MesRegs.h"
#include "KTime.h"
#include "TimerThread.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class TMesLoopback : public TMesBase
{
   typedef TMesBase        inherited;
   typedef TMesLoopback    this_type;
public:
   K_ctor            TMesLoopback ();
   K_mf(bool)        SettingApply (const std::string&);
   K_mf(std::string) GetSetting   ();
   K_mf(int)         Send         (const void*, size_t);
   K_mf(int)         Recv         (      void*, size_t);

   static K_mf(TMesReg&)  GetReg  ();
protected:
   K_mf(bool)  OpenImpl  ();
   K_mf(bool)  CloseImpl ();

private:
   size_t         Counter_;
   TKTime         CounterTime_;
   size_t         BufSize_;
   bool           WaitCR_;
   std::string    Buffer_;
   std::string    InitStr_;
   std::string    StatStr_;

   TTimer<this_type> Timer_;
   dword             Interval_;
   virtual K_mf(void)  OnTimerEv   (TTimerBase*);
           K_mf(void)  SendInitStr ();
};
/////////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------------------------
#endif
