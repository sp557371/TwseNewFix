//---------------------------------------------------------------------------
/*
   class TSesBuffer;
   class TSesTextline;
*/
//
// $Id: SesBufferLine.h,v 1.1 2004/02/04 08:22:09 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesBufferLineH
#define SesBufferLineH
//---------------------------------------------------------------------------
#include "Ses.h"
#include <memory>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesBuffer : public TSesBase
{
   typedef TSesBase  inherited;

   K_mf(void)  OnMesLinkReady  (TMesBase*);
   K_mf(void)  OnMesLinkFail   (TMesBase*, const std::string&);//do nothing
   K_mf(void)  OnMesLinkBroken (TMesBase*, const std::string&);//do nothing
   K_mf(void)  OnMesRecvReady  (TMesBase*, size_t);
   K_mf(void)  SkipBytes       (byte*, size_t);
   //¡õreturn: how many bytes processed
   virtual K_mf(size_t)  OnBufferRecv    (byte*, size_t sz) = 0;
   virtual K_mf(size_t)  OnBufferFull    (byte*, size_t sz);//default: return sz
protected:
   inline K_mf(byte*) GetBuffer ()  { return Buffer_.get() + PrvReserve_; }
   inline K_mf(void)  ClearRecv ()  { RecvPos_ = 0; }

public:
   K_ctor  TSesBuffer  (size_t bufferSize, size_t prvReserve = 0);

private:
   std::auto_ptr<byte>  Buffer_;
   size_t               RecvPos_;
   size_t               BufSize_;
   size_t               PrvReserve_;
};
//---------------------------------------------------------------------------
LIBNS_class TSesTextline : public TSesBuffer
{
   typedef TSesBuffer  inherited;
   K_mf(size_t)  OnBufferRecv  (byte*, size_t);
   K_mf(size_t)  OnBufferFull  (byte*, size_t);

   virtual K_mf(void)  OnTextline  (const char*, size_t sz) = 0;
public:
   K_ctor  TSesTextline (size_t maxLineSize);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
