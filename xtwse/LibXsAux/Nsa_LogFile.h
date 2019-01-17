/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaLogFile_h_)
#define _NsaLogFile_h_
//---------------------------------------------------------------------------
#include "KFile.h"
#include "Nsa_LineBuf.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TKDateTimeMS;

namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// enum ELogType
/////////////////////////////////////////////////////////////////////////////
enum ELogType
{
   elt_Info,
   elt_Warning,
   elt_Error,
   //elt_ExtMsg,

   elt_Send,
   elt_Recv,
   elt_MesIn,
   elt_Debug,
};

/////////////////////////////////////////////////////////////////////////////
// class TLogMsg
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TLogMsg : public TLineBufNew
{
   typedef TLineBufNew  inherited;

public:
   inline K_ctor TLogMsg (TLineBufNew::size_type allocSize = 1024)
      : inherited(allocSize)
   {}
   
   inline K_ctor TLogMsg (ELogType logType, const void* data, size_t size,
                          const TKDateTimeMS*    dtms      = NULL,
                          TLineBufNew::size_type allocSize = 1024)
      : inherited(allocSize)
   {
      Put(logType, data, size, dtms, true);
   }

   void PutStr (ELogType, const char* fmt, ...);

   inline K_mf(void) PutMem (ELogType logType, const void* data, size_t size, const TKDateTimeMS* dtms, bool incNewLine)
      {
         Put(logType, data, size, dtms, incNewLine);
      }
  
   inline K_mf(void) Append (const void* ptr, int size, bool incNewLine)
      {
         inherited::Append(ptr, size);
         if (incNewLine)
            inherited::AppendNewLine();
      }

private:
   K_mf(void) Put (ELogType, const void* data, size_t size, const TKDateTimeMS* dtms, bool incNewLine);
};

/////////////////////////////////////////////////////////////////////////////
// class TLogFile
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TLogFile
{
public:
   inline K_ctor TLogFile ()  {}
          K_ctor TLogFile (const char*);
   
   // Open
   K_mf(bool) Open      (const char*);
   K_mf(bool) OpenDaily (const char* fileName, const char* subDir = NULL);

   // LogStr
   void LogStr (ELogType,                      const char* fmt, ...);
   void LogStr (ELogType, const TKDateTimeMS&, const char* fmt, ...);
   
   // LogSize
   K_mf(void) LogSize (ELogType mtype, const void* data, size_t size);

   inline K_mf(bool)        Close       ()        { return File_.Close();       }
   inline K_mf(bool)        IsOpened    () const  { return File_.IsOpened();    }
   inline K_mf(std::string) GetFileName () const  { return File_.GetFileName(); }
   
   static K_mf(TLogFile&) GetInst ();

private:
   K_mf(void) WriteLog (ELogType mtype, const void* data, size_t size, const char* strExt, const TKDateTimeMS*);

private:
   Kway::TFile  File_;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaLogFile_h_)
