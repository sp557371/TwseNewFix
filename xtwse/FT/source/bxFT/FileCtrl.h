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
    std::string FileName_;  // �ɦW
    size_t      WorkSize_;  // �u�@�֭p���j�p
    size_t      BlockSize_; // �C�@�϶����j�p
    size_t      RecSize_;   // �C��record������(RecSize_ > 0 ��,�|�N�C��record �[�W����r����A��g��,�w�]��0)
public:
    K_ctor TFileCtrl (const char* filename, size_t blocksize, eFileDir, bool isAppend = false);    // bInput ��Ƭy�V
    K_dtor ~TFileCtrl(){}
    K_mf(bool)  IsWorkOK()      { return (File_.IsOpened() ? WorkSize_==File_.GetFileSize():false); }
    K_mf(size_t)GetFileSize()   { return (size_t)File_.GetFileSize(); }
};
//---------------------------------------------------------------------------
K_class TFileWrite : public TFileCtrl   // �ȭt�d�g��
{
    typedef TFileCtrl   inherited;
    typedef TFileWrite  this_type;
public:
    K_ctor  TFileWrite(const char* filename, size_t blocksize, size_t filesize, bool, size_t recsz = 0);
    K_mf(size_t) WriteTo(const char* data, size_t size);
};
//---------------------------------------------------------------------------
K_class TFileRead : public TFileCtrl    // �ȭt�dŪ��
{
    typedef TFileCtrl   inherited;
    typedef TFileRead   this_type;
protected:
public:
    K_ctor  TFileRead    (const char* filename, size_t blocksize);
    K_mf(size_t) ReadFrom(char*, size_t);
};
//---------------------------------------------------------------------------
K_class TFileFilter // �ȭt�d�N�ɮ׸̪�����r���L�o��
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
