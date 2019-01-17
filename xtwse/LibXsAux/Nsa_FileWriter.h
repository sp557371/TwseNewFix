/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_FileWriter_h_)
#define _Nsa_FileWriter_h_
//---------------------------------------------------------------------------
#include "MemPool.h"
#include "wise_ptr.h"
#include "ev_map.h"
#include "SyncEv.h"
#include "Nsa_LogFile.h"
#include "Nsa_GlobObjs.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace Nsa  {
   class TFileWriter;

/////////////////////////////////////////////////////////////////////////////
// class TFileWriterUnitBase
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TFileWriterUnitBase : public ref_counter<>
{
   TFile      File_;
   TMemQueue  MemQueue_;
   size_t     WriteCount_;

public:
   K_ctor TFileWriterUnitBase (const std::string&);

   K_mf(size_t) Polling ();

   inline K_mf(void) SetWriteCount (                 size_t size)  { WriteCount_ = size;                            }
   inline K_mf(void) PutQueue      (const void* buf, size_t size)  { MemQueue_.PutMemBlock(buf, size);              }
   inline K_mf(void) PutFile       (const void* buf, size_t size)  { File_    .Write      (buf, (Kway::dword)size); }

   inline K_mf(std::string) GetFileName () const  { return File_.GetFullName(); }
   inline K_mf(bool)        IsFinish    () const  { return MemQueue_.IsEmpty(); }
};
//---------------------------------------------------------------------------
typedef wise_ref<TFileWriterUnitBase>  TFileWriterUnit;

/////////////////////////////////////////////////////////////////////////////
// class TFileWriterHnd
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TFileWriterHnd
{
   friend class TFileWriter;

   TFileWriterUnit  Unit_;
   bool             UseQueue_;
   size_t           WriteCount_;

public:
   inline K_ctor TFileWriterHnd () : UseQueue_(true), WriteCount_(200)  {}
   
   K_mf(void) Reg           (const char*);
   K_mf(void) RegDaily      (const char* fileName, const char* subDir = NULL);
   K_mf(void) Unreg         ();
   K_mf(void) SetWriteCount (size_t);

   void WriteStr (ELogType, const char* fmt, ...);

   K_mf(void) WriteMem (ELogType logType, const void* data, size_t size, const TKDateTimeMS* dtms = NULL);
   K_mf(void) WriteMem (                  const void* data, size_t size);
   
   K_mf(void) WriteMsg (const TLogMsg&);

   inline K_mf(void) UseQueue (bool b)  { UseQueue_ = b; }

   inline K_mf(bool) IsValid () const  { return (Unit_.get() != NULL); }

protected:
   K_mf(void) Write (TFileWriterUnitBase*, const void* data, size_t size);
};

/////////////////////////////////////////////////////////////////////////////
// class TFileWriter
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TFileWriter : public TGlobObj
{
public:
   typedef std::map<std::string, TFileWriterUnit>  TRsUnit;
   typedef TRsUnit::iterator                       TRsUnitI;
   typedef std::pair<TRsUnitI, bool>               TRsUnitIns;

protected:
   class TImpl;
   friend class TImpl;

   TRsUnit     RsUnit_;
   TImpl*      Impl_;
   ost::Mutex  Mutex_;
   bool        Started_;

public:
   K_ctor  TFileWriter ();
   K_dtor ~TFileWriter ();

   //==============================================
   // overwrite TGlobObj virtual function
   inline K_mf(bool) KeepLast () const  { return true; }

   //==============================================
   // TFileWriter function
   K_mf(void) RegHnd (const std::string&, TFileWriterHnd&);
};

/////////////////////////////////////////////////////////////////////////////
// class TFileWriterGoh
/////////////////////////////////////////////////////////////////////////////
class TFileWriterGoh : public TGlobObjHnd<TFileWriter>
{
   typedef TGlobObjHnd<TFileWriter>  inherited;

public:
   inline K_ctor TFileWriterGoh (bool autoQuery = false)
      : inherited("Kway::Nsa::TFileWriter", autoQuery)
   {}
};

/////////////////////////////////////////////////////////////////////////////
// class TLogFileComm
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TLogFileComm : protected TFileWriterHnd
{
   typedef TFileWriterHnd  inherited;

   const void*  Owner_;
   int          RandNum_;

public:
   K_ctor TLogFileComm (const void*);

   K_mf(void) WriteEvent (ELogType, const void*, size_t);

   inline K_mf(void) OpenDaily (const char* fn)  { inherited::RegDaily(fn); }
   
   inline K_mf(void) UseQueue (bool b)  { inherited::UseQueue(b); }
   inline K_mf(void) Close    ()        { inherited::Unreg();     }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_FileWriter_h_)
