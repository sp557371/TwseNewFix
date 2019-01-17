/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_TextSimpReco.h"
#include <stdio.h>
#include "TextReader.h"
#include "Nsa_UtilStr.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
//---------------------------------------------------------------------------
static const char TextSimpReco_Default_Split[] = { "\r\n" };

/////////////////////////////////////////////////////////////////////////////
// class TTextSimpReco_Reader::TFileReader impl
/////////////////////////////////////////////////////////////////////////////
class TTextSimpReco_Reader::TFileReader : public TTextReader
{
   typedef TTextReader  inherited;

   TKpvStrings&  StrA_;

public:
   inline K_ctor TFileReader (const char* fileName, TKpvStrings& StrA)
      : inherited(fileName, fmRead)
      , StrA_    (StrA)
   {
      inherited::StartNotifyReadLN(sm_ReadAll);
   }

   // overwrite TTextReader virtual function
   K_mf(EOnLineResult) OnTextLine (const std::string& str)
   {
      if (str.empty()) {
         return olr_Continue;
      }
      
      StrA_.add(str);
      return olr_Continue;
   }
};


/////////////////////////////////////////////////////////////////////////////
// class TTextSimpReco_Reader impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTextSimpReco_Reader::TTextSimpReco_Reader (const char* cpFilePath)
   : FilePath_(cpFilePath)
{
   strcpy(NewLineSplit_, TextSimpReco_Default_Split);
}
//---------------------------------------------------------------------------

K_ctor TTextSimpReco_Reader::TTextSimpReco_Reader (const char* ccpFilePath, const char* ccpNewLineSplit)
   : FilePath_(ccpFilePath)
{
   strcpy(NewLineSplit_, ccpNewLineSplit);
}
//---------------------------------------------------------------------------

K_mf(std::string) TTextSimpReco_Reader::GetLastRec (size_t OneRecMaxSize)
{
   TFile            File(FilePath_.c_str(), fmRead);
   TFile::size_type FileSize = File.GetFileSize();
   TFile::size_type curSize  = OneRecMaxSize;

   if (FileSize == 0 || FileSize == NSA_KFILE_INVALID_SIZE)
      return std::string();

   if (curSize > FileSize)
      curSize = FileSize;

   TFile::size_type ResVal = File.Seek(curSize * -1, fsEnd);
   if (ResVal == NSA_KFILE_INVALID_SIZE) {
      return std::string();
   }

   size_t lenRead = (size_t)(curSize + 10);
   char*  bufRead = new char[lenRead];
   if (bufRead == NULL)
      return std::string();

   memset(bufRead, 0, lenRead);
   ResVal = File.Read(bufRead, (Kway::dword)curSize);
   if (ResVal == NSA_KFILE_INVALID_SIZE)
      return std::string();

   TStrSplit sp(bufRead, NewLineSplit_);

   delete [] bufRead;
   
   return sp.GetStr(sp.GetSize() - 1);
}
//---------------------------------------------------------------------------

K_mf(size_t) TTextSimpReco_Reader::GetAllRec (TKpvStrings& StrA)
{
   StrA.clear();

   TFileReader(FilePath_.c_str(), StrA);

   return StrA.size();
}


/////////////////////////////////////////////////////////////////////////////
// class TTextSimpReco_Writer impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTextSimpReco_Writer::TTextSimpReco_Writer (const char* cpFilePath)
   : FilePath_(cpFilePath)
{
   strcpy(NewLineSplit_, TextSimpReco_Default_Split);
}
//---------------------------------------------------------------------------

K_mf(void) TTextSimpReco_Writer::Append (const char* cpData)
{
   if (strlen(cpData) == 0) {
      return;
   }

   char  WriteBuf[1024];
   TFile File(FilePath_.c_str(), (TFileMode)(fmAppend|fmCreatePath));

   sprintf(WriteBuf, "%s%s", cpData, NewLineSplit_);
   File.Write(WriteBuf);
}
//---------------------------------------------------------------------------

K_mf(void) TTextSimpReco_Writer::SetAllRec (TKpvStrings& StrA)
{
   remove(FilePath_.c_str());
   
   char  WriteBuf[1024];
   TFile File(FilePath_.c_str(), (TFileMode)(fmAppend|fmTrunc|fmCreatePath));

   for (TKpvStrings::iterator itaStr=StrA.begin(); itaStr!=StrA.end(); ++itaStr) {
      sprintf(WriteBuf, "%s%s", itaStr->c_str(), NewLineSplit_);
      File.Write(WriteBuf);
   }
}
//---------------------------------------------------------------------------

