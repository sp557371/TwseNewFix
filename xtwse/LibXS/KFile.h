//
// class TFile :�ɮצs��class
//
// ����: $Id: KFile.h,v 1.11 2005/04/26 02:19:09 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef KFileH
#define KFileH
//---------------------------------------------------------------------------
#include "KTime.h"
//---------------------------------------------------------------------------
#if   defined(K_WINDOWS)
#  include <winsock2.h>//�קK�b VS 2005 make ����!
#  include <windows.h>

#elif defined(K_UNIXLIKE)
#  include <sys/stat.h> //for S_Ixxxx
#  include <unistd.h>
#  include <fcntl.h>

#else
#  error "�|���䴩..."
#endif
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
enum TFileMode {
      fmRead         = 0x0001,
      fmWrite        = 0x0002,
      fmReadWrite    = fmRead|fmWrite,

      fmAppendBit    = 0x0010,
      fmAppend       = fmAppendBit|fmWrite,
      /// Write()��Append��w�İ�,�b�t�~��Thread�̭��g�J.
      fmThreadAppend = fmAppend|0x0008,

      fmReadSeqBit   = 0x0020,
      fmReadSeq      = fmReadSeqBit|fmRead,
      fmRandomAccessBit = 0x0040,
      fmRandomAccess = fmRandomAccessBit|fmReadWrite,

      fmDupOpened    = 0x0080,   //�Y���w���ɮפw�}�ҥBFileMode�@��,�h�ϥ�DUP���覡�}��.
                                 //�p���@��,�N�i�H���\�b�ۦP�{������,
                                 //���ƶ}�� fmDenyRead �� fmDenyWrite ���ɮ�.

//    fmOpenExist    = 0x0000,   //�}�Ҥw�s�b���ɮ�,�Y�ɮפ��s�b,�h�}�ɥ���
                                 // => ���]�w fmOpenAlways => �N�O fmOpenExist
      fmOpenAlways   = 0x0100,   //�Y�ɮפ��s�b,�h�إ߷s��(�Y�ؿ����s�b,���|�}�ҥ���)
                                 //�Y�n�۰ʫإߥؿ�,�h�мW�[ fmCreatePath
      fmTrunc        = 0x0200,
      fmCreatePath   = 0x0400|fmOpenAlways,
//    fmAutoFlush    = 0x0800,

      fmDenyNone     = 0x0000,
      fmDenyRead     = 0x1000,
      fmDenyWrite    = 0x2000,
      fmDenyAll      = fmDenyRead|fmDenyWrite,

