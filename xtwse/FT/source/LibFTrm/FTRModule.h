//---------------------------------------------------------------------------
#ifndef RModuleH
#define RModuleH
//---------------------------------------------------------------------------
#include "FTRSesMgr.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace Bx{
namespace FT{
//---------------------------------------------------------------------------
K_class TRModule
{
private:
   typedef TRModule                  this_type;
   typedef Kway::TTimer<this_type>   TRTimer;
 
   TRSesMgr*               RSesMgr_;
   TRTimer*                Timer_;
   K_mf(void)              OnTimer     (TTimerBase*);
public:
   K_ctor     TRModule (void);
   K_dtor     ~TRModule();
};
//---------------------------------------------------------------------------
};
};
};
};
//---------------------------------------------------------------------------
#endif
