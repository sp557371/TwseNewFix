//---------------------------------------------------------------------------
#include "FileLog.h"
#include "ExtDef.h"
//FileLog	appLog;
//---------------------------------------------------------------------------
K_ctor FileLog::FileLog()//char* fileName)
{
   std::string logname = GetProcLogPath() + "/AppTrace.log";
   Log_.Open(logname.c_str(), (TFileMode)(fmCreatePath|fmOpenAlways|fmAppend));
}
//---------------------------------------------------------------------------
K_mf(bool) FileLog::log(char* buf, int len)
{
   Log_.Write(TKDateTime::Now().as_string().c_str());
   Log_.Write("---");
   char buf2[1050] = {0};
   if(1050>len+2){
      memcpy(buf2,buf,len);
      buf2[len]='\r';
      buf2[len+1]='\n';
   }else{
      //char temp[35] = "packet more than 250 bytes\r\n";
      strcpy(buf2, "packet more than 1050 bytes\r\n");
      //memcpy(buf2,temp,strlen(temp)+1);
   }
   if((len+2)==Log_.Write(buf2, len+2))
      return true;
   else
      return false;
}
//--------------------------------------------------------------------------- 
K_ctor MesLog::MesLog(const char* filename)
{
   std::string logname = GetProcLogPath() + filename;
   Log_.Open(logname.c_str(), (TFileMode)(fmCreatePath|fmOpenAlways|fmAppend));
}
//--------------------------------------------------------------------------- 

