/**
   [�ɮצX��]�޲z��
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
   /** [�ɮצX��]�]�w����
   */
   FMERG_class TIni : public TTokenFile {
      typedef TTokenFile   inherited;
   public:
      K_ctor  TIni  (const std::string& iniFileName);
      K_ctor  TIni  ();

      /** �]�w log �ɪ��ɮצW��.
      */
      K_mf(void)  SetLogFileName  (const std::string&);

      /** lndata = �X�֥ت��a�ɦW(��r��).
           �Ҧp:   ./etmpdata/log/%Edate/blb/ts/ts001.api0.log
             fmt = [S][S][-n-]  �榡�аѾ\ TTokenStr���榡����(TokenStr.h)
                  n ��ܰ��F���椧�~,�٭n�AŪ��n bytes,�~�s�P����@�ּg�J�ت���.
      */
      K_mf(void)  AddDestinFile  (const std::string& destinFileName, const std::string& fmt);

      /** lndata = �X�֥ت��a�ɦW
           �Ҧp:   ./etmpdata/log/%Edate/blb/ts/rectest.bin
           recSz = �C����ƪ��j�p.
      */
      K_mf(void)  AddDestinFile  (const std::string& destinFileName, size_t recSz);

      /** �b�I�s AddDestinFile() ����, ���U�ӭn�I�s AddSourceFile() �[�J�X�֪��ӷ���.
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

   /** Mgr���غc/�Ѻc
   */
   K_ctor  TFileMergMgr  (const std::string& iniFileName);
   K_ctor  TFileMergMgr  (TIni& ini);
   K_ctor  TFileMergMgr  ();
   K_dtor ~TFileMergMgr  ();

   /** �]�w[�ɮצX��]�Ѽ�.
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
