/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ApaSesSmpFix.h"
#include "Nsa_LineBuf.h"
#include "src/Code/Message.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpFix impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TApaSesSmpFix::TApaSesSmpFix ()
   : inherited   ()
   , IsAutoFlush_(false)
{
}
//---------------------------------------------------------------------------

K_dtor TApaSesSmpFix::~TApaSesSmpFix ()
{
}
//---------------------------------------------------------------------------
//
//K_mf(void) TApaSesSmpFix::OnSmpLogon ()
//{
//   //==============================================
//   // 詢問 appia 目前有那些 SessionID
//   inherited::SmpGetMiddlewareInfoRequest();
//}
//---------------------------------------------------------------------------

K_mf(TFixMsgType2) TApaSesSmpFix::GetFixMsgType (const SSmpMsgMiddlewareEvent& data, TMessageRef msgRef, const char* caller)
{
   char         logMsg[512];
   int          logLen;
   TFixMsgType2 msgType;

   if (strncmp(data.Protocol_.c_str(), "FIX", 3) != 0) {
      logLen = sprintf(logMsg, "<TApaSesSmpFix::%s> Protocol[%s] 不是 FIX!", caller, data.Protocol_.c_str());
      inherited::LogFileHnd_.WriteMem(elt_Error, logMsg, logLen);
      return msgType;
   }

   try {
      msgRef->setString(data.EventData_, false);
   }
   catch (Exception& ex) {
      TLineBufNew line;

      // "<TApaSesSmpFix::%s> setString [%s] exception [%s]"
      logLen = sprintf(logMsg, "<TApaSesSmpFix::%s> setString [", caller);
      line.Append(logMsg, logLen);
      line.Append(data.EventData_);
      line.Append("] exception [");
      line.Append(ex.what());
      line.Append("]", 1);

      inherited::LogFileHnd_.WriteMem(elt_Error, line.GetCStr(), line.GetUsedSize());
      
      return msgType;
   }
   
   Header& header = msgRef->getHeader();
   MsgType qfMsgType;

   if (header.findField(qfMsgType) == false) {
      logLen = sprintf(logMsg, "<TApaSesSmpFix::%s> 找不到 MsgType!", caller);
      inherited::LogFileHnd_.WriteMem(elt_Error, logMsg, logLen);
      return msgType;
   }

   msgType.assign(qfMsgType);
   if (msgType.is_invalid()) {
      logLen = sprintf(logMsg, "<TApaSesSmpFix::%s> MsgType empty!", caller);
      inherited::LogFileHnd_.WriteMem(elt_Error, logMsg, logLen);
   }

   return msgType;
}
//---------------------------------------------------------------------------
