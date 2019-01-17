//---------------------------------------------------------------------------
#ifndef TwseH
#define TwseH
//---------------------------------------------------------------------------
#include "DspDef.h"
#include "TimerThread.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TTwseDispatch : public TFileDispatch
{
public:
        K_ctor  TTwseDispatch() {}
virtual K_dtor ~TTwseDispatch() {}
virtual K_mf(void) DoDispatch(const TFTJob&, const std::string& fname) const;
};
//---------------------------------------------------------------------------
K_class TTwseFileGet : public TFileGet
{
public:
        K_ctor  TTwseFileGet() {}
virtual K_dtor ~TTwseFileGet() {}
virtual K_mf(bool) GetSource (const TFTJob&, const std::string& src, const std::string& dst) const;
};
//---------------------------------------------------------------------------
K_class TTwseLmModTimer
{
   typedef TTwseLmModTimer this_type;
private:
   TTimer<this_type>	Timer_;
   K_mf(void)   	OnTimer(TTimerBase*);
public:
   	K_ctor  TTwseLmModTimer();
virtual K_dtor ~TTwseLmModTimer();
};
//--------------------------------------------------------------------------- 
K_exportC(void*)  nsModuleStart  ();
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
