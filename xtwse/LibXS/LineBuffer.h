//---------------------------------------------------------------------------
#ifndef LineBufferH
#define LineBufferH
//---------------------------------------------------------------------------
#include "Mes.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TLineBuffer
{
   char        Buffer_[1024*16];
   char        ProtectedBuffer_[1024];
   int         BufIdx_;
   int         UnknowBytes_;
   int         LastCheckIdx_;
   const char* LineSpliter_;

   virtual K_mf(void) ParseBuffer(char* ptr, char* eos) = 0;

protected:
   inline K_mf(int)  GetUnknowBytes () const    { return UnknowBytes_; }
   inline K_mf(void) AddUnknowBytes (int n)     { UnknowBytes_ += n;   }
   inline K_mf(void) ClearUnknowBytes()         { UnknowBytes_ =  0;   }
   inline K_mf(void) ClearBuffer    ()          { BufIdx_ = LastCheckIdx_ = 0; }
          K_mf(void) DebugBreak ();

   //傳回呼叫 ParseBuffer() 的次數(已處理的行數).
   // maxProcessTime = 最多處理時間(ms)
   //    : -1 不限(處理到沒資料為止)
   //    :  0 只處理1筆 or 沒資料
   K_mf(int) CheckRecv (Kway::TMesBase*, int maxProcessTime);

   K_mf(void) SetLineSpliter (const char* spl) {
      LineSpliter_ = spl;
   }

public:
   K_ctor TLineBuffer();
};
//---------------------------------------------------------------------------
} // namespace
#endif

