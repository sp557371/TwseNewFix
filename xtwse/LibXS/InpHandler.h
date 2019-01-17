//
// class TInpHandler; �B�z�ϥΪ̿�J���r��
//
// $Id: InpHandler.h,v 1.4 2005/08/08 08:35:54 fonwin Exp $
//---------------------------------------------------------------------------
#ifndef InpHandlerH
#define InpHandlerH
//---------------------------------------------------------------------------
#include "pod_vector.h"
#include <ctype.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#ifdef K_WINDOWS
#include <cc++/thread.h>
#endif
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class TInpHandler;
//---------------------------------------------------------------------------
enum TInpCmdResult
{
   cr_QuitProgram,
   cr_InvalidCmd,
   cr_InvalidArgs,
   cr_OK,
};
//---------------------------------------------------------------------------
struct TInpCmdHandler
{
   /** �p�G��J���O: run   a1 b2  c3 d4
       �h cmd     = "run"
          cmdargs = "a1 b2  c3 d4"
   */
   virtual K_mf(TInpCmdResult)  OnCmd  (TInpHandler*, FILE* pout, const char* cmd, char* cmdargs) = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TInpHandler
{
public:
           K_ctor   TInpHandler  (bool isNeedRegDefaultCmds = true); 
   virtual K_dtor  ~TInpHandler  ();

   //�w�]�L�X: "ns$"
   virtual K_mf(void)  PrintPrompt  (FILE*);

   /** �B�z�R�O�C, �N cmdbuf, ���Ѧ� "cmd" + "arg-string",
       �éI�s�A�� TInpCmdHandler �B�z.
   */
   virtual K_mf(TInpCmdResult)  OnInpHandle  (FILE*, char* cmdbuf);

   static K_mf(char*)  SkipSpaces  (char* cmdbuf);
   static K_mf(char*)  FetchToken  (char** cmdbuf);

   /** ���U�R�O�B�z����. �C�өR�O�̪����o�W�L sizeof(TCmd) �Ӧr��.
   */
   K_mf(bool)  AddCmd  (const char* cmd, TInpCmdHandler& handler)    { return CmdHandlers_.add(TCmdHandler(cmd, handler)); }
   K_mf(bool)  DelCmd  (const char* cmd, TInpCmdHandler& handler)    { return CmdHandlers_.del(TCmdHandler(cmd, handler)); }
   K_mf(bool)  DelCmd  (TInpCmdHandler& handler)                     { if(!CmdHandlers_.del(handler))    return false;
                                                                       while(CmdHandlers_.del(handler)); return true; }

   /** cmd =
         "quit"   �����{��
         "exit"   �����{��
         "daemon" �h�ܦ� daemon (UNIX)
         "ml ..." ModuleLoader �аѦ� InpHandler.cpp ����@.
   */
   K_mf(void)  RegDefaultCmds  ();
private:
   typedef char   TCmd[16];
   struct TCmdHandler {
      TCmd              Cmd_;
      TInpCmdHandler*   Handler_;

      inline K_ctor  TCmdHandler (const char* cmd, TInpCmdHandler& handler)
               : Handler_(&handler)
               { strncpy(Cmd_, cmd, sizeof(TCmd)); }

      inline K_mf(bool)  operator!=  (const TCmdHandler& a) const { return !operator==(a); }
      inline K_mf(bool)  operator==  (const TCmdHandler& a) const
         { return(strncmp(Cmd_, a.Cmd_, sizeof(TCmd))==0  &&  Handler_==a.Handler_); }

      inline K_mf(bool)  operator!=  (const TInpCmdHandler& a) const  { return(Handler_!=&a); }
      inline K_mf(bool)  operator==  (const TInpCmdHandler& a) const  { return(Handler_==&a); }

      inline K_mf(bool)  operator!=  (const char* a) const  { return(strncmp(Cmd_, a, sizeof(TCmd))!=0); }
      inline K_mf(bool)  operator==  (const char* a) const  { return(strncmp(Cmd_, a, sizeof(TCmd))==0); }
   };
   typedef Kstl::pod_vector<TCmdHandler>   TCmdHandlers;
   TCmdHandlers   CmdHandlers_;
};
//---------------------------------------------------------------------------
#ifdef K_WINDOWS
class TWinConsoleInput : public ost::Thread
{
   char           Cmd_[1024*8];
   DWORD          MainThreadId_;
   TInpHandler*   InpHandler_;
   bool           Terminated_;
   HANDLE         HInput_;
   bool           IsTTY_;

   void run ();
public:
   K_ctor  TWinConsoleInput  (TInpHandler*);

   K_mf(int)  AppRun  ();
};
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
