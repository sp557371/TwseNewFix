/**
   [檔案合併]管理員
*/
//---------------------------------------------------------------------------
#ifndef nsfmerg_mgrH
#define nsfmerg_mgrH
//---------------------------------------------------------------------------
#include "nsfmerg_obj.h"
#include "libnsfmerg.h"
#include "ServerList.h"
#include <list>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
FMERG_class TFileMergMgr
{
public:
   /** [檔案合併]設定物件
   */
   FMERG_class TIni : public TTokenFile {
      typedef TTokenFile   inherited;
   public:
      K_ctor  TIni  (const std::string& iniFileName);
      K_ctor  TIni  ();

      /** 設定 log 檔的檔案名稱.
      */
      K_mf(void)  SetLogFileName  (const std::string&);

      /** lndata = 合併目的地檔名(文字檔).
           例如:   ./etmpdata/log/%Edate/blb/ts/ts001.api0.log
             fmt = [S][S][-n-]  格式請參閱 TTokenStr的格式說明(TokenStr.h)
                  n 表示除了此行之外,還要再讀取n bytes,才連同此行一併寫入目的檔.
      */
      K_mf(void)  AddDestinFile  (const std::string& destinFileName, const std::string& fmt);

      /** lndata = 合併目的地檔名
           例如:   ./etmpdata/log/%Edate/blb/ts/rectest.bin
           recSz = 每筆資料的大小.
      */
      K_mf(void)  AddDestinFile  (const std::string& destinFileName, size_t recSz);

      /** 在呼叫 AddDestinFile() 之後, 接下來要呼叫 AddSourceFile() 加入合併的來源檔.
      */
      K_mf(void)  AddSourceFile  (const std::string& sourceFileName);

   private:
      friend class TFileMergMgr;
      struct TSourceRec {
         inline K_ctor  TSourceRec  ()  : SourcePosFrom_(0), LogFileIdx_(0) {}
         TFile::size_type  SourcePosFrom_;
         size_t            LogFileIdx_;
      };
      struct TFileNames : public std::map<std::string,TSourceRec> {
         std::string    TxtFormat_;
      };
      typedef std::map<std::string,TFileNames>  TMergs;
      std::string LogFileName_;
      TMergs      Mergs_;
      std::string CurLabel_;
      TServerList ServerList_;

      K_mf(void)  OnGetParam    (char* opname, char* param);
      K_mf(void)  OnGetLabel    (const char* labelName);
      K_mf(void)  OnUnknowLine  (const std::string& lndata);
   };

   /** Mgr的建構/解構
   */
   K_ctor  TFileMergMgr  (const std::string& iniFileName);
   K_ctor  TFileMergMgr  (TIni& ini);
   K_ctor  TFileMergMgr  ();
   K_dtor ~TFileMergMgr  ();

   /** 設定[檔案合併]參數.
   */
   K_mf(void)  SetIni  (TIni& ini);

protected:
   typedef std::list<TMergObj>   TMergs;
   TMergs   Mergs_;

private:
   TFile    LogFile_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
