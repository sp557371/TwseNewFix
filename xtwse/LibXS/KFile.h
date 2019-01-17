//
// class TFile :檔案存取class
//
// 版本: $Id: KFile.h,v 1.11 2005/04/26 02:19:09 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef KFileH
#define KFileH
//---------------------------------------------------------------------------
#include "KTime.h"
//---------------------------------------------------------------------------
#if   defined(K_WINDOWS)
#  include <winsock2.h>//避免在 VS 2005 make 失敗!
#  include <windows.h>

#elif defined(K_UNIXLIKE)
#  include <sys/stat.h> //for S_Ixxxx
#  include <unistd.h>
#  include <fcntl.h>

#else
#  error "尚未支援..."
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
      /// Write()時Append到緩衝區,在另外的Thread裡面寫入.
      fmThreadAppend = fmAppend|0x0008,

      fmReadSeqBit   = 0x0020,
      fmReadSeq      = fmReadSeqBit|fmRead,
      fmRandomAccessBit = 0x0040,
      fmRandomAccess = fmRandomAccessBit|fmReadWrite,

      fmDupOpened    = 0x0080,   //若指定的檔案已開啟且FileMode一樣,則使用DUP的方式開檔.
                                 //如此一來,就可以允許在相同程式之中,
                                 //重複開啟 fmDenyRead 或 fmDenyWrite 的檔案.

//    fmOpenExist    = 0x0000,   //開啟已存在的檔案,若檔案不存在,則開檔失敗
                                 // => 不設定 fmOpenAlways => 就是 fmOpenExist
      fmOpenAlways   = 0x0100,   //若檔案不存在,則建立新檔(若目錄不存在,仍會開啟失敗)
                                 //若要自動建立目錄,則請增加 fmCreatePath
      fmTrunc        = 0x0200,
      fmCreatePath   = 0x0400|fmOpenAlways,
//    fmAutoFlush    = 0x0800,

      fmDenyNone     = 0x0000,
      fmDenyRead     = 0x1000,
      fmDenyWrite    = 0x2000,
      fmDenyAll      = fmDenyRead|fmDenyWrite,

      fmCheckFileNameCase = 0x8000,//在Windows之下, 檔名不分大小寫, 此時 File.a = File.A 可能會造成[檔案使用中錯誤]
                                   //加上此旗標, 則會檢查開啟的檔名是否符合大小寫, 如果不符, 則會自動將檔名變成 File_1_.a
                                   //此旗標僅在Windows下有效
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

   /// 當使用 fmThreadAppend 開檔時的輔助寫入工具.
   /// 沒有設定 fmThreadAppend 也可以用,
   /// 實際會在 TThreadAppender 解構時寫入.
   LIBNS_class TThreadAppender {
      TFile&      File_;
      std::string TmpBuf_;
      //禁止複製.
      K_ctor TThreadAppender (TThreadAppender&);
      void operator=(TThreadAppender&);
   public:
      K_ctor  TThreadAppender (TFile& f);
      K_dtor ~TThreadAppender ();
      /// 取出 str 之後請使用 str.append(), 如果清除 str, 或直接 assign(), 則會將尚未寫入的資料清掉!!
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
   K_mf(size_type)  Write    (const void* buf, dword size);//最多一次寫入0xffffffff bytes
   K_mf(size_type)  Write    (const char* str);
   K_mf(size_type)  WriteLN  (const char* str);
        size_type   printf   (const char* fmt, ...);
   K_mf(size_type)  Read     (void* buf, dword); //最多一次讀出0xffffffff bytes
   K_mf(size_type)  Read     (void* buf, dword recSize, dword n, const char* newLineSplit=0);

   //字串尾端會加上\0, buf不會包含換行字元, 是否有讀到換行字元,應從lnChar判斷,
   //傳回的size,包含\0. 傳回0表示 EOF 或 讀檔錯誤.
   K_mf(size_type)  ReadLN  (char* buf, dword bufSz, char& lnChar, const char* newLineSplit="\n\r");

   //Seek() error: return size_type(-1), success: return CurPos()
   K_mf(size_type)  Seek         (distance_type, TFileSeekFrom);
   K_mf(bool)       Flush        ();
   K_mf(bool)       SetFileSize  (size_type aNewSize);

          K_mf(TFile&)      operator=    (const TFile&);

   inline K_mf(bool)        operator!    () const  { return !IsOpened(); } /*operator!() 是否開檔成功*/
          K_mf(bool)        IsOpened     () const;
   inline K_mf(bool)        IsEOF        ()        { return(GetCurPos() >= GetFileSize()); }
          K_mf(size_type)   GetFileSize  () const  { return GetFileSize(0); }//若發生錯誤不會更新 LastErr
          K_mf(size_type)   GetFileSize  ();
          K_mf(size_type)   GetCurPos    ();

   inline K_mf(TFileMode)   GetFileMode  () const  { return FileMode_;   }
   inline K_mf(std::string) GetLastErr   () const  { return LastErrMsg_; }
   inline K_mf(std::string) GetOpenName  () const  { return OpenName_;   }//呼叫 Open() 時,使用的名稱,可包含%4y,%2y%2m,%2d
   inline K_mf(std::string) GetFileName  () const  { return FileName_;   }//實際的檔案名稱
          K_mf(std::string) GetFullName  () const; //取得完整路徑(含案名稱)

          K_mf(TKDateTime)  GetLastModifyTime (bool isUTC=true) const;
      #ifndef K_UNIXLIKE   // UNIX沒有支援 Creation time!
          K_mf(TKDateTime)  GetCreateTime     (bool isUTC=true) const;
      #endif

   inline K_mf(void)  ClearLastErr  ()    { LastErrMsg_.clear(); }
   inline K_mf(TFD)   GetFD() const       { return FD_; }

   /// 若開檔時有指定 %Edate, %4y, %2m, %2d, %2H,
   /// 則可呼叫 CheckReopen() 檢查是否需要重新開檔.
   /// 傳回檔案是否開啟成功(不一定有重新開檔).
   K_mf(bool) CheckReopen ();

   /// 將檔案內容讀入 fileCtx.
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

