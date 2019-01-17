/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaSeqNoGene_h_)
#define _NsaSeqNoGene_h_
//---------------------------------------------------------------------------
#include <string>
#include "libnsAux.h"
//---------------------------------------------------------------------------
namespace ost {
   class Mutex;
};

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TFile;
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TSeqNoGene
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TSeqNoGene
{
   size_t       CurrSeqNo_;
   ost::Mutex*  Mutex_;
   TFile*       File_;
   bool         IsAppendMode_;
   bool         IsAutoFlush_;

public:
   K_ctor  TSeqNoGene (const char*, bool isAppendMode = true, bool isAutoFlush = false);
   K_dtor ~TSeqNoGene ();

   K_mf(size_t)      GetNewInt ();
   K_mf(std::string) GetNewStr (size_t len = 0, char prefiller = '0');
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaSeqNoGene_h_)
