//---------------------------------------------------------------------------
#ifndef FileCtrlH
#define FileCtrlH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "bxFT.h"
#include "KFile.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TFileCtrl
{
protected:
enum eFileDir { efdWrite, efdRead };
    eFileDir    Direction_;
    TFile       File_;
    std::string FileName_;  // 檔名
    size_t      WorkSize_;  // 工作累計的大小
    size_t      BlockSize_; // 每一區塊的大小
    size_t      RecSize_;   // 每筆record的長度(RecSize_ > 0 時,會將每筆record 加上換行字元後再行寫檔,預設為0)
public:
    K_ctor TFileCtrl (const char* filename, size_t blocksize, eFileDir, bool isAppend = false);    // bInput 資料流向
    K_dtor ~TFileCtrl(){}
    K_mf(bool)  IsWorkOK()      { return (File_.IsOpened() ? WorkSize_==File_.GetFileSize():false); }
    K_mf(size_t)GetFileSize()   { return (size_t)File_.GetFileSize(); }
};
//---------------------------------------------------------------------------
K_class TFileWrite : public TFileCtrl   // 僅負責寫檔
{
    typedef TFileCtrl   inherited;
    typedef TFileWrite  this_type;
public:
    K_ctor  TFileWrite(const char* filename, size_t blocksize, size_t filesize, bool, size_t recsz = 0);
    K_mf(size_t) WriteTo(const char* data, size_t size);
};
//---------------------------------------------------------------------------
K_class TFileRead : public TFileCtrl    // 僅負責讀檔
{
    typedef TFileCtrl   inherited;
    typedef TFileRead   this_type;
protected:
public:
    K_ctor  TFileRead    (const char* filename, size_t blocksize);
    K_mf(size_t) ReadFrom(char*, size_t);
};
//---------------------------------------------------------------------------
K_class TFileFilter // 僅負責將檔案裡的換行字元過濾掉
{
    typedef TFileFilter   this_type;
protected:
    TFile   SrcFile_;
    TFile   DstFile_;
public:
    K_ctor TFileFilter(const char* src, const char* dst);
    K_dtor ~TFileFilter();
    K_mf(size_t) Transfer(size_t recsize);
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
