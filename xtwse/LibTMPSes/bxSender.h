//---------------------------------------------------------------------------
#ifndef bxSenderH
#define bxSenderH
//---------------------------------------------------------------------------
#include "TimerThread.h"
//---------------------------------------------------------------------------
class TbxSender 
{
private:	
   typedef TbxSender       this_type;
protected:
   Kway::TTimer<this_type> Timer_;
public:	
           K_ctor          TbxSender();
           K_dtor          ~TbxSender();
   K_mf(void)              OnTimer         (Kway::TTimerBase*);
};
/////////////////////////////////////////////////////////////////////////////
#endif
