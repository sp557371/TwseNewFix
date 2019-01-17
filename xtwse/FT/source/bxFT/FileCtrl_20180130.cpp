//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FileCtrl.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_ctor TFileCtrl::TFileCtrl(const char* filename, size_t blocksize, eFileDir dir, bool isAppend)
    : FileName_ (filename),
      BlockSize_(blocksize),
      Direction_(dir)
{
  File_.Open(filename, Kway::TFileMode(Direction_==efdRead ? Kway::fmRead:
    (isAppend?(Kway::fmWrite | Kway::fmOpenAlways | Kway::fmCreatePath):
              (Kway::fmWrite | Kway::fmOpenAlways | Kway::fmCreatePath | Kway::fmTrunc))));
  if(File_.IsOpened())
    File_.Seek(0, (isAppend?Kway::fsEnd:Kway::fsBegin));
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TFileWrite::TFileWrite(const char* filename, size_t blocksize, size_t filesize, bool isAppend, size_t recsz)
    : inherited(filename, blocksize, efdWrite, isAppend)
{
  WorkSize_ = 0;
  RecSize_ = recsz;
}
//---------------------------------------------------------------------------
K_mf(size_t) TFileWrite::WriteTo(const char* data, size_t size)
{
   if(File_.IsOpened() == false)
      return false;
   size_t szwr = 0;
   if(RecSize_ > 0){
      size_t difsz = WorkSize_ % RecSize_;
      while(size > szwr){
         szwr += File_.Write(data + szwr, (RecSize_ < (size - szwr)? RecSize_:(size - szwr)) - difsz);
         difsz = 0;
         if((WorkSize_ + (size > szwr ? szwr:size)) % RecSize_ == 0)
            File_.Write("\n",1);
      }
   }else{
      size_t szwr = (size_t)File_.Write(data, size);
   }
   WorkSize_ += (size > szwr ? szwr:size);
   return szwr;
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TFileRead::TFileRead(const char* filename, size_t blocksize)
    : inherited(filename, blocksize, efdRead)
{
  WorkSize_ = 0;
}
//---------------------------------------------------------------------------
K_mf(size_t) TFileRead::ReadFrom(char* buffer, size_t size)
{
  if(!File_.IsOpened())
    return 0;  // fail
  if(size < BlockSize_)
    return 0;  // buffer size too small
  TFile::size_type szrd;
  if((szrd = File_.Read(buffer, size)) > 0)
    WorkSize_ += (size_t)szrd;
  return (size_t)szrd;
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TFileFilter::TFileFilter(const char* src, const char* dst)
{
  SrcFile_.Open(src, Kway::TFileMode(Kway::fmRead));
  DstFile_.Open(dst, Kway::TFileMode(Kway::fmWrite | Kway::fmOpenAlways | Kway::fmTrunc | Kway::fmCreatePath));
}
//---------------------------------------------------------------------------
K_dtor TFileFilter::~TFileFilter()
{
}
//---------------------------------------------------------------------------
K_mf(size_t) TFileFilter::Transfer(size_t recsize)
{
  size_t count=0;
  size_t recsz = recsize;
  if(SrcFile_.IsOpened() == false || DstFile_.IsOpened() == false)
    return count;
  if(recsize == 0){
     char buf[4096];            // 應該不會有一筆record 大於 4096 byte 吧!!
     size_t rdsz = SrcFile_.Read(buf, 4096);
     char* pos  = strchr(buf,'\n');
     if(pos){
        if(*(--pos) == '\r')
           recsz =  pos - buf;
        else
           recsz = pos - buf +1;
     }else
        recsz = SrcFile_.GetFileSize(); //如果沒有設定record長度也沒有換行字元時, 就將整個檔案當作一個record
     SrcFile_.Seek(0, Kway::fsBegin);
  }
  char* buffer = new char[recsz+1];
  char line;
  TFile::size_type readsize;
  while((readsize = SrcFile_.Read(buffer, recsz)) > 0)
  {
    if(readsize == recsz) // record correct
    {
      if(DstFile_.Write(buffer, recsz) == recsz)
        count++;
      while((readsize = SrcFile_.Read(&line, 1)) == 1)
        if(line == '\r' || line == '\n')
          continue;
        else
        {
          SrcFile_.Seek(-1, Kway::fsCurrent);
          break;
        }
    }
    else
    {
      count = 0;
      break;
    }
  }
  delete [] buffer;
  return count;
}
//---------------------------------------------------------------------------