      fmCheckFileNameCase = 0x8000,//�bWindows���U, �ɦW�����j�p�g, ���� File.a = File.A �i��|�y��[�ɮרϥΤ����~]
                                   //�[�W���X��, �h�|�ˬd�}�Ҫ��ɦW�O�_�ŦX�j�p�g, �p�G����, �h�|�۰ʱN�ɦW�ܦ� File_1_.a
                                   //���X�жȦbWindows�U����
};
//---------------------------------------------------------------------------
enum TFileSeekFrom {
#if defined(K_WINDOWS)
      fsBegin   = FILE_BEGIN,
      fsCurrent = FILE_CURRENT,
      fsEnd     = FILE_END,
#else
      fsBegin   = SEEK_SET,
      fsCurrent = SEEK_CUR,
      fsEnd     = SEEK_END,
#endif
};
//---------------------------------------------------------------------------
LIBNS_class TFile
{
protected:
   std::string    LastErrMsg_;
   std::string    OpenName_;
   std::string    FileName_;
   TFileMode      FileMode_;
#if   defined(K_WINDOWS)
   typedef HANDLE TFD;
   typedef int    mode_t;
   static const mode_t  defaultAccessMode    = 0;
   static const mode_t  defaultDirAccessMode = 0;
#else
   typedef int TFD;
   static const mode_t  defaultAccessMode    = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   static const mode_t  defaultDirAccessMode = S_IRUSR | S_IWUSR | S_IXUSR
                                             | S_IRGRP |           S_IXGRP
                                             | S_IROTH |           S_IXOTH;
#endif
   TFD  FD_;

   class TThrAppImpl;
   TThrAppImpl* ThrAppImpl_;
   friend class TThrAppImpl;

   K_mf(void) FlushThreadAppend();
   K_mf(void) StartThreadAppend();

public:
   //types.
   typedef uint64    size_type;
   typedef int64     distance_type;
   static inline bool IsResultOK (size_type a) { return(a!=size_type(-1)); }

   /// ��ϥ� fmThreadAppend �}�ɮɪ����U�g�J�u��.
   /// �S���]�w fmThreadAppend �]�i�H��,
   /// ��ڷ|�b TThreadAppender �Ѻc�ɼg�J.
   LIBNS_class TThreadAppender {
      TFile&      File_;
      std::string TmpBuf_;
      //�T��ƻs.
      K_ctor TThreadAppender (TThreadAppender&);
      void operator=(TThreadAppender&);
   public:
      K_ctor  TThreadAppender (TFile& f);
      K_dtor ~TThreadAppender ();
      /// ���X str ����Шϥ� str.append(), �p�G�M�� str, �Ϊ��� assign(), �h�|�N�|���g�J����ƲM��!!
      K_mf(std::string&) GetAppender ();
      K_mf(void) Append (const void* dat, dword size);
   };
   friend LIBNS_class TThreadAppender;

   //constructor/destructor.
        K_ctor  TFile  ();
        K_ctor  TFile  (const char* fileName, TFileMode,
                        mode_t unixAccessMode = defaultAccessMode,
                        mode_t dirAccessMode  = defaultDirAccessMode);
        K_ctor  TFile  (const char* fileName, TFileMode, const char* fileDesp,
                        mode_t unixAccessMode = defaultAccessMode,
                        mode_t dirAccessMode  = defaultDirAccessMode);
        K_ctor  TFile  (const TFile&);
virtual K_dtor ~TFile  ();

   //Open/Close
   K_mf(bool)  Open   (const char* fileName, TFileMode,
                       mode_t unixAccessMode = defaultAccessMode,
                       mode_t dirAccessMode  = defaultDirAccessMode);
   K_mf(bool)  Open   (const char* fileName, TFileMode, const char* fileDesp,
                       mode_t unixAccessMode = defaultAccessMode,
                       mode_t dirAccessMode  = defaultDirAccessMode);
   K_mf(bool)  Close  ();

   //Read/Write: error: return size_type(-1)
   K_mf(size_type)  Write    (const void* buf, dword size);//�̦h�@���g�J0xffffffff bytes
   K_mf(size_type)  Write    (const char* str);
   K_mf(size_type)  WriteLN  (const char* str);
        size_type   printf   (const char* fmt, ...);
   K_mf(size_type)  Read     (void* buf, dword); //�̦h�@��Ū�X0xffffffff bytes
   K_mf(size_type)  Read     (void* buf, dword recSize, dword n, const char* newLineSplit=0);

   //�r����ݷ|�[�W\0, buf���|�]�t����r��, �O�_��Ū�촫��r��,���qlnChar�P�_,
   //�Ǧ^��size,�]�t\0. �Ǧ^0��� EOF �� Ū�ɿ��~.
   K_mf(size_type)  ReadLN  (char* buf, dword bufSz, char& lnChar, const char* newLineSplit="\n\r");

   //Seek() error: return size_type(-1), success: return CurPos()
   K_mf(size_type)  Seek         (distance_type, TFileSeekFrom);
   K_mf(bool)       Flush        ();
   K_mf(bool)       SetFileSize  (size_type aNewSize);

          K_mf(TFile&)      operator=    (const TFile&);

   inline K_mf(bool)        operator!    () const  { return !IsOpened(); } /*operator!() �O�_�}�ɦ��\*/
          K_mf(bool)        IsOpened     () const;
   inline K_mf(bool)        IsEOF        ()        { return(GetCurPos() >= GetFileSize()); }
          K_mf(size_type)   GetFileSize  () const  { return GetFileSize(0); }//�Y�o�Ϳ��~���|��s LastErr
          K_mf(size_type)   GetFileSize  ();
          K_mf(size_type)   GetCurPos    ();

   inline K_mf(TFileMode)   GetFileMode  () const  { return FileMode_;   }
   inline K_mf(std::string) GetLastErr   () const  { return LastErrMsg_; }
   inline K_mf(std::string) GetOpenName  () const  { return OpenName_;   }//�I�s Open() ��,�ϥΪ��W��,�i�]�t%4y,%2y%2m,%2d
   inline K_mf(std::string) GetFileName  () const  { return FileName_;   }//��ڪ��ɮצW��
          K_mf(std::string) GetFullName  () const; //���o������|(�t�צW��)

          K_mf(TKDateTime)  GetLastModifyTime (bool isUTC=true) const;
      #ifndef K_UNIXLIKE   // UNIX�S���䴩 Creation time!
          K_mf(TKDateTime)  GetCreateTime     (bool isUTC=true) const;
      #endif

   inline K_mf(void)  ClearLastErr  ()    { LastErrMsg_.clear(); }
   inline K_mf(TFD)   GetFD() const       { return FD_; }

   /// �Y�}�ɮɦ����w %Edate, %4y, %2m, %2d, %2H,
   /// �h�i�I�s CheckReopen() �ˬd�O�_�ݭn���s�}��.
   /// �Ǧ^�ɮ׬O�_�}�Ҧ��\(���@�w�����s�}��).
   K_mf(bool) CheckReopen ();

   /// �N�ɮפ��eŪ�J fileCtx.
   static K_mf(bool) ReadAll (const char* fileName, std::string& fileCtx);

private:
   K_mf(size_type) GetFileSize (dword* ErrCode) const;
   K_mf(size_type) WriteImpl (const void* buf, dword size);
};
//---------------------------------------------------------------------------
LIBNS_fn(std::string) ReplaceFileName (const char* fileName, TKDateTime dt = TKDateTime::Now());
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

