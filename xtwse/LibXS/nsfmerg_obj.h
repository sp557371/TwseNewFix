/**
   單一[檔案合併]物件.
   可讀取數個來源檔,寫入一個目的檔.
*/
//---------------------------------------------------------------------------
#ifndef nsfmerg_objH
#define nsfmerg_objH
//---------------------------------------------------------------------------
#include "TextReader.h"
#include <map>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMergObj
{
public:
   /** 建構
   */
   K_ctor  TMergObj  (TFile& mergLogFile, const char* destinFileName, size_t recSize);
   K_ctor  TMergObj  (TFile& mergLogFile, const char* destinFileName, const std::string& txtFormat);

   /** 每筆資料的大小, 傳回 0 則為文字檔(以換行字元做為結束).
   */
   inline K_mf(size_t)  GetRecSize  () const  { return RecSize_; }

   /** 傳回文字檔的格式.
   */
   inline K_mf(const std::string&)  GetTxtFormat  () const  { return TxtFormat_; }

   /** 來源檔處理物件
   */
   LIBNS_class TMergSource : protected TTextReader {
      typedef TTextReader  inherited;
      TMergObj*   MergObj_;
      size_t      LogFileIdx_;
      std::string TxtFormat_;
      std::string TxtBuffer_;
      K_mf(EOnLineResult)  OnTextLine  (const std::string&);
      K_mf(EOnLineResult)  OnNextLine  (const std::string&);
      K_mf(void)           SetNext     ();
   public:
      K_ctor  TMergSource  (const TMergSource&);
      K_ctor  TMergSource  (TMergObj*,
                            const char* fileName,
                            size_type   sourcePosFrom,
                            size_t      logFileIdx,
                            void*       userDefined = 0);

      using inherited::GetCurPos;
      using inherited::StartNotifyReadLN;
      using inherited::GetFullName;
      inline K_mf(size_t)  GetLogFileIdx  () const   { return LogFileIdx_; }

      void* UserDefined_;
   }; // class TMergSource

   /** 當來源檔收到[一行文字]資料時,由此函式處理.此函式會進行文字格式分析:
       傳回 0, 表示此行已被處理.
       傳回 >0, 表示還需要再等 n(傳回值)bytes.
   */
   K_mf(size_t)  OnSourceText  (TMergSource*, const std::string&);

   /** 當來源檔收到一筆資料時,由此函式處理.
       寫入合併目的地檔之前,您可以先看看內容,
       如果您決定不寫入合併檔,請呼叫:WriteSourceIndex()
   */
   virtual K_mf(void)  OnSourceData  (TMergSource*, const std::string&);

   /** 增加一個來源檔.
       userDefined可以自訂一個指標型別的資料.
   */
   K_mf(bool)  AddSource  (const std::string& sourceFileName,
                           TFile::size_type   sourcePosFrom,
                           size_t             logFileIdx,
                           void*              userDefined = 0);
   /** 取得合併目的地檔案.
   */
   K_mf(TFile&)  GetDestinFile  ()  { return Destin_; }

   /** 取得來源檔的數量.
   */
   K_mf(size_t)  GetSourceCount  () const   { return Sources_.size(); }

protected:
   K_mf(void) WriteSourceIndex (TMergSource*);

private:

   typedef std::map<std::string,TMergSource> TSources;

   TFile*      MergLogFile_;
   TFile       Destin_;
   size_t      RecSize_;
   std::string TxtFormat_;
   int         TkrIdx_;
   TSources    Sources_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

