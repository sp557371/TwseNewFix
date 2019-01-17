/////////////////////////////////////////////////////////////////////////////
#if !defined(_KIniFile_h_)
#define _KIniFile_h_
//---------------------------------------------------------------------------
#include "ev_map.h"
#include "ViewArea.h"
/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TKIniFile
/////////////////////////////////////////////////////////////////////////////
K_class TKIniFile
{
public:
//   typedef Kstl::ev_map<std::string, std::string>  TSection;
// 20090907 第二層的value 改用 std::vector<std::string> TStrings型別以支援multiple-value
   typedef Kstl::ev_map<std::string, TStrings>     TSection;
   typedef TSection::iterator                      TSectionI;
   typedef Kstl::ev_map<std::string, TSection>     TSections;
   typedef TSections::iterator                     TSectionsI;

protected:
   class TReader;
   
   TSections    Sections_;
   std::string  FilePath_;
   bool         IsModify_;
   // 在 windows 上是使用 ';'
   // 在 unix    上是使用 '#'
   char         RemarkChar_;

public:
   K_ctor  TKIniFile (char remChar = ' ');
   K_ctor  TKIniFile (const char*, char remChar = ' ');
   K_dtor ~TKIniFile ();

   //==============================================
   // Readxxx
   K_mf(bool)        ReadBool   (const std::string&, const std::string&, bool               bDefault);
   K_mf(int)         ReadInt    (const std::string&, const std::string&, int                nDefault);
// 20090907 ReadString 供讀取 single-value 使用
   K_mf(std::string) ReadString (const std::string&, const std::string&, const std::string& strDefault);
// 20090907 ReadStrings 供讀取 multiple-value 使用
   K_mf(int)         ReadStrings(const std::string& strSection, const std::string& strIdent, TStrings& strs);

   //==============================================
   // Writexxx
   K_mf(void) WriteBool   (const std::string&, const std::string&, bool               bNewVal);
   K_mf(void) WriteInt    (const std::string&, const std::string&, int                nNewVal);
// 20090907 WriteString 供寫入single-value 使用
   K_mf(void) WriteString (const std::string&, const std::string&, const std::string& strNewVal);
// 20090907 WriteStrings 供寫入multiple-value 使用
   K_mf(void) WriteStrings(const std::string&, const std::string&, const std::string& strNewVal);

   // 新增一個 Indent 的 Value, 並檢查 Indent 的數量
   // 如果大於指定數量, 則從頭開始移除
   K_mf(void) AppendValue (const std::string&, const std::string&, const std::string&, size_t);

   K_mf(void) FileRead  (const std::string&);
   K_mf(void) FileWrite ();

   K_mf(bool) GetBool (const std::string&) const;
   K_mf(void) SetBool (      std::string&, bool nNewVal);
// 20090907 刪除一個section
   K_mf(void) EraseSection(const std::string&);
// 20090907 刪除一個Indent
   K_mf(void) EraseIndent(const std::string&, const std::string&);
// 20090907 刪除一個Value, 己無value 時同時刪除indent
   K_mf(void) EraseValue(const std::string&, const std::string&, const std::string& strval);
// 20091009 複製一個section
   K_mf(bool) CopySection(const std::string&, const std::string&);

   inline K_mf(std::string) ReadString (const char* ccpSection, const char* ccpIdent, const char* ccpDefault)
      { return ReadString(std::string(ccpSection), std::string(ccpIdent), std::string(ccpDefault)); }
         
   inline K_mf(void) FileRead  ()  { FileRead(FilePath_);         }
   inline K_mf(void) Flush     ()  { if (IsModify_)  FileWrite(); }
   inline K_mf(void) Clear     ()  { Sections_.clear();           }
   
   inline K_mf(TSections*)         GetSections ()  { return &Sections_; }
   inline K_mf(const std::string&) GetFilePath ()  { return FilePath_;  }

protected:
   K_mf(bool)         Read          (const std::string& strSection, const std::string& strIdent, std::string& strVal);
   K_mf(std::string&) GetVal        (const std::string& strSection, const std::string& strIdent);
   K_mf(std::string)  MakeLowercase (const std::string&) const;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_KIniFile_h_)
